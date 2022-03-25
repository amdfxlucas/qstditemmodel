/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/



#include "qstditemmodel.h"

#include "qstditemmodel_commands.h"

#include "qstditemmodel_p.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qpair.h>
#include <QtCore/qvariant.h>
#include <QtCore/qstringlist.h>
#include <QTimer>
#include <QtCore/qbitarray.h>
#include <QtCore/qmimedata.h>
#include <QtCore/qiodevice.h>
#include <QtCore/private/qduplicatetracker_p.h>

#include "scope_tagger.h"

#include <qdebug.h>
#include <algorithm>

#include "qstditemcommands.h"

//QT_BEGIN_NAMESPACE


 #include <QFile>
#include <QSaveFile>

#include "aqp.hpp"

QUndoCommand* lastChildofCmd(const QUndoCommand* cmd)
{
    //  for(unsigned int i{0}; i < cmd->childCount();++i )
    // {          }

   const QUndoCommand* child;

    child = cmd->child(cmd->childCount()-1);

    if(child->childCount()>0)
    {return lastChildofCmd(child);
    }
    else
    {return const_cast<QUndoCommand*>(child);
    }
}

static inline QString qStandardItemModelDataListMimeType()
{
   return QStringLiteral("application/x-qstandarditemmodeldatalist");
}



/*!
    Constructs a new item model with the given \a parent.
*/
QStdItemModel::QStdItemModel(QObject *parent)
    : QAbstractItemModel(*new QStdItemModelPrivate, parent)
{
    Q_D(QStdItemModel);
    m_stack= new UndoStack(this);
       m_stack->beginMacro("QStdItemModel::QStdItemModel");
    d->init();
    //d->root->d_func()->setModel(this);
    d->root->setModel(this);
       m_stack->endMacro();

  QTimer::singleShot(0, this, SLOT(connectRefCtrl()));
//       connect(this,&QObject::destroyed,reference_controller::get_instance(),
//               &reference_controller::modelDestroyed);
}


  QModelIndexList QStdItemModel::find(const QModelIndex& start_node, int role, const QVariant& key) const
{
    QModelIndexList hits;
    auto search_key = [this,&hits,&role,&key](const auto* item)
    {
        if(item)
        {
            if(item->data(role)==key)
            {
                hits<<item->index();
            }
        }
    };

    iterate(search_key,start_node);

    return hits;

}

  // change the return type to std::optional<QModelIndex> !!
 std::optional< QModelIndex> QStdItemModel::find_uid(unsigned long long uid,const QModelIndex& start_node) const
{
    QModelIndexList hits;
    auto search_key = [&hits,&uid](const auto* item)
    {
        if(item)
        {
            if(item->uuid()==uid)
            {
                hits<<item->index();
                return false; // end the search
            }

        }
        return true; // continue the search
    };

    //iterate_interuptable(search_key,start_node);

    iterate(search_key,start_node);
  //  if(hits.isEmpty())return QModelIndex();



 if(!(hits.isEmpty())         )
 {
     return hits.first();
 }
 else
 {  if(invisibleRootItem()->uuid()==uid)
     {return QModelIndex();}
     else{
     return std::nullopt;
     }
 }



}


void QStdItemModel::connectRefCtrl()
{
 /*   connect(this,&QObject::destroyed,reference_controller::get_instance(),
                   &reference_controller::modelDestroyed);
                   */
}

/*!
    Constructs a new item model that initially has \a rows rows and \a columns
    columns, and that has the given \a parent.
*/
QStdItemModel::QStdItemModel(int rows, int columns, QObject *parent)
    : QAbstractItemModel(*new QStdItemModelPrivate, parent)
{
    Q_D(QStdItemModel);
     m_stack= new UndoStack(this);
    m_stack->beginMacro("QStdItemModel::QStdItemModel");
    d->init();
    d->root->insertColumns(0, columns);
    d->columnHeaderItems.insert(0, columns, nullptr);
    d->root->insertRows(0, rows);
    d->rowHeaderItems.insert(0, rows, nullptr);
  //  d->root->d_func()->setModel(this);
      d->root->setModel(this);
    m_stack->endMacro();

      QTimer::singleShot(0, this, SLOT(connectRefCtrl()));

 //   connect(this,&QObject::destroyed,reference_controller::get_instance(),
 //           &reference_controller::modelDestroyed);
}


QStdItemModel::QStdItemModel(QStdItemModelPrivate &dd, QObject *parent)
    : QAbstractItemModel(dd, parent)
{
    Q_D(QStdItemModel);
     m_stack= new UndoStack(this);
    d->init();


      QTimer::singleShot(0, this, SLOT(connectRefCtrl()));
  //  connect(this,&QObject::destroyed,reference_controller::get_instance(),
  //          &reference_controller::modelDestroyed);
}

/*!
    Destructs the model. The model destroys all its items.
*/
QStdItemModel::~QStdItemModel()
{
    // hässlicher workaround, der nur ausnahmsweise funktioniert,
    // da es lediglich ein einziges modell in der anwendung gibt
  //  reference_controller::get_instance()->clear();

    Q_D(QStdItemModel);
    delete d->itemPrototype;
    qDeleteAll(d->columnHeaderItems);
    qDeleteAll(d->rowHeaderItems);
    d->root.reset();
}


Path QStdItemModel::parentPath(const Path& p )
{
    if(p.length() >=2)
    {return Path(p.cbegin(),p.cend()-1);    }
    else
    {
        return Path()<<PathItem(-1,-1);
    }
}


QModelIndex QStdItemModel::cut(const QModelIndex &index)
{
    if(!canAcceptCut(index))return index;

   on_scope_exit t{ [this](){ qDebug().noquote() <<"< QStdItemModel::cut Macro >";
                            undo_stack()->beginMacro("QStdItemModel::cut");},
                    [this](){   qDebug().noquote()<< "</ QStdItemModel::cut Macro>";
                                undo_stack()->endMacro();}
                  };

    auto cut_cmd = new CutItemCmd(this,index) ;
    undo_stack()->push(cut_cmd);

 //   auto idx{undo_stack()->index() };
   // const QStdItemModel::QStdItemModelCmd* cmd {static_cast<const QStdItemModel::QStdItemModelCmd*>(undo_stack()->command(idx) ) };

    if(cut_cmd)
    {
        return cut_cmd->returnValue().value<QModelIndex>();
    }


  return QModelIndex();

}

bool QStdItemModel::hasCutItem() const
{
    return d_func()->hasCutItem();
}

QModelIndex QStdItemModel::paste(const QModelIndex &index,Behaviour b)
{
    if(!canAcceptPaste(index))return QModelIndex();

    on_scope_exit t{ [this](){ qDebug().noquote() <<"< QStdItemModel::paste Macro >";
                             undo_stack()->beginMacro("QStdItemModel::paste");},
                     [this](){   qDebug().noquote()<< "</ QStdItemModel::paste Macro>";
                                 undo_stack()->endMacro();}
                   };

     auto paste_cmd = new PasteItemCmd(this,index,b) ;
     undo_stack()->push(paste_cmd);

     if(paste_cmd)
     {
         return paste_cmd->returnValue().value<QModelIndex>();
     }


   return QModelIndex();
}

