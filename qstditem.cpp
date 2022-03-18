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
#include "qstditemmodel_p.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qpair.h>
#include <QtCore/qvariant.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qbitarray.h>
#include <QtCore/qmimedata.h>
#include <QtCore/qiodevice.h>
#include <QtCore/private/qduplicatetracker_p.h>
#include"scope_tagger.h"


#include <qdebug.h>
#include <algorithm>


#include "qstditemcommands.h"

// time_point_type stream i/o operators rely on ints duration type operators
QDataStream &operator<<(QDataStream &out, const time_point_type &myObj)
{
    out << myObj.time_since_epoch();
    return out;
}
QDataStream &operator>>(QDataStream &in, time_point_type &myObj)
{   time_point_type::duration dur;
    in >>dur;
    myObj=time_point_type(dur);
    return in;
}

QDataStream &operator<<(QDataStream &out, const duration_type &myObj)
{
    out<< myObj.count();
    return out;
}
QDataStream &operator>>(QDataStream &in, duration_type &myObj)
{   int count{0};
    in >> count;
    myObj=duration_type(count);
    return in;
}

QMap<int,QVariant> QStdItem::itemData()const
{
    Q_D(const QStdItem);

    return d->itemData();

}

void QStdItem::setItemData(const QMap<int, QVariant> &roles)
{
    scope_tagger t{ "QStdItem::setItemData"};


    if(model())
    {
        model()->undo_stack()->push(new SetDataCmd(this,roles));
    }
    else
    {
        auto tmp{std::make_unique<SetDataCmd>(this,roles)};
        tmp->redo();

    }
//update();

}


QStdItem::QStdItem(QStdItemModel* m)
   : QStdItem(*new QStdItemPrivate(m) )
{
}

QStdItem::QStdItem()
   : QStdItem(*new QStdItemPrivate)
{
}


QStdItem::QStdItem(const QString &text)
   : QStdItem(*new QStdItemPrivate)
{
  // setText(text);
    d_func()->setData( Qt::DisplayRole,text);
}


QStdItem::QStdItem(const QIcon &icon, const QString &text)
   : QStdItem(text)
{
   //setIcon(icon);
    d_func()->setData( Qt::DecorationRole,icon);
}

/*!
  Constructs an item with \a rows rows and \a columns columns of child items.
*/
QStdItem::QStdItem(int rows, int columns)
   : QStdItem(*new QStdItemPrivate)
{
     Q_D(QStdItem);
   d->setRowCount_impl(rows);
  d-> setColumnCount_impl(columns);
}

void QStdItem::update()
{
    scope_tagger t{"QStdItem::update"};
}

QStdItem::QStdItem(QStdItemPrivate &dd)
   : d_ptr(&dd)
{
   Q_D(QStdItem);
   d->q_ptr = this;
}

QStdItem::QStdItem(QStdItemPrivate *dd)
   : d_ptr(dd)
{
   Q_D(QStdItem);
   d->q_ptr = this;
}

/*!
 Constructs a copy of \a other. Note that model() is
 not copied.

 This function is useful when reimplementing clone().
*/
QStdItem::QStdItem(const QStdItem &other)
   : d_ptr(new QStdItemPrivate)
{
   Q_D(QStdItem);
   d->q_ptr = this;
   operator=(other);
}

/*!
 Assigns \a other's data and flags to this item. Note that
 type() and model() are not copied. Neigther are its children or its uuid

 This function is useful when reimplementing clone().
*/
QStdItem &QStdItem::operator=(const QStdItem &other)
{
   Q_D(QStdItem);
   d->values = other.d_func()->values;
   return *this;
}

unsigned long long QStdItem::uuid()const
{
    Q_D(const QStdItem);
    if(d)
    {return d->uuid();}
    else{return -1;}
}
/*!
 Destructs the item.
 This causes the item's children to be destructed as well.
*/
QStdItem::~QStdItem()
{

   scope_tagger t {"QStdItem::~QStdItem()"};

   Q_D(QStdItem);
   for (QStdItem *child : qAsConst(d->children))
   {
       if (child)
           child->d_func()->setModel(nullptr);
       delete child;
   }
   d->children.clear();
   if (d->parent && d->model)
       d->parent->d_func()->childDeleted(this);
}

