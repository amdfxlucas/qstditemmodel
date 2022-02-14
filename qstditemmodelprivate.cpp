
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

#include "scope_tagger.h"

#include <qdebug.h>
#include <algorithm>


QStdItemModelPrivate::QStdItemModelPrivate(QStdItemModel* m)
   : root(new QStdItem(m)), itemPrototype(nullptr)
{
   root->setFlags(Qt::ItemIsDropEnabled);
}


QStdItemModelPrivate::QStdItemModelPrivate()
   : root(new QStdItem), itemPrototype(nullptr)
{
   root->setFlags(Qt::ItemIsDropEnabled);
}


QStdItemModelPrivate::~QStdItemModelPrivate()
{
}


void QStdItemModelPrivate::init()
{
   Q_Q(QStdItemModel);
   QObject::connect(q, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                    q, SLOT(_q_emitItemChanged(QModelIndex,QModelIndex)));
   roleNames = QAbstractItemModelPrivate::defaultRoleNames();
}


void QStdItemModelPrivate::_q_emitItemChanged(const QModelIndex &topLeft,
                                                  const QModelIndex &bottomRight)
{
   Q_Q(QStdItemModel);
   QModelIndex parent = topLeft.parent();
   for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
       for (int column = topLeft.column(); column <= bottomRight.column(); ++column) {
           QModelIndex index = q->index(row, column, parent);
           if (QStdItem *item = itemFromIndex(index))
               emit q->itemChanged(item);
       }
   }
}

void QStdItemModelPrivate::itemChanged(QStdItem *item, const QList<int> &roles)
{
   qDebug()<< "<QStdItemModelPrivate::itemChanged(QStdItem* item, const QList<int>& roles)>";

   Q_Q(QStdItemModel);
   Q_ASSERT(item);
   if (item->d_func()->parent == nullptr)
   {
       // Header item
       int idx = columnHeaderItems.indexOf(item);
       if (idx != -1) {
           emit q->headerDataChanged(Qt::Horizontal, idx, idx);
       } else {
           idx = rowHeaderItems.indexOf(item);
           if (idx != -1)
               emit q->headerDataChanged(Qt::Vertical, idx, idx);
       }
   } else {
       // Normal item
       const QModelIndex index = q->indexFromItem(item);
       emit q->dataChanged(index, index, roles);
   }

   qDebug()<< "</QStdItemModelPrivate::itemChanged(QStdItem* item, const QList<int>& roles)>";
}

void QStdItemModelPrivate::rowsAboutToBeInserted(QStdItem *parent,
                                                     int start, int end)
{

   qDebug() << "<QStdItemModelPrivate::rowsAboutToBeInserted(QStdItem* parent, int start,int end)>";
   Q_Q(QStdItemModel);
   QModelIndex index = q->indexFromItem(parent);
   q->beginInsertRows(index, start, end);
   qDebug() << "</QStdItemModelPrivate::rowsAboutToBeInserted(QStdItem* parent, int start,int end)>";
}


void QStdItemModelPrivate::columnsAboutToBeInserted(QStdItem *parent,
                                                        int start, int end)
{
   qDebug() << "<QStdItemModelPrivate::columnsAboutToBeInserted(QStdItem* parent,int start,int end)>";
   Q_Q(QStdItemModel);
   QModelIndex index = q->indexFromItem(parent);
   q->beginInsertColumns(index, start, end);
   qDebug() << "/<QStdItemModelPrivate::columnsAboutToBeInserted(QStdItem* parent,int start,int end)>";
}


void QStdItemModelPrivate::rowsAboutToBeRemoved(QStdItem *parent,
                                                    int start, int end)
{
   qDebug()<< "<QStdItemModelPrivate::rowsAboutToBeRemoved(QStdItem* parent, int start,int end)>";

   Q_Q(QStdItemModel);
   QModelIndex index = q->indexFromItem(parent);
   q->beginRemoveRows(index, start, end);
   qDebug()<< "</QStdItemModelPrivate::rowsAboutToBeRemoved(QStdItem* parent, int start,int end)>";
}


void QStdItemModelPrivate::columnsAboutToBeRemoved(QStdItem *parent,
                                                       int start, int end)
{

   qDebug()<< "<QStdItemModelPrivate::columnsAboutToBeRemoved(QStdItem* parent, int start,int end)>";

   Q_Q(QStdItemModel);
   QModelIndex index = q->indexFromItem(parent);
   q->beginRemoveColumns(index, start, end);
   qDebug()<< "</QStdItemModelPrivate::columnsAboutToBeRemoved(QStdItem* parent, int start,int end)>";
}


void QStdItemModelPrivate::rowsInserted(QStdItem *parent,
                                            int row, int count)
{
   scope_tagger t{ "QStdItemModelPrivate::rowsInserted(QStdItem* parent, int start,int end)"};

   Q_Q(QStdItemModel);
   if (parent == root.data())
       rowHeaderItems.insert(row, count, nullptr);
   q->endInsertRows();


}


void QStdItemModelPrivate::columnsInserted(QStdItem *parent,
                                               int column, int count)
{
   scope_tagger t{ "QStdItemModelPrivate::columnsInserted(QStdItem* parent, int start,int end)"};

   Q_Q(QStdItemModel);
   if (parent == root.data())
       columnHeaderItems.insert(column, count, nullptr);
   q->endInsertColumns();

}


void QStdItemModelPrivate::rowsRemoved(QStdItem *parent,
                                           int row, int count)
{
   scope_tagger t{ "QStdItemModelPrivate::rowsRemoved(QStdItem* parent, int start,int end)"};

   Q_Q(QStdItemModel);
   if (parent == root.data())
   {
       for (int i = row; i < row + count; ++i)
       {
           QStdItem *oldItem = rowHeaderItems.at(i);
           if (oldItem)
               oldItem->d_func()->setModel(nullptr);
           delete oldItem;
       }
       rowHeaderItems.remove(row, count);
   }
   q->endRemoveRows();

}


void QStdItemModelPrivate::columnsRemoved(QStdItem *parent,
                                              int column, int count)
{
  scope_tagger t{ "QStdItemModelPrivate::columnsRemoved(QStdItem* parent, int start,int end)"};

   Q_Q(QStdItemModel);
   if (parent == root.data())
   {
       for (int i = column; i < column + count; ++i)
       {
           QStdItem *oldItem = columnHeaderItems.at(i);
           if (oldItem)
               oldItem->d_func()->setModel(nullptr);
           delete oldItem;
       }
       columnHeaderItems.remove(column, count);
   }
   q->endRemoveColumns();


}



/* \internal
    Used by QStdItemModel::dropMimeData
    stream out an item and his children
 */
void QStdItemModelPrivate::decodeDataRecursive(QDataStream &stream, QStdItem *item)
{
   scope_tagger t {"QStdItemModelPrivate::decodeDataRecursive"};

    int colCount, childCount;
    stream >> *item;
    stream >> colCount >> childCount;
    item->setColumnCount(colCount);

    int childPos = childCount;

    while(childPos > 0)
    {
        childPos--;
        QStdItem *child = createItem();
        child->setModel(q_func() )             ;

        decodeDataRecursive(stream, child);
        item->setChild( childPos / colCount, childPos % colCount, child);
    }

}