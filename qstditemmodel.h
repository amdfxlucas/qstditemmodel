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

#ifndef QSTDITEMMODEL_H
#define QSTDITEMMODEL_H
//#include <QUndoCommand>
#include "undostack.h"

// #include "qstditemmodel_p.h"

#include <QtGui/qtguiglobal.h>
#include <QtCore/qabstractitemmodel.h>
//#include "qabstractitemmodel.h"
#include <QtGui/qbrush.h>
#include <QtGui/qfont.h>
#include <QtGui/qicon.h>
#ifndef QT_NO_DATASTREAM
#include <QtCore/qdatastream.h>
#endif

QT_REQUIRE_CONFIG(standarditemmodel);

QT_BEGIN_NAMESPACE

#include "path.h"

#include "qstditem.h"


class QStdItemModelPrivate;



// Path pathFromIndex(const QModelIndex &index);
// QModelIndex pathToIndex(const Path &path, const QAbstractItemModel *model);


class QStdItemModel
        : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(int sortRole READ sortRole WRITE setSortRole BINDABLE bindableSortRole)
protected:
    class CutItemCmd;
    class PasteItemCmd;
    class MoveRowsCmd;

    class QStdItemModelCmd    ; // base class for commands

    class SetVHeaderItemCmd;
    class SetHHeaderItemCmd;
public:
    using move_rows_cmd_t= MoveRowsCmd;
    using item_cmd_t = QStdItem::StdItemCmd;
    using set_item_data_cmd_t =  QStdItem::SetDataCmd;
    using remove_row_cmd_t = QStdItem::RemoveRowsCmd;

private slots:
    void connectRefCtrl();

public:
    enum Behaviour {AsChild,AsSibling,Absolute};

    void  iterate(const auto& func,const QModelIndex & index=QModelIndex()) const;
     bool  iterate_interuptable(const auto& func,const QModelIndex & index=QModelIndex()) const;
    virtual QModelIndexList find(const QModelIndex& start_node, int role, const QVariant& key) const;
    std::optional<QModelIndex> find_uid(unsigned long long uid,const QModelIndex& start_node=QModelIndex()) const;


 static   Path pathFromIndex(const QModelIndex &index);
    QModelIndex pathToIndex(const Path &path);

    static Path parentPath(const Path& );

    QString filename() const ;
    void setFilename(const QString &filename);

    void save();
    void load();

    void saveToFile(const QString& filename= QString(),const QModelIndex& selection=QModelIndex()) ;
    void loadFromFile(const QString& filename=QString()  );

    bool contains(unsigned long long int uuid)const;
    bool contains(QStdItem* item)const;