/*!
 Returns the item's parent item, or \nullptr if the item has no parent.
 \note For toplevel items parent() returns \nullptr. To receive toplevel
 item's parent use QStdItemModel::invisibleRootItem() instead.

 \sa child(), QStdItemModel::invisibleRootItem()
*/
QStdItem *QStdItem::parent() const
{
   Q_D(const QStdItem);
   if (!d->model || (d->model->d_func()->root.data() != d->parent))
       return d->parent;
   return nullptr;
}

/*!
   Sets the item's data for the given \a role to the specified \a value.

   If you subclass QStdItem and reimplement this function, your
   reimplementation should call emitDataChanged() if you do not call
   the base implementation of setData(). This will ensure that e.g.
   views using the model are notified of the changes.

   \note The default implementation treats Qt::EditRole and Qt::DisplayRole
   as referring to the same data.

   \sa Qt::ItemDataRole, data(), setFlags()
*/
void QStdItem::setData(const QVariant &value, int role)
{
scope_tagger t{ "QStdItem::setData(const QVariant& value,int role)"};

   // if the item is not contained in a model (its 'model' pointer is null)
   // there is no undo_stack, we could push the command onto
   if(model() )
   {

       model()->undo_stack()->push( new SetDataCmd(this,value,role ) );
   }else
   {
           auto ptr= std::make_shared<SetDataCmd>(this,value,role);
           ptr->redo();
   }


}


void QStdItem::clearData()
{
  scope_tagger t{ "QStdItem::clearData"};

   if(model())
   {model()->undo_stack()->push( new ClearDataCmd(this) );}
   else
   {
       auto ptr= std::make_shared<ClearDataCmd>(this);
       ptr->redo();
   }


}

/*!
   Returns the item's data for the given \a role, or an invalid
   QVariant if there is no data for the role.

   \note The default implementation treats Qt::EditRole and Qt::DisplayRole
   as referring to the same data.
*/
QVariant QStdItem::data(int role) const
{
   Q_D(const QStdItem);
   const int r = (role == Qt::EditRole) ? Qt::DisplayRole : role;
   for (const auto &value : d->values) {
       if (value.role == r)
           return value.value;
   }
   return QVariant();
}

/*!
   \since 6.0

   Fills the \a roleDataSpan span with the data from this item.

   The default implementation simply calls data() for each role
   in the span.

   \sa data()
*/
void QStdItem::multiData(QModelRoleDataSpan roleDataSpan) const
{
   for (auto &roleData : roleDataSpan)
       roleData.setData(data(roleData.role()));
}

/*!
 \since 4.4

 Causes the model associated with this item to emit a
 \l{QAbstractItemModel::dataChanged()}{dataChanged}() signal for this
 item.

 You normally only need to call this function if you have subclassed
 QStdItem and reimplemented data() and/or setData().

 \sa setData()
*/
void QStdItem::emitDataChanged()
{
  scope_tagger t{"QStdItem::emitDataChanged"};

   Q_D(QStdItem);
   if (d->model)
       d->model->d_func()->itemChanged(this);


}


void QStdItem::setFlags(Qt::ItemFlags flags)
{
  scope_tagger t{ "QStdItem::setFlags(Qt::ItemFlags flags)"};
       setData((int)flags, Qt::UserRole - 1);

}

Qt::ItemFlags QStdItem::flags() const
{
   QVariant v = data(Qt::UserRole - 1);
   if (!v.isValid())
       return (Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable
               |Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled);
   return Qt::ItemFlags(v.toInt());
}




void QStdItem::setEnabled(bool enabled)
{
   scope_tagger t{ "QStdItem::setEnabled(bool enabled)"};

   Q_D(QStdItem);
   d->changeFlags(enabled, Qt::ItemIsEnabled);

}

void QStdItem::setEditable(bool editable)
{
   scope_tagger t{ "QStdItem::setEditable(bool editable)"};

   Q_D(QStdItem);
   d->changeFlags(editable, Qt::ItemIsEditable);

}

void QStdItem::setSelectable(bool selectable)
{
   scope_tagger t{ "QStdItem::setSelectable(bool selectable)"};
       Q_D(QStdItem);
       d->changeFlags(selectable, Qt::ItemIsSelectable);

}