/*!
    Sets the item role names to \a roleNames.
*/
void QStdItemModel::setItemRoleNames(const QHash<int,QByteArray> &roleNames)
{
 scope_tagger t{"QStdItemModel::setItemRoleNames(const QHash<int,QByteArray>& roleNames)"};



    Q_D(QStdItemModel);
    d->roleNames = roleNames;

}

bool QStdItemModel::canDropMimeData(const QMimeData *data,
    Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    scope_tagger t {"QStdItemModel::canDropMimeData"};

/* This is the body of QAbstractItemModel::canDropMimeData
     Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);

    if (!(action & supportedDropActions()))
        return false;

    const QStringList modelTypes = mimeTypes();
    for (int i = 0; i < modelTypes.count(); ++i) {
        if (data->hasFormat(modelTypes.at(i)))
            return true;
    }
    return false;
*/


    auto tmp{ QAbstractItemModel::canDropMimeData(data,action,row,column,parent)};



    return tmp;
}


QHash<int, QByteArray> QStdItemModel::roleNames() const
{
    Q_D(const QStdItemModel);
    return d->roleNames;
}

/*!
    Removes all items (including header items) from the model and sets the
    number of rows and columns to zero.

    \sa removeColumns(), removeRows()
*/
void QStdItemModel::clear()
{

 scope_tagger t{"QStdItemModel::clear"};


    m_stack->clear(); // forget the command history
auto refctrl{reference_controller::get_instance()};
auto cnd_before{refctrl->cmd_count()};
refctrl->clear();
qDebug()<<refctrl->cmd_count();
    Q_D(QStdItemModel);

    auto text{QString("clear")};

    qDebug().noquote()<< "<QStdItemModel::undo_stack::beginMacro text=\"" +text+ "\">";
    undo_stack()->beginMacro("QStdItemModel:" + text );

    beginResetModel();
    d->root.reset(new QStdItem);


   // d->root->setFlags(Qt::ItemIsDropEnabled);
  //  d->root->d_func()->setModel(this);
      d->root->setModel(this);
         d->root->d_func()->setFlags(Qt::ItemIsDropEnabled);

    qDeleteAll(d->columnHeaderItems);
    d->columnHeaderItems.clear();

    qDeleteAll(d->rowHeaderItems);
    d->rowHeaderItems.clear();

    endResetModel();

    undo_stack()->endMacro();
    qDebug().noquote()<< "</QStdItemModel::undo_stack::beginMacro text=\"" +text+ "\">";


}

/*!
    \since 4.2

    Returns a pointer to the QStdItem associated with the given \a index.

    Calling this function is typically the initial step when processing
    QModelIndex-based signals from a view, such as
    QAbstractItemView::activated(). In your slot, you call itemFromIndex(),
    with the QModelIndex carried by the signal as argument, to obtain a
    pointer to the corresponding QStdItem.

    Note that this function will lazily create an item for the index (using
    itemPrototype()), and set it in the parent item's child table, if no item
    already exists at that index.

    If \a index is an invalid index, this function returns \nullptr.

    \sa indexFromItem()
*/
QStdItem *QStdItemModel::itemFromIndex(const QModelIndex &index) const
{
    Q_D(const QStdItemModel);

    if ((index.row() < 0) || (index.column() < 0) || (index.model() != this))
        return nullptr;

    QStdItem *parent = static_cast<QStdItem*>(index.internalPointer());

    if (parent == nullptr)
        return nullptr;

    QStdItem *item = parent->child(index.row(), index.column());

    // lazy part
    if (item == nullptr)
    {
        undo_stack()->beginMacro("QStdItemModel::itemFromIndex");

        item = d->createItem();

      //  parent->d_func()->setChild(index.row(), index.column(), item); // bypassing undo
          parent->setChild(index.row(), index.column(), item);

        undo_stack()->endMacro();


    }
    return item;
}

/*!
    \since 4.2

    Returns the QModelIndex associated with the given \a item.

    Use this function when you want to perform an operation that requires the
    QModelIndex of the item, such as
    QAbstractItemView::scrollTo(). QStdItem::index() is provided as
    convenience; it is equivalent to calling this function.

    \sa itemFromIndex(), QStdItem::index()
*/
QModelIndex QStdItemModel::indexFromItem(const QStdItem *item) const
{
    if (item && item->d_func()->parent)
    {
        QPair<int, int> pos = item->d_func()->position();
        return createIndex(pos.first, pos.second, item->d_func()->parent);
    }
    return QModelIndex();
}

/*!
    \since 4.2

    Sets the number of rows in this model to \a rows. If
    this is less than rowCount(), the data in the unwanted rows
    is discarded.

    \sa setColumnCount()
*/
void QStdItemModel::setRowCount(int rows)
{
 scope_tagger t{"QStdItemModel::setRowCount"};


    Q_D(QStdItemModel);
    auto text{QString("QStdItemModel::setRowCount: %1").arg(rows)};

    qDebug().noquote()<< "<QStdItemModel::undo_stack::beginMacro text=\"" +text+ "\">";
    undo_stack()->beginMacro("QStdItemModel:" + text );

            // delegate to QStdItem
            d->root->setRowCount(rows);

    undo_stack()->endMacro();
    qDebug().noquote()<< "</QStdItemModel::undo_stack::beginMacro text=\"" +text+ "\">";


}

/*!
    \since 4.2

    Sets the number of columns in this model to \a columns. If
    this is less than columnCount(), the data in the unwanted columns
    is discarded.

    \sa setRowCount()
*/
void QStdItemModel::setColumnCount(int columns)
{
scope_tagger t {"QStdItemModel::setColumnCount"};


    Q_D(QStdItemModel);
    auto text{QString("setColumnCount: %1").arg(columns)};

      qDebug().noquote() << "<QStdItemModel::undo_stack::beginMacro text=\"" +text+ "\">";
    undo_stack()->beginMacro("QStdItemModel::"+text);

            d->root->setColumnCount(columns);

    undo_stack()->endMacro();
    qDebug().noquote() << "</QStdItemModel::undo_stack::beginMacro text=\"" +text+ "\">";


}

/*!
    \since 4.2

    Sets the item for the given \a row and \a column to \a item. The model
    takes ownership of the item. If necessary, the row count and column count
    are increased to fit the item. The previous item at the given location (if
    there was one) is deleted.

    \sa item()
*/
void QStdItemModel::setItem(int row, int column, QStdItem *item)
{
scope_tagger t {"QStdItemModel::setItem(int row,int column, QStdItem* item)"};


    Q_D(QStdItemModel);
 //   d->root->d_func()->setChild(row, column, item, true);

   undo_stack()->beginMacro("QStdItemModel::setItem");

        d->root->setChild(row,column,item);

    undo_stack()->endMacro();

}

/*!
  \fn QStdItemModel::setItem(int row, QStdItem *item)
  \overload
*/

/*!
    \since 4.2

    Returns the item for the given \a row and \a column if one has been set;
    otherwise returns \nullptr.

    \sa setItem(), takeItem(), itemFromIndex()
*/
QStdItem *QStdItemModel::item(int row, int column) const
{
    Q_D(const QStdItemModel);
    return d->root->child(row, column);
}