virtual bool canAcceptCut(const QModelIndex&)const;
virtual bool canAcceptPaste(const QModelIndex&)const;
    QModelIndex cut(const QModelIndex &index);
    bool hasCutItem() const ;
    QModelIndex paste(const QModelIndex &index,Behaviour b = AsSibling);

    UndoStack* undo_stack() const{return m_stack;};

    explicit QStdItemModel(QObject *parent = nullptr);
    QStdItemModel(int rows, int columns, QObject *parent = nullptr);
    ~QStdItemModel();

    void setItemRoleNames(const QHash<int,QByteArray> &roleNames);
    QHash<int, QByteArray> roleNames() const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool clearItemData(const QModelIndex &index) override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                       int role = Qt::EditRole) override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;



    virtual bool	moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count,
                                const QModelIndex &destinationParent, int destinationChild)override;

    // inlined (no need to override)
    //   bool	moveColumn(const QModelIndex &sourceParent, int sourceColumn,
    //                      const QModelIndex &destinationParent, int destinationChild) ;
   // bool	moveRow(const QModelIndex &sourceParent, int sourceRow,
   //                 const QModelIndex &destinationParent, int destinationChild);

    virtual bool	moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                             const QModelIndex &destinationParent, int destinationChild)override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDropActions() const override;

    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override;

    void clear();

    using QObject::parent;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    QStdItem *itemFromIndex(const QModelIndex &index) const;
    QModelIndex indexFromItem(const QStdItem *item) const;

    QStdItem *item(int row, int column = 0) const;
    void setItem(int row, int column, QStdItem *item);
    inline void setItem(int row, QStdItem *item);

    // warum gibt es nicht 'insertItem(const QModelIndex& index, QStdItem* item)' ??

    QStdItem *invisibleRootItem() const;

    QStdItem *horizontalHeaderItem(int column) const;
    void setHorizontalHeaderItem(int column, QStdItem *item);
    QStdItem *verticalHeaderItem(int row) const;
    void setVerticalHeaderItem(int row, QStdItem *item);

    void setHorizontalHeaderLabels(const QStringList &labels);
    void setVerticalHeaderLabels(const QStringList &labels);

    void setRowCount(int rows);
    void setColumnCount(int columns);

    void appendRow(const QList<QStdItem*> &items);
    void appendColumn(const QList<QStdItem*> &items);
    inline void appendRow(QStdItem *item);

    void insertRow(int row, const QList<QStdItem*> &items);
    void insertColumn(int column, const QList<QStdItem*> &items);
    inline void insertRow(int row, QStdItem *item);

    inline bool insertRow(int row, const QModelIndex &parent = QModelIndex());
    inline bool insertColumn(int column, const QModelIndex &parent = QModelIndex());

    QStdItem *takeItem(int row, int column = 0);
    QList<QStdItem*> takeRow(int row);
    QList<QStdItem*> takeColumn(int column);

    QStdItem *takeHorizontalHeaderItem(int column);
    QStdItem *takeVerticalHeaderItem(int row);

    const QStdItem *itemPrototype() const;
    void setItemPrototype(const QStdItem *item);

    QList<QStdItem*> findItems(const QString &text,
                                    Qt::MatchFlags flags = Qt::MatchExactly,
                                    int column = 0) const;

    int sortRole() const;
    void setSortRole(int role);
    QBindable<int> bindableSortRole();

    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData (const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    bool canDropMimeData(const QMimeData *data,
        Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;

Q_SIGNALS:
    void itemChanged(QStdItem *item);

protected:
    QStdItemModel(QStdItemModelPrivate &dd, QObject *parent = nullptr);

    static void setModel(QStdItemModel* model,QStdItem* item)
    {item->setModel(model);}

    void setModel(QStdItem* item)
    {item->setModel(this);}
     Q_DECLARE_PRIVATE(QStdItemModel)
private:




    mutable UndoStack* m_stack{nullptr};

    friend class QStdItemPrivate;
    friend class QStdItem;
    Q_DISABLE_COPY(QStdItemModel)


    Q_PRIVATE_SLOT(d_func(), void _q_emitItemChanged(const QModelIndex &topLeft,
                                                     const QModelIndex &bottomRight))
};



void  QStdItemModel::iterate(const auto& func,const QModelIndex & index) const
 {
    if (index.isValid())
    {
    // Do action here
      func(this->itemFromIndex(index));
   // func(static_cast<QStdItem*>(index.internalPointer()) );

    }



    if (!this->hasChildren(index)
            // || (index.flags() &    Qt::ItemNeverHasChildren)
            )
    {
        return ;
    }

    auto item{index.isValid() ? this->itemFromIndex(index) : invisibleRootItem() };
    if(item)
    {
    // auto rows = this->rowCount(index);
        auto rows = item->rowCount();

    for (int i = 0; i < rows; ++i)
     {/*
        auto child_idx{this->index(i, 0, index)};
        iterate(func,child_idx);*/

        auto child{item->child(i,0)};
        if(child)
        {iterate(func,child->index());
        }
    }
    }
}


    bool  QStdItemModel::iterate_interuptable(const auto& func,const QModelIndex & index) const
     {
        bool cont {true}; // shall search continue ?! or do we already have found what we were after

        if (index.isValid())
        {
        // Do action here
        cont =   func(this->itemFromIndex(index));
       // func(static_cast<QStdItem*>(index.internalPointer()) );

        }

        if(!cont)return false;

        if (!this->hasChildren(index)
                // || (index.flags() &    Qt::ItemNeverHasChildren)
                )
        {
            return true;
        }

        auto item{index.isValid() ? this->itemFromIndex(index) : invisibleRootItem() };
        if(item)
        {
        // auto rows = this->rowCount(index);
            auto rows = item->rowCount();

        for (int i = 0; i < rows; ++i)
         {/*
            auto child_idx{this->index(i, 0, index)};
            iterate(func,child_idx);*/

            auto child{item->child(i,0)};
            if(child)
            {       //cont = iterate_interuptable(func,child->index());
                cont = iterate_interuptable(func,indexFromItem(child));
                  if(!cont)return false;
            }
        }
        }
    }


inline void QStdItemModel::setItem(int arow, QStdItem *aitem)
{ setItem(arow, 0, aitem); }

inline void QStdItemModel::appendRow(QStdItem *aitem)
{ appendRow(QList<QStdItem*>() << aitem); }

inline void QStdItemModel::insertRow(int arow, QStdItem *aitem)
{ insertRow(arow, QList<QStdItem*>() << aitem); }

inline bool QStdItemModel::insertRow(int arow, const QModelIndex &aparent)
{ return QAbstractItemModel::insertRow(arow, aparent); }

inline bool QStdItemModel::insertColumn(int acolumn, const QModelIndex &aparent)
{ return QAbstractItemModel::insertColumn(acolumn, aparent); }

#ifndef QT_NO_DATASTREAM
Q_GUI_EXPORT QDataStream &operator>>(QDataStream &in, QStdItem &item);
Q_GUI_EXPORT QDataStream &operator<<(QDataStream &out, const QStdItem &item);
#endif


QT_END_NAMESPACE

#endif //QSTANDARDITEMMODEL_H