void QStdItem::setCheckable(bool checkable)
{
   scope_tagger t{ "QStdItem::setCheckable(bool checkable)"};

   Q_D(QStdItem);
   if (checkable && !isCheckable()) {
       // make sure there's data for the checkstate role
       if (!data(Qt::CheckStateRole).isValid())
           setData(Qt::Unchecked, Qt::CheckStateRole);
   }
   d->changeFlags(checkable, Qt::ItemIsUserCheckable);

}



/*!
 Determines that the item is tristate and controlled by QTreeWidget if \a tristate
 is \c true.
 This enables automatic management of the state of parent items in QTreeWidget
 (checked if all children are checked, unchecked if all children are unchecked,
 or partially checked if only some children are checked).

 \since 5.6
 \sa isAutoTristate(), setCheckable(), setCheckState()
*/
void QStdItem::setAutoTristate(bool tristate)
{
   Q_D(QStdItem);
   d->changeFlags(tristate, Qt::ItemIsAutoTristate);
}

/*!
 \fn bool QStdItem::isAutoTristate() const

 Returns whether the item is tristate and is controlled by QTreeWidget.

 The default value is false.

 \since 5.6
 \sa setAutoTristate(), isCheckable(), checkState()
*/

/*!
 Sets whether the item is tristate and controlled by the user.
 If \a tristate is true, the user can cycle through three separate states;
 otherwise, the item is checkable with two states.
 (Note that this also requires that the item is checkable; see isCheckable().)

 \since 5.6
 \sa isUserTristate(), setCheckable(), setCheckState()
*/
void QStdItem::setUserTristate(bool tristate)
{
   scope_tagger t{ "QStdItem::setTristate"};

   Q_D(QStdItem);
   d->changeFlags(tristate, Qt::ItemIsUserTristate);

}

/*!
 \fn bool QStdItem::isUserTristate() const
 \since 5.6

 Returns whether the item is tristate; that is, if it's checkable with three
 separate states and the user can cycle through all three states.

 The default value is false.

 \sa setUserTristate(), isCheckable(), checkState()
*/

#if QT_CONFIG(draganddrop)


void QStdItem::setDragEnabled(bool dragEnabled)
{
   scope_tagger t{ "QStdItem::setDragEnabled"};
   Q_D(QStdItem);
   d->changeFlags(dragEnabled, Qt::ItemIsDragEnabled);

}


void QStdItem::setDropEnabled(bool dropEnabled)
{
   scope_tagger t{"QStdItem::setDropEnabled(bool dropEnabled)"};

   Q_D(QStdItem);
   d->changeFlags(dropEnabled, Qt::ItemIsDropEnabled);


}



#endif // QT_CONFIG(draganddrop)

/*!
 Returns the row where the item is located in its parent's child table, or
 -1 if the item has no parent.

 \sa column(), parent()
*/
int QStdItem::row() const
{
   Q_D(const QStdItem);
   QPair<int, int> pos = d->position();
   return pos.first;
}

/*!
 Returns the column where the item is located in its parent's child table,
 or -1 if the item has no parent.

 \sa row(), parent()
*/
int QStdItem::column() const
{
   Q_D(const QStdItem);
   QPair<int, int> pos = d->position();
   return pos.second;
}

/*!
 Returns the QModelIndex associated with this item.

 When you need to invoke item functionality in a QModelIndex-based API (e.g.
 QAbstractItemView), you can call this function to obtain an index that
 corresponds to the item's location in the model.

 If the item is not associated with a model, an invalid QModelIndex is
 returned.

 \sa model(), QStdItemModel::itemFromIndex()
*/
QModelIndex QStdItem::index() const
{
   Q_D(const QStdItem);
   return d->model ? d->model->indexFromItem(this) : QModelIndex();
}

QList<QStdItem*> QStdItem::children()const
{ Q_D(const QStdItem);
    return d->get_children();
}

QList<QModelIndex> QStdItem::childIndexes()const
{
    Q_D(const QStdItem);
    auto childs{d->get_children()};

    QModelIndexList indexes;

    for(auto* child : childs)
    {
        if(child)
        {
            indexes.append(child->index());
        }

    }

return indexes;

}

/*!
 Returns the QStdItemModel that this item belongs to.

 If the item is not a child of another item that belongs to the model, this
 function returns \nullptr.

 \sa index()
*/
QStdItemModel *QStdItem::model() const
{
   Q_D(const QStdItem);
   return d->model;
}