/*!
    \since 4.2

    Returns the model's invisible root item.

    The invisible root item provides access to the model's top-level items
    through the QStdItem API, making it possible to write functions that
    can treat top-level items and their children in a uniform way; for
    example, recursive functions involving a tree model.

    \note Calling \l{QAbstractItemModel::index()}{index()} on the QStdItem object
    retrieved from this function is not valid.
*/
QStdItem *QStdItemModel::invisibleRootItem() const
{
    Q_D(const QStdItemModel);
    return d->root.data();
}

/*!
    \since 4.2

    Sets the horizontal header item for \a column to \a item.  The model takes
    ownership of the item. If necessary, the column count is increased to fit
    the item. The previous header item (if there was one) is deleted.

    \sa horizontalHeaderItem(), setHorizontalHeaderLabels(),
    setVerticalHeaderItem()
*/



void QStdItemModel::setHorizontalHeaderItem(int column, QStdItem *item)
{
scope_tagger t{ "QStdItemModel::setHorizontalHeaderItem(int column,QStdItem* item)"};


  undo_stack()->beginMacro("QStdItemModel::setHorizontalHeaderItem");

        undo_stack()->push(      new SetHHeaderItemCmd(this,column,item)        );

  undo_stack()->endMacro();


}

/*!
    \since 4.2

    Returns the horizontal header item for \a column if one has been set;
    otherwise returns \nullptr.

    \sa setHorizontalHeaderItem(), verticalHeaderItem()
*/
QStdItem *QStdItemModel::horizontalHeaderItem(int column) const
{
    Q_D(const QStdItemModel);
    if ((column < 0) || (column >= columnCount()))
        return nullptr;
    return d->columnHeaderItems.at(column);
}

/*!
    \since 4.2

    Sets the vertical header item for \a row to \a item.  The model takes
    ownership of the item. If necessary, the row count is increased to fit the
    item. The previous header item (if there was one) is deleted.

    \sa verticalHeaderItem(), setVerticalHeaderLabels(),
    setHorizontalHeaderItem()
*/
void QStdItemModel::setVerticalHeaderItem(int row, QStdItem *item)
{
 scope_tagger t{ "QStdItemModel::setHorizontalHeaderItem(int row,QStdItem* item)"};




    Q_D(QStdItemModel);
    if (row < 0)
    {     return;
    }

    if (rowCount() <= row)
        setRowCount(row + 1);

    QStdItem *oldItem = d->rowHeaderItems.at(row);
    if (item == oldItem)
    {        return;
    }

    if (item)
    {
        if (item->model() == nullptr)
        {
            item->d_func()->setModel(this);
        } else
        {
            qWarning("QStdItem::setVerticalHeaderItem: Ignoring duplicate insertion of item %p",
                     item);
              return;
        }
    }

    if (oldItem)
        oldItem->d_func()->setModel(nullptr);
    delete oldItem;

    d->rowHeaderItems.replace(row, item);
    emit headerDataChanged(Qt::Vertical, row, row);


}

/*!
    \since 4.2

    Returns the vertical header item for row \a row if one has been set;
    otherwise returns \nullptr.

    \sa setVerticalHeaderItem(), horizontalHeaderItem()
*/
QStdItem *QStdItemModel::verticalHeaderItem(int row) const
{
    Q_D(const QStdItemModel);
    if ((row < 0) || (row >= rowCount()))
        return nullptr;
    return d->rowHeaderItems.at(row);
}

/*!
    \since 4.2

    Sets the horizontal header labels using \a labels. If necessary, the
    column count is increased to the size of \a labels.

    \sa setHorizontalHeaderItem()
*/
void QStdItemModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    scope_tagger t{"QStdItemModel::setHorizontalHeaderLabels(const QStringList& labels)"};

    Q_D(QStdItemModel);
    if (columnCount() < labels.count())
        setColumnCount(labels.count());

    for (int i = 0; i < labels.count(); ++i)
    {
        QStdItem *item = horizontalHeaderItem(i);
        if (!item)
        {
            item = d->createItem();
            setHorizontalHeaderItem(i, item);
        }
        item->setText(labels.at(i));
    }

}

/*!
    \since 4.2

    Sets the vertical header labels using \a labels. If necessary, the row
    count is increased to the size of \a labels.

    \sa setVerticalHeaderItem()
*/
void QStdItemModel::setVerticalHeaderLabels(const QStringList &labels)
{
  scope_tagger t{ "QStdItemModel::setVerticalHeaderLabels(const QStringList& labels)"};

    Q_D(QStdItemModel);
    if (rowCount() < labels.count())
        setRowCount(labels.count());

    for (int i = 0; i < labels.count(); ++i)
    {
        QStdItem *item = verticalHeaderItem(i);
        if (!item)
        {
            item = d->createItem();
            setVerticalHeaderItem(i, item);
        }
        item->setText(labels.at(i));
    }

}

/*!
    \since 4.2

    Sets the item prototype for the model to the specified \a item. The model
    takes ownership of the prototype.

    The item prototype acts as a QStdItem factory, by relying on the
    QStdItem::clone() function.  To provide your own prototype, subclass
    QStdItem, reimplement QStdItem::clone() and set the prototype to
    be an instance of your custom class. Whenever QStdItemModel needs to
    create an item on demand (for instance, when a view or item delegate calls
    setData())), the new items will be instances of your custom class.

    \sa itemPrototype(), QStdItem::clone()
*/
void QStdItemModel::setItemPrototype(const QStdItem *item)
{
    scope_tagger t{"QStdItemModel::setItemPrototype(const QStdItem* item)"};

    Q_D(QStdItemModel);
    if (d->itemPrototype != item)
    {
        delete d->itemPrototype;
        d->itemPrototype = item;
    }


}

/*!
    \since 4.2

    Returns the item prototype used by the model. The model uses the item
    prototype as an item factory when it needs to construct new items on
    demand (for instance, when a view or item delegate calls setData()).

    \sa setItemPrototype()
*/
const QStdItem *QStdItemModel::itemPrototype() const
{
    Q_D(const QStdItemModel);
    return d->itemPrototype;
}

/*!
    \since 4.2

    Returns a list of items that match the given \a text, using the given \a
    flags, in the given \a column.
*/
QList<QStdItem*> QStdItemModel::findItems(const QString &text,
                                                    Qt::MatchFlags flags, int column) const
{
    QModelIndexList indexes = match(index(0, column, QModelIndex()),
                                    Qt::DisplayRole, text, -1, flags);
    QList<QStdItem*> items;
    const int numIndexes = indexes.size();
    items.reserve(numIndexes);
    for (int i = 0; i < numIndexes; ++i)
        items.append(itemFromIndex(indexes.at(i)));
    return items;
}

/*!
    \since 4.2

    Appends a row containing \a items. If necessary, the column count is
    increased to the size of \a items.

    \sa insertRow(), appendColumn()
*/
void QStdItemModel::appendRow(const QList<QStdItem*> &items)

{
scope_tagger t{ "QStdItemModel::appendRow(const QList<QStdItem*>& items)"};


    qDebug().noquote()<< "<QStdItemModel::undo_stack::beginMacro text=\"appendRow\">";
    undo_stack()->beginMacro("QStdItemModel::appendRow");

          invisibleRootItem()->appendRow(items);

    undo_stack()->endMacro();
    qDebug().noquote()<< "</QStdItemModel::undo_stack::beginMacro text=\"appendRow\">";


}

