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

#ifndef QSTDITEMMODEL_P_H
#define QSTDITEMMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

 #include "qstditemmodel.h"
#include "QtGui/private/qtguiglobal_p.h"
#include "QtCore/private/qabstractitemmodel_p.h"
//#include "qabstractitemmodel_p.h"

#include <QtCore/qlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qstack.h>
#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtCore/QObject>

QT_REQUIRE_CONFIG(standarditemmodel);

QT_BEGIN_NAMESPACE


#ifndef QT_NO_DATASTREAM

inline QDataStream &operator>>(QDataStream &in, QStdItemData &data)
{
    in >> data.role;
    in >> data.value;
    return in;
}

inline QDataStream &operator<<(QDataStream &out, const QStdItemData &data)
{
    out << data.role;
    out << data.value;
    return out;
}

inline QDebug &operator<<(QDebug &debug, const QStdItemData &data)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << data.role
                    << " "
                    << data.value;
    return debug.space();
}

#endif // QT_NO_DATASTREAM




class TEST_LIB_EXPORT QStdItemPrivate
        : public QObject
{
Q_OBJECT
private:
  //  Q_DECLARE_PUBLIC(QStdItem)
protected:
      Q_DECLARE_PUBLIC(QStdItem)
public:

~QStdItemPrivate();
 signals:
    void free_uuid(unsigned long long uuid);

public:


    inline QStdItemPrivate()
        : model(nullptr),
          parent(nullptr),
          rows(0),
          columns(0),
          q_ptr(nullptr),
          lastKnownIndex(-1)
        {
    m_uuid= next_free_uuid++;
    }


    inline QStdItemPrivate(QStdItemModel* m)
        : model(m),
          parent(nullptr),
          rows(0),
          columns(0),
          q_ptr(nullptr),
          lastKnownIndex(-1)
        {
    m_uuid= next_free_uuid++;
    }


    Qt::ItemFlags setFlags(Qt::ItemFlags f);

    // sets the Data under the given role and returns the old value
  virtual  QVariant setData(int m_role,const QVariant& m_value);

    unsigned long long uuid()const {return m_uuid;}

    // child items are indexed in row-major fashion
    inline int childIndex(int row, int column) const
    {
        if ((row < 0) || (column < 0)
            || (row >= rowCount()) || (column >= columnCount()))
        {
            return -1;
        }
        return (row * columnCount()) + column;
    }

    inline int childIndex(const QStdItem *child) const;


    QPair<int, int> position() const;
    QStdItem* setChild(int row, int column, QStdItem *item,
                  bool emitChanged = false);
    inline int rowCount() const {
        return rows;
    }
    inline int columnCount() const {
        return columns;
    }
    void childDeleted(QStdItem *child);
    QList<QStdItem*> get_children()const{return children;}

    void setModel(QStdItemModel *mod);

    inline void setParentAndModel(
        QStdItem *par,
        QStdItemModel *mod)
    {
      // q_func()-> setModel(mod);
        setModel(mod);
        parent = par;
    }

    void changeFlags(bool enable, Qt::ItemFlags f);

    void setItemData(const QMap<int, QVariant> &roles);

    const QMap<int, QVariant> itemData() const;

    bool insertRows(int row, int count, const QList<QStdItem*> &items,bool _emit=true);

    QList<QStdItem*> removeRows(int row,int count,bool _emit = true);
    QList<QStdItem*> removeColumns(int column,int count);
    int setRowCount_impl(int m_rows);
    int setColumnCount_impl(int m_columns);

    bool insertRows(int row, const QList<QStdItem*> &items,bool _emit=true);

    bool insertColumns(int column, int count, const QList<QStdItem*> &items);

    void sortChildren(int column, Qt::SortOrder order);
     QStdItem *takeChild(int row, int column = 0);
     QList<QStdItem*> takeRow(int row);
     QList<QStdItem*> takeColumn(int column);

    bool hasChild(unsigned long long int uuid)const;
    bool hasChildren()const;

    QStdItemModel *model;
    QStdItem *parent;
    QList<QStdItemData> values;
    QList<QStdItem *> children;
    int rows;
    int columns;

    QStdItem *q_ptr;

    // every Item has a globally unique Identifier
    inline static unsigned long long next_free_uuid{0};
    unsigned long long m_uuid;

    mutable int lastKnownIndex; // this is a cached value
};







class QStdItemModelPrivate
        : public QAbstractItemModelPrivate
{
    Q_DECLARE_PUBLIC(QStdItemModel)

public:
    QStdItemModelPrivate(QStdItemModel* );
    QStdItemModelPrivate();
    ~QStdItemModelPrivate();

    void init();

    inline QStdItem *createItem() const {
        return itemPrototype ? itemPrototype->clone() : new QStdItem;
    }

    inline QStdItem *itemFromIndex(const QModelIndex &index) const {
        Q_Q(const QStdItemModel);
        if (!index.isValid())
            return root.data();
        if (index.model() != q)
            return nullptr;
        QStdItem *parent = static_cast<QStdItem*>(index.internalPointer());
        if (parent == nullptr)
            return nullptr;
        return parent->child(index.row(), index.column());
    }

    void sort(QStdItem *parent, int column, Qt::SortOrder order);
    void itemChanged(QStdItem *item, const QList<int> &roles = QList<int>());
    void rowsAboutToBeInserted(QStdItem *parent, int start, int end);
    void columnsAboutToBeInserted(QStdItem *parent, int start, int end);
    void rowsAboutToBeRemoved(QStdItem *parent, int start, int end);
    void columnsAboutToBeRemoved(QStdItem *parent, int start, int end);
    void rowsInserted(QStdItem *parent, int row, int count);
    void columnsInserted(QStdItem *parent, int column, int count);
    void rowsRemoved(QStdItem *parent, int row, int count);
    void columnsRemoved(QStdItem *parent, int column, int count);

    void _q_emitItemChanged(const QModelIndex &topLeft,
                            const QModelIndex &bottomRight);

    void decodeDataRecursive(QDataStream &stream, QStdItem *item);

    bool hasCutItem()const{return cut_item;}


    QStdItem* cut_item;

    QString m_filename;
    QList<QStdItem *> columnHeaderItems;
    QList<QStdItem *> rowHeaderItems;
    QHash<int, QByteArray> roleNames;
    QScopedPointer<QStdItem> root;
    const QStdItem *itemPrototype;
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(QStdItemModelPrivate, int, sortRole, Qt::DisplayRole)
};

QT_END_NAMESPACE

#endif // QSTDITEMMODEL_P_H