/*!
   Sets the number of child item rows to \a rows. If this is less than
   rowCount(), the data in the unwanted rows is discarded.

   \sa rowCount(), setColumnCount()
*/
void QStdItem::setRowCount(int rows)
{
   scope_tagger t{ "QStdItem::setRowCount"};

   if(model())
   {model()->undo_stack()->push(new SetRowCountCmd(this,rows));}
   else
   {
       auto ptr= std::make_shared<SetRowCountCmd>(this,rows);
       ptr->redo();
   }
//update();
}

/*!
   Returns the number of child item rows that the item has.

   \sa setRowCount(), columnCount()
*/
int QStdItem::rowCount() const
{
   Q_D(const QStdItem);
   return d->rowCount();
}

/*!
   Sets the number of child item columns to \a columns. If this is less than
   columnCount(), the data in the unwanted columns is discarded.

   \sa columnCount(), setRowCount()
*/
void QStdItem::setColumnCount(int columns)
{
   scope_tagger t{"QStdItem::setColumnCount"};

   if(model())
   {model()->undo_stack()->push(new SetColumnCountCmd(this,columns));}
   else
   {
       auto ptr= std::make_shared<SetColumnCountCmd>(this,columns);
       ptr->redo();
   }
//update();
}

/*!
   Returns the number of child item columns that the item has.

   \sa setColumnCount(), rowCount()
*/
int QStdItem::columnCount() const
{
   Q_D(const QStdItem);
   return d->columnCount();
}

/*!
   Inserts a row at \a row containing \a items. If necessary, the column
   count is increased to the size of \a items.

   \sa insertRows(), insertColumn()
*/
void QStdItem::insertRow(int row, const QList<QStdItem*> &items)
{
  scope_tagger t{ "QStdItem::insertRow(int row, const QList<QStdItem*>& items)"};


if(model())
{
   model()->undo_stack()->push( new InsertRowCmd(this,row,items,true) );
}
else
{
   auto ptr= std::make_shared<InsertRowCmd>(this,row,items,true);
   ptr->redo();
}
//update();
}

// VARIANTE I
void QStdItem::insertRows(int row, const QList<QStdItem*> &items)
{
  scope_tagger t{ "QStdItem::insertRows(int row, const QList<QStdItem*>& items)"};

if(model())
{model()->undo_stack()->push(new InsertRowCmd(this,row,items,false));}
else
{
   auto ptr= std::make_shared<InsertRowCmd>(this,row,items,false);
   ptr->redo();
}
//update();
}

/*!
   Inserts a column at \a column containing \a items. If necessary,
   the row count is increased to the size of \a items.

   \sa insertColumns(), insertRow()
*/

// VARIANTE I
void QStdItem::insertColumn(int column, const QList<QStdItem*> &items)
{
  scope_tagger t{ "QStdItem::insertColumn(int column, const QList<QStdItem*>& items)"};

   if(model())
   {model()->undo_stack()->push(new InsertColumnCmd(this,column, items) );}
   else
   {
       auto ptr= std::make_shared<InsertColumnCmd>(this,column,items);
       ptr->redo();
   }

//update();
}

// VARIANTE II
void QStdItem::insertRows(int row, int count)
{
   scope_tagger t{"QStdItem::insertRows(int row,int count)"};

   if(model())
   {model()->undo_stack()->push(new InsertRowCmd(this, row, count ) );}
   else{
       auto ptr= std::make_shared<InsertRowCmd>(this,row,count);
       ptr->redo();
   }
//update();
}

// VARIANTE II
void QStdItem::insertColumns(int column, int count)
{
 scope_tagger t{"QStdItem::insertColumns(int column,int count)"};

   if(model())
   {
       auto cmd {new InsertColumnCmd(this,column,count)};


       model()->undo_stack()->push( cmd);}
   else
   {
       auto ptr= std::make_shared<InsertColumnCmd>(this,column,count);
       ptr->redo();
   }
//update();
}

bool QStdItem::hasChild(unsigned long long int uuid)const
{
    return d_func()->hasChild(uuid);
}


void QStdItem::removeRow(int row)
{
   scope_tagger t{ "QStdItem::removeRow(int row)"};

   removeRows(row, 1);
//update();
}