/*!
    \since 4.2

    Appends a column containing \a items. If necessary, the row count is
    increased to the size of \a items.

    \sa insertColumn(), appendRow()
*/
void QStdItemModel::appendColumn(const QList<QStdItem*> &items)
{
     scope_tagger t{ "QStdItemModel::appendColumn(const QList<QStdItem*>& items)"};



       qDebug().noquote()<< "<QStdItemModel::undo_stack::beginMacro text=\"appendColumn\">";
    undo_stack()->beginMacro("QStdItemModel::appendColumn");

           invisibleRootItem()->appendColumn(items);

    undo_stack()->endMacro();
    qDebug().noquote()<< "</QStdItemModel::undo_stack::beginMacro text=\"appendColumn\">";


}

/*!
    \since 4.2
    \fn QStdItemModel::appendRow(QStdItem *item)
    \overload

    When building a list or a tree that has only one column, this function
    provides a convenient way to append a single new \a item.
*/

/*!
    \since 4.2

    Inserts a row at \a row containing \a items. If necessary, the column
    count is increased to the size of \a items.

    \sa takeRow(), appendRow(), insertColumn()
*/
void QStdItemModel::insertRow(int row, const QList<QStdItem*> &items)
{
   scope_tagger t{ "QStdItemModel::insertRow(int row, const QList<QStdItem*>& items)"};


     auto text{QString("insertRow: %1").arg(row)};


            qDebug().noquote() << "<QStdItemModel::undo_stack::beginMacro text=\"" + text+ "\">";
    undo_stack()->beginMacro("QStdItemModel::" + text);

            invisibleRootItem()->insertRow(row, items);

    undo_stack()->endMacro();
           qDebug()<< "<QStdItemModel::undo_stack::beginMacro text=\"" + text+ "\">";

    qDebug().noquote() << "</QStdItemModel::insertRow(int row, const QList<QStdItem*>& items)>";
}

/*!
    \since 4.2

    Inserts a column at \a column containing \a items. If necessary, the row
    count is increased to the size of \a items.

    \sa takeColumn(), appendColumn(), insertRow()
*/
void QStdItemModel::insertColumn(int column, const QList<QStdItem*> &items)
{
  scope_tagger t{"QStdItemModel::insertColumn(int column,const QList<QStdItem*>& items)"};

     auto text{QString("insertColumn: %1").arg(column)};

       qDebug().noquote()<< "<QStdItemModel::undo_stack::beginMacro text=\"" + text+ "\">";

    undo_stack()->beginMacro("QStdItemModel::" + text );

            invisibleRootItem()->insertColumn(column, items);

    undo_stack()->endMacro();

      qDebug().noquote()<< "</QStdItemModel::undo_stack::beginMacro text=\"" + text+ "\">";


}

/*!
    \since 4.2

    Removes the item at (\a row, \a column) without deleting it. The model
    releases ownership of the item.

    \sa item(), takeRow(), takeColumn()
*/
QStdItem *QStdItemModel::takeItem(int row, int column)
{
  scope_tagger t{ "QStdItemModel::takeItem(int row,int column)"};

    Q_D(QStdItemModel);
    auto tmp{ d->root->takeChild(row, column)};


    return tmp;
}

/*!
    \since 4.2

    Removes the given \a row without deleting the row items, and returns a
    list of pointers to the removed items. The model releases ownership of the
    items. For items in the row that have not been set, the corresponding
    pointers in the list will be \nullptr.

    \sa takeColumn()
*/
QList<QStdItem*> QStdItemModel::takeRow(int row)
{
   scope_tagger t{"QStdItemModel::takeRow(int row)"};
    Q_D(QStdItemModel);
    auto tmp{ d->root->takeRow(row)};

    return tmp;
}

/*!
    \since 4.2

    Removes the given \a column without deleting the column items, and returns
    a list of pointers to the removed items. The model releases ownership of
    the items. For items in the column that have not been set, the
    corresponding pointers in the list will be \nullptr.

    \sa takeRow()
*/
QList<QStdItem*> QStdItemModel::takeColumn(int column)
{
    scope_tagger t{ "QStdItemModel::takeColumn"};
    Q_D(QStdItemModel);
    auto tmp{ d->root->takeColumn(column)};

    return tmp;
}

/*!
    \since 4.2

    Removes the horizontal header item at \a column from the header without
    deleting it, and returns a pointer to the item. The model releases
    ownership of the item.

    \sa horizontalHeaderItem(), takeVerticalHeaderItem()
*/
QStdItem *QStdItemModel::takeHorizontalHeaderItem(int column)
{
    scope_tagger t{ "QStdItemModel::takeHorizontalHeaderItem(int column)"};
    Q_D(QStdItemModel);
    if ((column < 0) || (column >= columnCount()))
    {

        return nullptr;
    }

    QStdItem *headerItem = d->columnHeaderItems.at(column);
    if (headerItem)
    {
        headerItem->d_func()->setParentAndModel(nullptr, nullptr);
        d->columnHeaderItems.replace(column, nullptr);
    }


    return headerItem;
}

/*!
    \since 4.2

    Removes the vertical header item at \a row from the header without
    deleting it, and returns a pointer to the item. The model releases
    ownership of the item.

    \sa verticalHeaderItem(), takeHorizontalHeaderItem()
*/
QStdItem *QStdItemModel::takeVerticalHeaderItem(int row)
{
 scope_tagger t{ "QStdItemModel::takeVerticalHeaderItem"};




    Q_D(QStdItemModel);
    if ((row < 0) || (row >= rowCount()))
    {
        return nullptr;
    }
    QStdItem *headerItem = d->rowHeaderItems.at(row);
    if (headerItem)
    {
        headerItem->d_func()->setParentAndModel(nullptr, nullptr);
        d->rowHeaderItems.replace(row, nullptr);
    }


    return headerItem;
}

/*!
    \since 4.2
    \property QStdItemModel::sortRole
    \brief the item role that is used to query the model's data when sorting items

    The default value is Qt::DisplayRole.

    \sa sort(), QStdItem::sortChildren()
*/
int QStdItemModel::sortRole() const
{
    Q_D(const QStdItemModel);
    return d->sortRole;
}

void QStdItemModel::setSortRole(int role)
{
   scope_tagger t{ "QStdItemModel::setSortRole"};



    Q_D(QStdItemModel);
    d->sortRole = role;

}

QBindable<int> QStdItemModel::bindableSortRole()
{
    Q_D(QStdItemModel);
    return &d->sortRole;
}


int QStdItemModel::columnCount(const QModelIndex &parent) const
{
    Q_D(const QStdItemModel);
    QStdItem *item = d->itemFromIndex(parent);
    return item ? item->columnCount() : 0;
}


QVariant QStdItemModel::data(const QModelIndex &index, int role) const
{
    Q_D(const QStdItemModel);
    QStdItem *item = d->itemFromIndex(index);
    return item ? item->data(role) : QVariant();
}

void QStdItemModel::multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const
{
    // Cannot offer a better implementation; users may be overriding
    // data(), and thus multiData() may fall out of sync for them.
    // The base class' implementation will simply call data() in a loop,
    // so it's fine.
    QAbstractItemModel::multiData(index, roleDataSpan);
}


