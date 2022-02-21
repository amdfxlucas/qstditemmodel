/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include <QList>

class TreeModel;

class TreeItem
{
public:
    virtual int  type() const; // von QStandardItem
   // virtual void read();
    // virtual void write();

    explicit TreeItem(const QList<QVariant> &data, TreeItem *parent = nullptr);
    virtual ~TreeItem();

    TreeItem *child(int number); // heist childAt() bei timelog1

    int childCount() const; // heisst 'rowCount()' bei QStandardItem
    int columnCount() const;

    virtual QVariant data(int column) const;

    virtual bool insertChildren(int position, int count, int columns);

    bool insertColumns(int position, int columns);

    TreeItem *parent(); // returns this items's parent

    void set_parent(TreeItem* new_parent);// make this item a child of new_parent

    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);

    QModelIndex index() const;
    TreeModel* model() const;

    // gibt aus, das wievielte Kind seines parents dieses item ist
    // bei QStandardItem heisst diese methode 'row()'
    int childNumber() const;

    bool setData(int column, const QVariant &value);

    int row_of_child(TreeItem *child) const
        { return childItems.indexOf(child); }

    bool has_any_children() const { return !childItems.isEmpty(); }

    QList<TreeItem*> children() const { return childItems; }

// heisst 'setChild()' bei QStandardItem
    void insert_child(int row, TreeItem *item)
        { item->set_parent( this); childItems.insert(row, item); }

    void add_child(TreeItem *item)
        { item->set_parent( this); childItems << item; }


    void swap_children(int oldRow, int newRow)
        { childItems.swapItemsAt(oldRow, newRow); }

    void add_sibling(TreeItem* item)
    {
        if(parent() )
        {parent()->add_child(item);}
        else
        {
        qDebug() <<"root_item cannot have siblings !"    ;

        }
    }

    // heisst 'takeRow()' bei QStandardItem
    TreeItem* take_child(int row);

private:
    QList<TreeItem *> childItems;
    QList<QVariant> itemData;
    TreeItem *parentItem;
};
//! [0]

#endif // TREEITEM_H