/*!
   Removes the given \a column. The items that were in the
   column are deleted.

   \sa takeColumn(), removeColumns(), removeRow()
*/
void QStdItem::removeColumn(int column)
{
   scope_tagger t{ "QStdItem::removeColumn(int columns)"};
   removeColumns(column, 1);
//update();
}

/*!
   Removes \a count rows at row \a row. The items that were in those rows are
   deleted.

   \sa removeRow(), removeColumn()
*/
void QStdItem::removeRows(int row, int count)
{
  scope_tagger t{ "QStdItem::removeRows(int row,int count)"};

if(model())
{model()->undo_stack()->push(new RemoveRowsCmd(this,row,count) );
}
else
{
   auto ptr= std::make_shared<RemoveRowsCmd>(this,row,count);
   ptr->redo();
}
//update();
}

/*!
   Removes \a count columns at column \a column. The items that were in those
   columns are deleted.

   \sa removeColumn(), removeRows()
*/
void QStdItem::removeColumns(int column, int count)
{
  scope_tagger t{"QStdItem::removeColumns(int column, int count)"};

   if(model())
   {model()->undo_stack()->push(new RemoveColumnsCmd(this,column,count) );}
   else
   {
       auto ptr= std::make_shared<RemoveColumnsCmd>(this,column,count);
       ptr->redo();
   }
//update();
}


bool QStdItem::hasChildren() const
{
   return (rowCount() > 0) && (columnCount() > 0);
}

/*!
   Sets the child item at (\a row, \a column) to \a item. This item (the parent
   item) takes ownership of \a item. If necessary, the row count and column
   count are increased to fit the item.

   \note Passing \nullptr as \a item removes the item.

   \sa child()
*/
void QStdItem::setChild(int row, int column, QStdItem *item)
{
  scope_tagger t{"QStdItem::setChild(int row,int column, QStdItem* item)"};

   if(model())
   {
       model()->undo_stack()->push(new SetChildCmd(this,row,column,item) );
   }
   else
   {
       auto tmp{std::make_unique<SetChildCmd>(this,row,column,item)};
       tmp->redo();
   }

//update();
}


QStdItem *QStdItem::child(int row, int column) const
{
   Q_D(const QStdItem);
   int index = d->childIndex(row, column);
   if (index == -1)
       return nullptr;
   return d->children.at(index);
}


/*!
   Removes the child item at (\a row, \a column) without deleting it, and returns
   a pointer to the item. If there was no child at the given location, then
   this function returns \nullptr.

   Note that this function, unlike takeRow() and takeColumn(), does not affect
   the dimensions of the child table.

   \sa child(), takeRow(), takeColumn()
*/
QStdItem *QStdItem::takeChild(int row, int column)
{
   scope_tagger t{"QStdItem::takeChild(int row,int column)"};

   Q_D(QStdItem);

   auto tmp{ d->takeChild(row,column)};
   update();
   return tmp;
}

/*!
   Removes \a row without deleting the row items, and returns a list of
   pointers to the removed items. For items in the row that have not been
   set, the corresponding pointers in the list will be \nullptr.

   \sa removeRow(), insertRow(), takeColumn()
*/
QList<QStdItem*> QStdItem::takeRow(int row)
{

  scope_tagger t{ "QStdItem::takeRow"};

   Q_D(QStdItem);

  auto tmp{d->takeRow(row)};
  update();
  return tmp;
}

/*!
   Removes \a column without deleting the column items, and returns a list of
   pointers to the removed items. For items in the column that have not been
   set, the corresponding pointers in the list will be \nullptr.

   \sa removeColumn(), insertColumn(), takeRow()
*/
QList<QStdItem*> QStdItem::takeColumn(int column)
{
  scope_tagger t{ "QStdItem::takeColumn"};

   Q_D(QStdItem);

  auto tmp{ d->takeColumn(column)};
  update();
  return tmp;
}

/*!
   Returns \c true if this item is less than \a other; otherwise returns \c false.

   The default implementation uses the data for the item's sort role (see
   QStdItemModel::sortRole) to perform the comparison if the item
   belongs to a model; otherwise, the data for the item's Qt::DisplayRole
   (text()) is used to perform the comparison.

   sortChildren() and QStdItemModel::sort() use this function when
   sorting items. If you want custom sorting, you can subclass QStdItem
   and reimplement this function.
*/
bool QStdItem::operator<(const QStdItem &other) const
{
   const int role = model() ? model()->sortRole() : Qt::DisplayRole;
   const QVariant l = data(role), r = other.data(role);
   return QAbstractItemModelPrivate::isVariantLessThan(l, r);
}