Qt::ItemFlags QStdItemModel::flags(const QModelIndex &index) const
{
    Q_D(const QStdItemModel);
    if (!d->indexValid(index))
        return d->root->flags();
    QStdItem *item = d->itemFromIndex(index);
    if (item)
        return item->flags();
    return Qt::ItemIsSelectable
        |Qt::ItemIsEnabled
        |Qt::ItemIsEditable
        |Qt::ItemIsDragEnabled
        |Qt::ItemIsDropEnabled;
}


bool QStdItemModel::hasChildren(const QModelIndex &parent) const
{
    Q_D(const QStdItemModel);
    QStdItem *item = d->itemFromIndex(parent);
    return item ? item->hasChildren() : false;
}


QVariant QStdItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_D(const QStdItemModel);
    if ((section < 0)
        || ((orientation == Qt::Horizontal) && (section >= columnCount()))
        || ((orientation == Qt::Vertical) && (section >= rowCount()))) {
        return QVariant();
    }
    QStdItem *headerItem = nullptr;
    if (orientation == Qt::Horizontal)
     { if(!d->columnHeaderItems.isEmpty())
        {headerItem = d->columnHeaderItems.at(section);
        }
    }
    else if (orientation == Qt::Vertical)
    {   if(!d->rowHeaderItems.isEmpty())
        {headerItem = d->rowHeaderItems.at(section);
        }
    }

    return headerItem ? headerItem->data(role)
        : QAbstractItemModel::headerData(section, orientation, role);
}


Qt::DropActions QStdItemModel::supportedDropActions () const
{
    return Qt::CopyAction | Qt::MoveAction;
}

bool QStdItemModel::canAcceptCut(const QModelIndex&)const
{
    return true;
}

bool QStdItemModel::canAcceptPaste(const QModelIndex&)const
{
    return true;
}

QModelIndex QStdItemModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const QStdItemModel);
    QStdItem *parentItem = d->itemFromIndex(parent);
    if ((parentItem == nullptr)
        || (row < 0)
        || (column < 0)
        || (row >= parentItem->rowCount())
        || (column >= parentItem->columnCount())) {
        return QModelIndex();
    }
    return createIndex(row, column, parentItem);
}


bool QStdItemModel::insertColumns(int column, int count, const QModelIndex &parent)
{

    scope_tagger t{"QStdItemModel::insertColumns(int column, int count,const QModelIndex& parent)"};



    Q_D(QStdItemModel);
    QStdItem *item = parent.isValid() ? itemFromIndex(parent) : d->root.data();

    if (item == nullptr)
    {
        return false;
    }

//    return item->d_func()->insertColumns(column, count, QList<QStdItem*>());

         auto text{QString("insertColumns: column %1, count: %2").arg(column).arg(count)};

         qDebug().noquote()<< "<QStdItemModel::undo_stack::beginMacro text=\"" + text+ "\">";
         undo_stack()->beginMacro("QStdItemModel::" +text );

                item->insertColumns(column, count);

         auto index{undo_stack()->index() };
        const QStdItem::StdItemCmd* cmd {static_cast<const QStdItem::StdItemCmd*>( lastChildofCmd( undo_stack()->command(index) ) ) };

        undo_stack()->endMacro();
          qDebug().noquote()<< "</QStdItemModel::undo_stack::beginMacro text=\"" + text+ "\">";

        bool ret_val{false};

         if(cmd)
         {
             ret_val= cmd->redoSuccessFlag();
            // qDebug()<< ret_val;
             return ret_val;
         }







         return true;

}


bool QStdItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
  scope_tagger t{ "QStdItemModel::insertRows(int row,int count,const QModelIndex& parent)"};



    Q_D(QStdItemModel);
    QStdItem *item = parent.isValid() ? itemFromIndex(parent) : d->root.data();

    if (item == nullptr)
    {
        return false;
    }

  //  return item->d_func()->insertRows(row, count, QList<QStdItem*>());

    auto text{QString("insertRows row: %1, count: %2").arg(row).arg(count)};


      qDebug().noquote()<< "<QStdItemModel::undo_stack::beginMacro text=\"" + text+ "\">";
    undo_stack()->beginMacro("QStdItemModel::" + text );

            item->insertRows(row,count);

    auto index{undo_stack()->index() };
   const QStdItem::StdItemCmd* cmd {static_cast<const QStdItem::StdItemCmd*>( lastChildofCmd(undo_stack()->command(index) ) )};


   undo_stack()->endMacro();
     qDebug().noquote() << "</QStdItemModel::undo_stack::beginMacro text=\"" + text+ "\">";


    if(cmd)
    {
        return cmd->redoSuccessFlag();
     }

    return true;
}


QMap<int, QVariant> QStdItemModel::itemData(const QModelIndex &index) const
{
    Q_D(const QStdItemModel);
    const QStdItem *const item = d->itemFromIndex(index);
    if (!item || item == d->root.data())
        return QMap<int, QVariant>();
    return item->d_func()->itemData();
}


QModelIndex QStdItemModel::parent(const QModelIndex &child) const
{
    Q_D(const QStdItemModel);
    if (!d->indexValid(child))
        return QModelIndex();

    QStdItem *parentItem = static_cast<QStdItem*>(child.internalPointer());

    return indexFromItem(parentItem);
}


bool QStdItemModel::removeColumns(int column, int count, const QModelIndex &parent)
{
     scope_tagger t{ "QStdItemModel::removeColumns(int column, int count, const QModelIndex& parent)"};



    Q_D(QStdItemModel);


    QStdItem *item = d->itemFromIndex(parent);

    if ((item == nullptr) || (count < 1) || (column < 0) || ((column + count) > item->columnCount()))
    {
        return false;
    }
    auto text{QString("removeColumns: %1 , %2").arg(column).arg(count)};

     qDebug().noquote()   << "<QStdItemModel::undo_stack::beginMacro text=\"" + text+ "\">";
    undo_stack()->beginMacro("QStdItemModel::" + text);

                item->removeColumns(column, count);

    undo_stack()->endMacro();
     qDebug().noquote() << "</QStdItemModel::undo_stack::beginMacro text=\"" + text+ "\">";


    return true;
}


bool QStdItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
  scope_tagger t{ "QStdItemModel::removeRows(int row,int count, const QModelIndex& parent)"};


    Q_D(QStdItemModel);
    QStdItem *item = d->itemFromIndex(parent);

    if ((item == nullptr) || (count < 1) || (row < 0) || ((row + count) > item->rowCount()))
    {
        return false;
    }
    auto text{QString("removeRows: %1 , %2").arg(row).arg(count)};

    qDebug().noquote()<< "<QStdItemModel::undo_stack::beginMacro text=\"" + text+ "\">";
    undo_stack()->beginMacro("QStdItemModel::" + text);

                item->removeRows(row, count);

    undo_stack()->endMacro();
    qDebug().noquote()<< "</QStdItemModel::undo_stack::beginMacro text=\"" + text+ "\">";


    return true;
}


int QStdItemModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const QStdItemModel);
    QStdItem *item = d->itemFromIndex(parent);
    return item ? item->rowCount() : 0;
}


bool QStdItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    scope_tagger t{ "QStdItemModel::setData"};


    if (!index.isValid())
    {
        return false;
    }

    QStdItem *item = itemFromIndex(index);

    if (item == nullptr)
     {
        return false;
    }

    qDebug().noquote()<< "<QStdItemModel::undo_stack::beginMacro text=\"setData\">";
    undo_stack()->beginMacro("QStdItemModel::setData");

            item->setData(value, role);

    undo_stack()->endMacro();
    qDebug().noquote()<< "</QStdItemModel::undo_stack::beginMacro text=\"setData\">";


    return true;
}


bool QStdItemModel::clearItemData(const QModelIndex &index)
{
    scope_tagger t{ "QStdItemModel::clearItemData"};


    if (!checkIndex(index, CheckIndexOption::IndexIsValid))
    {
        return false;
    }

    Q_D(QStdItemModel);

    QStdItem *item = d->itemFromIndex(index);

    if (!item)
    {
        return false;
    }
    qDebug().noquote()<< "<QStdItemModel::undo_stack::beginMacro text=\"clearItemData\">";
    undo_stack()->beginMacro("QStdItemModel::clearItemData");

            item->clearData();

    undo_stack()->endMacro();
    qDebug().noquote() << "</QStdItemModel::undo_stack::beginMacro text=\"clearItemData\">";


    return true;
}


bool QStdItemModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  scope_tagger t{ "QStdItemModel::setHeaderData(nit section,Qt::Orientation,const QVariant& value,int role)"};

    Q_D(QStdItemModel);
    if ((section < 0)
        || ((orientation == Qt::Horizontal) && (section >= columnCount()))
        || ((orientation == Qt::Vertical) && (section >= rowCount())))
    {
        return false;
    }

    QStdItem *headerItem = nullptr;
    if (orientation == Qt::Horizontal)
    {
        headerItem = d->columnHeaderItems.at(section);
        if (headerItem == nullptr)
        {
            headerItem = d->createItem();
            headerItem->d_func()->setModel(this);
            d->columnHeaderItems.replace(section, headerItem);
        }
    } else if (orientation == Qt::Vertical)
    {
        headerItem = d->rowHeaderItems.at(section);
        if (headerItem == nullptr)
        {
            headerItem = d->createItem();
            headerItem->d_func()->setModel(this);
            d->rowHeaderItems.replace(section, headerItem);
        }
    }
    if (headerItem)
    {
        headerItem->setData(value, role);

        return true;
    }

    return false;
}


bool QStdItemModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{

scope_tagger t{ "QStdItemModel::setItemData(const QModelIndex& index, const QMap<int,QVariant>& roles)"};



    QStdItem *item = itemFromIndex(index);
    if (item == nullptr)
    {
        return false;
    }

    undo_stack()->beginMacro("QStdItemModel::setItemData");

    // item->d_func()->setItemData(roles); // original: nicht undo-able
       item->setItemData(roles); // uses SetDataCmd internally

       undo_stack()->endMacro();


    return true;
}

void QStdItemModel::sort(int column, Qt::SortOrder order)
{
    scope_tagger t{ "QStdItemModel::sort(int column, Qt::SortOrder order)"};

    Q_D(QStdItemModel);
    d->root->sortChildren(column, order);


}

QStringList QStdItemModel::mimeTypes() const
{
    return QAbstractItemModel::mimeTypes() << qStandardItemModelDataListMimeType();
}

QMimeData *QStdItemModel::mimeData(const QModelIndexList &indexes) const
{ scope_tagger t{ "QStdItemModel::mimeData"};


    QMimeData *data = QAbstractItemModel::mimeData(indexes);
    if (!data)
    {     
        return nullptr;
    }

    const QString format = qStandardItemModelDataListMimeType();

    if (!mimeTypes().contains(format))
     {     
        return data;
    }

    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);

    QSet<QStdItem*> itemsSet;
    QStack<QStdItem*> stack;
    itemsSet.reserve(indexes.count());
    stack.reserve(indexes.count());

    for (int i = 0; i < indexes.count(); ++i)
    {
        if (QStdItem *item = itemFromIndex(indexes.at(i)))
        {
            itemsSet << item;
            stack.push(item);
        } else
        {
            qWarning("QStdItemModel::mimeData: No item associated with invalid index");

            return nullptr;
        }
    }

    //remove duplicates children
    {
        QDuplicateTracker<QStdItem *> seen;

        while (!stack.isEmpty())
        {
            QStdItem *itm = stack.pop();
            if (seen.hasSeen(itm))
                continue;

            const QList<QStdItem*> &childList = itm->d_func()->children;

            for (int i = 0; i < childList.count(); ++i)
            {
                QStdItem *chi = childList.at(i);
                if (chi)
                {
                    itemsSet.remove(chi);
                    stack.push(chi);
                }
            }
        }

        // after the while-loop the stack is empty !
    }

    stack.reserve(itemsSet.count());

    for (QStdItem *item : qAsConst(itemsSet))
        stack.push(item);

    //stream everything recursively
    while (!stack.isEmpty())
    {
        QStdItem *item = stack.pop();

        if (itemsSet.contains(item)) //if the item is selection 'top-level', stream its position
            stream << item->row() << item->column();

        stream << *item << item->columnCount() << int(item->d_ptr->children.count());
        stack += item->d_ptr->children;
    }

    // here the stack is empty again !

    data->setData(format, encoded);

    return data;
}