/*!
   Sorts the children of the item using the given \a order, by the values in
   the given \a column.

   \note This function is recursive, therefore it sorts the children of the
   item, its grandchildren, etc.

   \sa {operator<()}
*/
void QStdItem::sortChildren(int column, Qt::SortOrder order)
{

   scope_tagger t{"QStdItem::sortChildren(int column,Qt::SortOrder )"};

   Q_D(QStdItem);
   if ((column < 0) || (rowCount() == 0))
   {       return;
   }

   QList<QPersistentModelIndex> parents;
   if (d->model)
   {
       parents << index();
       emit d->model->layoutAboutToBeChanged(parents, QAbstractItemModel::VerticalSortHint);
   }

   d->sortChildren(column, order);
   if (d->model)
       emit d->model->layoutChanged(parents, QAbstractItemModel::VerticalSortHint);

update();
   }

/*!
   Returns a copy of this item. The item's children are not copied.

   When subclassing QStdItem, you can reimplement this function
   to provide QStdItemModel with a factory that it can use to
   create new items on demand.

   \sa QStdItemModel::setItemPrototype(), operator=()
*/
QStdItem *QStdItem::clone() const
{
   scope_tagger t{"QStdItem::clone"};

   auto tmp{ new QStdItem(*this)};

   return tmp;
}

/*!
   Returns the type of this item. The type is used to distinguish custom
   items from the base class. When subclassing QStdItem, you should
   reimplement this function and return a new value greater than or equal
   to \l UserType.

   \sa QStdItem::Type
*/
int QStdItem::type() const
{
   return Type;
}

#ifndef QT_NO_DATASTREAM

/*!
   Reads the item from stream \a in. Only the data and flags of the item are
   read, not the child items.

   \sa write()
*/
void QStdItem::read(QDataStream &in)
{
  scope_tagger t{ "QStdItem::read(QDataStream& )"};

   Q_D(QStdItem);
 /* qulonglong ptrval;
  in >> ptrval;
  d->model = reinterpret_cast<QStdItemModel*>(ptrval);*/


  QByteArray ptr_val;
  in>> ptr_val;
  QStdItemModel* m_model = *reinterpret_cast<QStdItemModel**>( ptr_val.data() );

  if(d->model==m_model)
{
      qDebug("code war richtig , test bestanden");
  }
//  setModel(m_model);

  //..............................................
   in >> d->values;
   qint32 flags;
   in >> flags;
  // setFlags(Qt::ItemFlags(flags));

   // no undoCommand recording here
    d_func()->setFlags(Qt::ItemFlags(flags));
    // it is better to use this method in QStdItemPrivate
   // to be able to sometimes circumvent the undo framework
   // this promises to increase performance compared to locking the undo_stack all the time

}

/*!
   Writes the item to stream \a out. Only the data and flags of the item
   are written, not the child items.

   \sa read()
*/
void QStdItem::write(QDataStream &out) const
{

  scope_tagger t{"QStdItem::write(QDataStream&)"};
   Q_D(const QStdItem);

 // qulonglong ptrval{*reinterpret_cast<qulonglong*>( d->model ) };

 // out<< ptrval;// out << d->model;

  QByteArray ptr_data=  QByteArray::fromRawData(reinterpret_cast<char*>(model()),sizeof(QStdItemModel*) );
out << ptr_data;
//....................................
   out << d->values;
   out << flags();

}

/*!
   \relates QStdItem
   \since 4.2

   Reads a QStdItem from stream \a in into \a item.

   This operator uses QStdItem::read().

   \sa {Serializing Qt Data Types}
*/
TEST_LIB_EXPORT QDataStream & operator>>(QDataStream &in, QStdItem &item)
{
   item.read(in);
   return in;
}

/*!
   \relates QStdItem
   \since 4.2

   Writes the QStdItem \a item to stream \a out.

   This operator uses QStdItem::write().

   \sa {Serializing Qt Data Types}
*/
TEST_LIB_EXPORT QDataStream &operator<<(QDataStream &out, const QStdItem &item)
{
   item.write(out);
   return out;
}

#endif // QT_NO_DATASTREAM