bool QStdItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                      int row, int column, const QModelIndex &parent)
{
     //qDebug()<< "<QStdItemModel::dropMimeData.´(const QMimeData* data,Qt::DropAction ,int row,int column, const QModelIndex& parent)";
  auto print_start =[this](){undo_stack()->beginMacro("QStdItemModel::dropMimeData");
                                qDebug()<< "<QStdItemModel::dropMimeData>";};
  auto print_end =[this](){  undo_stack()->endMacro();
                            qDebug()<< "</QStdItemModel::dropMimeData>";};

  print_start();


    Q_D(QStdItemModel);
    // check if the action is supported
    if (!data || !(action == Qt::CopyAction || action == Qt::MoveAction))
    {   print_end();
        return false;
    }
    // check if the format is supported
    const QString format = qStandardItemModelDataListMimeType();

    if (!data->hasFormat(format))
    {   print_end();
        return QAbstractItemModel::dropMimeData(data, action, row, column, parent);
    }

    if (row > rowCount(parent))
        row = rowCount(parent);
    if (row == -1)
        row = rowCount(parent);
    if (column == -1)
        column = 0;

    // decode and insert
    QByteArray encoded = data->data(format);
    QDataStream stream(&encoded, QIODevice::ReadOnly);


    //code based on QAbstractItemModel::decodeData
    // adapted to work with QStdItem
    int top = INT_MAX;
    int left = INT_MAX;
    int bottom = 0;
    int right = 0;
    QList<int> rows, columns;
    QList<QStdItem *> items;

    while (!stream.atEnd())
    {
        int r, c;
        QStdItem *item = d->createItem();
      //  item->setModel(this); // lucas 14.02.22
        item->d_func()->setModel(this);

      //  auto item = new QStdItem(this);
        // damit funktioniert das auto-update nach beenden eines DragDrop Vorgangs nicht mehr

        stream >> r >> c;
        d->decodeDataRecursive(stream, item);

        rows.append(r);
        columns.append(c);
        items.append(item);
        top = qMin(r, top);
        left = qMin(c, left);
        bottom = qMax(r, bottom);
        right = qMax(c, right);
    }

    // insert the dragged items into the table, use a bit array to avoid overwriting items,
    // since items from different tables can have the same row and column
    int dragRowCount = 0;
    int dragColumnCount = right - left + 1;

    // Compute the number of continuous rows upon insertion and modify the rows to match
    QList<int> rowsToInsert(bottom + 1);

    for (int i = 0; i < rows.count(); ++i)
        rowsToInsert[rows.at(i)] = 1;

    for (int i = 0; i < rowsToInsert.count(); ++i)
    {
        if (rowsToInsert.at(i) == 1)
        {
            rowsToInsert[i] = dragRowCount;
            ++dragRowCount;
        }
    }
    for (int i = 0; i < rows.count(); ++i)
        rows[i] = top + rowsToInsert.at(rows.at(i));

    QBitArray isWrittenTo(dragRowCount * dragColumnCount);

    // make space in the table for the dropped data
    int colCount = columnCount(parent);

    if (colCount < dragColumnCount + column)
    {
        insertColumns(colCount, dragColumnCount + column - colCount, parent);
        colCount = columnCount(parent);
    }

    insertRows(row, dragRowCount, parent);

    row = qMax(0, row);
    column = qMax(0, column);

    QStdItem *parentItem = itemFromIndex (parent);

    if (!parentItem)
    {    parentItem = invisibleRootItem();}

    QList<QPersistentModelIndex> newIndexes(items.size());
    // set the data in the table
    for (int j = 0; j < items.size(); ++j)
    {
        int relativeRow = rows.at(j) - top;
        int relativeColumn = columns.at(j) - left;
        int destinationRow = relativeRow + row;
        int destinationColumn = relativeColumn + column;
        int flat = (relativeRow * dragColumnCount) + relativeColumn;
        // if the item was already written to, or we just can't fit it in the table, create a new row
        if (destinationColumn >= colCount || isWrittenTo.testBit(flat))
        {
            destinationColumn = qBound(column, destinationColumn, colCount - 1);
            destinationRow = row + dragRowCount;
            insertRows(row + dragRowCount, 1, parent);
            flat = (dragRowCount * dragColumnCount) + relativeColumn;
            isWrittenTo.resize(++dragRowCount * dragColumnCount);
        }
        if (!isWrittenTo.testBit(flat))
        {
            newIndexes[j] = index(destinationRow, destinationColumn, parentItem->index());
            isWrittenTo.setBit(flat);
        }
    }

    for(int k = 0; k < newIndexes.size(); k++)
    {
        if (newIndexes.at(k).isValid())
        {
            parentItem->setChild(newIndexes.at(k).row(), newIndexes.at(k).column(), items.at(k));
        } else {
            delete items.at(k);
        }
    }

    print_end();
    return true;
}

bool QStdItemModel::contains(unsigned long long int uuid)const
{
    return d_func()->root->hasChild(uuid);
}
bool QStdItemModel::contains(QStdItem* item)const
{
    return contains(item->uuid());
}

QString QStdItemModel::filename() const
{
    return d_func()->m_filename;
}

void QStdItemModel::setFilename(const QString &filename)
{
    d_func()->m_filename=filename;
}

void QStdItemModel::save()
{
    saveToFile(filename());

}

void QStdItemModel::load()
{
    loadFromFile(filename() );
}



Path QStdItemModel::pathFromIndex(const QModelIndex &index)
{
   QModelIndex iter = index;
   Path path;
   while (iter.isValid())
   {
       path.prepend(PathItem(iter.row(), iter.column()));
       iter = iter.parent();
   }
   return path;
}


QModelIndex QStdItemModel::pathToIndex(const Path &path)
{

    UndoStackLock lck{undo_stack()};

     QModelIndex iter;
    //QModelIndex iter{-1,-1,invisibleRootItem(),this};
    // auto iter{createIndex(-1,-1,invisibleRootItem() )};

   for (int i=0;i<path.size();i++)
   {
       iter = this->index(path[i].first, path[i].second, iter);
   }
   return iter;
}


/*!
    On models that support this, moves \a count rows starting with the given
    \a sourceRow under parent \a sourceParent to row \a destinationChild under
    parent \a destinationParent.

    Returns \c{true} if the rows were successfully moved; otherwise returns
    \c{false}.

    The base class implementation does nothing and returns \c{false}.

    If you implement your own model, you can reimplement this function if you
    want to support moving. Alternatively, you can provide your own API for
    altering the data.

    \sa beginMoveRows(), endMoveRows()
*/

/*bool QStdItemModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                             const QModelIndex &destinationParent, int destinationChild)
{
    Q_D(QStdItemModel);

    QStdItem* source_item{};
    QStdItem* dest_item{};
    if(sourceParent.isValid())
    {
       // source_item = itemFromIndex(sourceParent);
        source_item= d->itemFromIndex(sourceParent);
    }else
    {
        source_item = invisibleRootItem();
    }

    if(destinationParent.isValid())
    {
        //dest_item=itemFromIndex(destinationParent);
        dest_item=d->itemFromIndex(destinationParent);
    }else
    {
        dest_item=invisibleRootItem();
    }

    if(source_item->rowCount()<sourceRow+count // cannot move non-existing (past the end) rows
            )
    {   qDebug()<< " cannot move non-existing (past the end) rows ";
        return false;
    }

    if( destinationChild - dest_item->rowCount() > 0 )// neighter can rows be inserted past-the end(with a gap between)
    {
        qDebug()<< "cannot insert row past-the end  under destination parent";
        return false;
    }

    Q_ASSERT(source_item);
    Q_ASSERT(dest_item);

auto src_parent{createIndex(sourceParent.row(),sourceParent.column(),sourceParent.internalPointer())};
auto dest_parent{createIndex(destinationParent.row(),destinationParent.column(),destinationParent.internalPointer())};

bool success{true};

     QAbstractItemModel::beginMoveRows(sourceParent, sourceRow, sourceRow+count-1,   destinationParent,  destinationChild);

        //beginMoveRows(src_parent, sourceRow, sourceRow+count,
        //                              dest_parent,  destinationChild);
     {
    // QSignalBlocker lck{this};
// beginMoveRows(source_item->index(),sourceRow,sourceRow+count,dest_item->index(),destinationChild);

  //   auto tmp_list{source_item->takeRows(sourceRow,count)};
   auto tmp_list{  source_item->d_func()->removeRows(sourceRow,count,false)};

 //   success={dest_item->d_func()->insertRows(destinationChild,count,tmp_list,false)};
      success={dest_item->d_func()->insertRows(destinationChild,tmp_list,false)};

     }
     QAbstractItemModel::endMoveRows();

     source_item->update();
     dest_item->update();

    return success;
}
*/

bool QStdItemModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                             const QModelIndex &destinationParent, int destinationChild)
{




    on_scope_exit t{ [this](){ qDebug().noquote() <<"< QStdItemModel::moveRows Macro >";
                             undo_stack()->beginMacro("QStdItemModel::moveRows");},
                     [this](){   qDebug().noquote()<< "</ QStdItemModel::moveRows Macro>";
                                 undo_stack()->endMacro();}
                   };

    auto cmd = new MoveRowsCmd(sourceParent,sourceRow,count,
                               destinationParent,destinationChild);
     undo_stack()->push(cmd);

     if(cmd)
     {
         return cmd->returnValue().toBool();
     }

    return false;

}

/*!
    On models that support this, moves \a count columns starting with the given
    \a sourceColumn under parent \a sourceParent to column \a destinationChild under
    parent \a destinationParent.

    Returns \c{true} if the columns were successfully moved; otherwise returns
    \c{false}.

    The base class implementation does nothing and returns \c{false}.

    If you implement your own model, you can reimplement this function if you
    want to support moving. Alternatively, you can provide your own API for
    altering the data.

    \sa beginMoveColumns(), endMoveColumns()
*/
bool QStdItemModel::moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count,
                                const QModelIndex &destinationParent, int destinationChild)
{
    return false;
}



void QStdItemModel::loadFromFile(const QString &filename)
{
     //qDebug()<< "<QStdItemModel::dropMimeData.´(const QMimeData* data,Qt::DropAction ,int row,int column, const QModelIndex& parent)";
  auto print_start =[this](){undo_stack()->beginMacro("QStdItemModel::loadFromFile");
                                qDebug()<< "<QStdItemModel::loadFromFile>";};
  auto print_end =[this](){  undo_stack()->endMacro();
                            qDebug()<< "</QStdItemModel::loadFromFile>";};

  print_start();



  if (!filename.isEmpty())
  {setFilename( filename);}

  if (this->filename().isEmpty())
  {throw AQP::Error(tr("no filename specified"));}

  clear(); // this clear the undo_stack as well
  UndoStackLock lock{undo_stack()}; // a load is not something we want to be able to undo

  int row{0};
  int column{0};
  auto parent{QModelIndex()};

    Q_D(QStdItemModel);

    QFile file{filename};
    if(!file.open(QIODevice::ReadOnly))
    {
        // throw some very bad exception
        throw AQP::Error("cannot open file");
    }

    // decode and insert
  //  QByteArray encoded = data->data(format);
  //  QDataStream stream(&encoded, QIODevice::ReadOnly);
    QDataStream stream{&file};


    //code based on QAbstractItemModel::decodeData
    // adapted to work with QStdItem
    int top = INT_MAX;
    int left = INT_MAX;
    int bottom = 0;
    int right = 0;
    QList<int> rows, columns;
    QList<QStdItem *> items;

    /*
    int cc,rc; // columHeaderItemCount, RowHeaderItemCount
    QList<QStdItem*> colHeaderItems;
    QList<QStdItem*> rowHeaderItems;


    stream>>cc>>rc;
    colHeaderItems.reserve(cc);
    rowHeaderItems.reserve(rc);

    // readColumnHeaderItems from stream
    for(unsigned int i{0}; i<cc;++i)
    {
        QStdItem* item= d->createItem();
        item->setModel(this); // lucas 14.02.22
        stream>>*item;
        colHeaderItems.append(item);
    }

    // read rowHeaderItems from stream
    for(unsigned int i{0}; i<rc;++i)
    {
        QStdItem* item= d->createItem();
        item->setModel(this); // lucas 14.02.22
        stream>>*item;
        rowHeaderItems.append(item);
    }
*/

    while (!stream.atEnd())
    {
        int r, c;
        QStdItem *item = d->createItem();
      //  item->setModel(this); // lucas 14.02.22
          item->d_func()->setModel(this); // lucas 14.02.22
        stream >> r >> c;
        d->decodeDataRecursive(stream, item);

        rows.append(r);
        columns.append(c);
        items.append(item);
        top = qMin(r, top);
        left = qMin(c, left);
        bottom = qMax(r, bottom);
        right = qMax(c, right);
    }

 beginResetModel();

    d->root.reset(items.takeLast());
    //invisibleRootItem()->setFlags(Qt::ItemIsDropEnabled             );
    invisibleRootItem()->d_func()->setFlags(Qt::ItemIsDropEnabled             );



    qDeleteAll(d->columnHeaderItems);
    d->columnHeaderItems.clear();
//    d->columnHeaderItems=std::move(colHeaderItems);

    qDeleteAll(d->rowHeaderItems);
    d->rowHeaderItems.clear();
//    d->rowHeaderItems=std::move(rowHeaderItems);

    print_end();



    endResetModel();
}

void QStdItemModel::saveToFile(const QString& filename,const QModelIndex& selection)
{ scope_tagger t{ "QStdItemModel::saveToFile"};

    if (!filename.isEmpty())
        d_func()->m_filename = filename;

    if (d_func()->m_filename.isEmpty())
    {  throw AQP::Error(tr("no filename specified"));
    }


    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);

    QSaveFile file(filename);
    file.open(QIODevice::WriteOnly);



    QSet<QStdItem*> itemsSet;
    QStack<QStdItem*> stack;
    itemsSet.reserve(1);
    stack.reserve(1);

    QStdItem* item;
    if(selection.isValid())
    {item=itemFromIndex(selection);
    }
     else
    {    item = invisibleRootItem();
    }

            itemsSet << item;
            stack.push(item);


    //remove duplicates children
 /*   {
        QDuplicateTracker<QStdItem *> seen;

        while (!stack.isEmpty())
        {
            QStdItem *itm = stack.pop();
            if (seen.hasSeen(itm))
                continue;

            const QList<QStdItem*> &childList = itm->d_func()->children;

            for (int i = 0; i < childList.count(); ++i)
            {
                QStdItem *chi = childList.at(i);
                if (chi)
                {
                    itemsSet.remove(chi);
                    stack.push(chi);
                }
            }
        }

        // after the while-loop the stack is empty !
    }


    stack.reserve(itemsSet.count());

    for (QStdItem *item : qAsConst(itemsSet))
        stack.push(item);
*/


            /*
    QList<QStdItem *> columnHeaderItems;
    QList<QStdItem *> rowHeaderItems;

diese information von QStdItemModelPrivate muss auch serialisiert werden ! */

    //        stream<< d_func()->columnHeaderItems.count() << d_func()->rowHeaderItems.count();
/*
            for(const auto& i :d_func()->columnHeaderItems)
            {
                if(i){
                stream<<*i;
                }else // the HeaderItems can be nullptr !
                {
                    stream<<*new char[sizeof(QStdItem)];
                }
            }

            for(const auto& i :d_func()->rowHeaderItems)
            {
                if(i){
                stream<<*i;
                }else
                {
                    stream<<*new char[sizeof(QStdItem)];
                }
            }
*/

    //stream everything recursively
    while (!stack.isEmpty())
    {
        QStdItem *item = stack.pop();

        if (itemsSet.contains(item)) //if the item is selection 'top-level', stream its position
            stream << item->row() << item->column();

        stream << *item << item->columnCount() << int(item->d_ptr->children.count());
        stack += item->d_ptr->children;
    }

    // here the stack is empty again !

   // data->setData(format, encoded);

    file.write(encoded);

    file.commit();


}


// QT_END_NAMESPACE

#include "moc_qstditemmodel.cpp"

