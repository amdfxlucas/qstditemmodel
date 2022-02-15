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

#include "mainwindow.h"
#include "treemodel.h"

#include "hplan_model.h"
#include <QFile>

#include "plan_model_builder.h"

#include "hplan_item.h"

namespace{

void setPlanItemType(auto* item , PlanItemTypes type)
{
    item->setData(0,DataRoles::TypeRole,type);
}

auto makeItemOrder = [](auto* item){setPlanItemType(item,PlanItemTypes::OrderType);};


void  iterate(const QModelIndex & index, const auto * model,const auto& func)
 {
if (index.isValid())
{
    // Do action here
    // func(model->itemFromIndex(index));
    func(static_cast<QTreeWidgetItem*>(index.internalPointer()) );

}

if (!model->hasChildren(index) || (index.flags() &
Qt::ItemNeverHasChildren))
{
    return;
}

auto rows = model->rowCount(index);
for (int i = 0; i < rows; ++i)
    iterate(model->index(i, 0, index), model,func);
  }

}

void MainWindow::hide_non_workdays(bool hide,const QModelIndex& index)
{
    QVariant var{std::cref(index)};

    bool hideThisOne = hide && model->is_workday(index);
    if (index.isValid())
        view->setRowHidden(index.row(), index.parent(),
                               hideThisOne);
    if (!hideThisOne) {
        for (int row = 0; row < model->rowCount(index); ++row)
           hide_non_workdays(hide, model->index(row, 0, index));
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
    view->setAcceptDrops(true);
    view->setDragEnabled(true);
    view->setDropIndicatorShown(true);
    view->setDefaultDropAction(Qt::MoveAction);
    view->setAlternatingRowColors(true);
    view->setDragDropMode(QAbstractItemView::DragDrop);

    /*
    const QStringList headers({tr("Title"), tr("Description")});

    QFile file(":/default.txt");
    file.open(QIODevice::ReadOnly);
    TreeModel *model = new TreeModel(headers, file.readAll());
    file.close();
    */

    model = new hplan_model(this);

    plan_model_builder builder("2022");

    builder.build(*model);



    view->setModel(model);

    //view->setWindowFlag(Qt::FramelessWindowHint,true);
//view->setAttribute(Qt::WA_TranslucentBackground,true);

  //  auto opacity_effect{ new QGraphicsOpacityEffect(this)};
//    opacity_effect->setOpacity(0.5);
 //   view->setGraphicsEffect(opacity_effect );


    view->setStyleSheet
       (
           "QTreeView#view {"
            "background-color: rgba(255,255,255,100);"
           "background-image:url(:image/025altro.png);"
                "alternate-background-color: rgba(252, 249, 249, 50) ;"
           "background-position:center ;"
           "background-repeat:none;"
                "opacity: 127;"
           "}  "
       );
// alternate-background-color: rgba(252, 249, 249, 80)  rgba(219, 219, 219, 80)

    // "filter:alpha(opacity=50);"
//background-position: bottom right
    // "background-color:white;"

    //view->setStyleSheet("background:transparent");




    // ---- only possible with TreeView --------
     for (int column = 0; column < model->columnCount(); ++column)
     {view->resizeColumnToContents(column);     }

    connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::updateActions);

    connect(actionsMenu, &QMenu::aboutToShow, this, &MainWindow::updateActions);
    connect(insertRowAction, &QAction::triggered, this, &MainWindow::insertRow);
    connect(insertColumnAction, &QAction::triggered, this, &MainWindow::insertColumn);
    connect(removeRowAction, &QAction::triggered, this, &MainWindow::removeRow);
    connect(removeColumnAction, &QAction::triggered, this, &MainWindow::removeColumn);
    connect(insertChildAction, &QAction::triggered, this, &MainWindow::insertChild);

    updateActions();

   populate_dock_widget();

}


void MainWindow::customMenuRequested(QPoint pos)
{
  // auto item{proxyModel->item( list_view->indexAt(pos))};

  QModelIndex index1{view->indexAt(pos)};







       // parent_dashboard->set_current_action_target_indexes(view->selectionModel()->selectedIndexes());

    // QModelIndex index =item_list_widget->indexAt(pos);
    // lokale variable index  vlt ersetzen durch attribut current_action_index des product_selectors
    // damit der slot darauf zugreifen kann

   /* QMessageBox::information(this,"row selected",
                                tr("the following selection was added: %1").
                                arg( index.data().toString() ));
*/



//    item_list_widget_menu->popup(item_list_widget->viewport()->mapToGlobal(pos));
        // item_list_widget_menu->popup(list_view->viewport()->mapToGlobal(pos));

  //    parent_dashboard->cMenu_edit()->popup(view->viewport()->mapToGlobal(pos));
}


void MainWindow::populate_dock_widget()
{
    //-------------------------- populate the dockWidget ------------------------------------------------
    dock->setWindowTitle("current orders");
    QTreeWidget*  current_orders = new QTreeWidget(dock);
    current_orders->setHeaderHidden(true);

    current_orders->setDragDropMode(QAbstractItemView::DragDrop);
    // bei DragOnly kann man es sich nicht nocheinmal anders überlegen, wenn man das produkt einmal eingefügt hat
    // current_orders->setDragDropOverwriteMode(true);
    current_orders->setDefaultDropAction(Qt::MoveAction);
    current_orders->setDragEnabled(true);
    current_orders->viewport()->setAcceptDrops(true); // mit false kann man es sich nichtmehr anders überlegen


    auto t_model{current_orders->model()};

    QTreeWidgetItem * config00 = new QTreeWidgetItem(current_orders);
    config00->setText(0,"Konfiguration 00");
    config00->setFlags(config00->flags() & ~Qt::ItemIsDragEnabled);
    // mit  & ~Qt::ItemIsDropEnabled kann man ein produkt nichtmehr zurück tuhn, wenn man es sich anders überlegt


    QTreeWidgetItem * order00 = new QTreeWidgetItem(config00);
    order00->setText(0,"00000234156    2007-270302");
    order00->setBackground(0,QBrush(QColor(39, 241, 245, 80),Qt::SolidPattern));
    order00->setFlags(order00->flags() & ~Qt::ItemIsDropEnabled);
    config00->addChild(order00);

    QTreeWidgetItem * order01 = new QTreeWidgetItem(config00);
    order01->setText(0,"00000637156    2007-270313");
    order01->setBackground(0,QBrush(QColor(39, 241, 245, 80),Qt::SolidPattern));
    order01->setFlags(order01->flags() & ~Qt::ItemIsDropEnabled);
    config00->addChild(order01);

    QTreeWidgetItem * order02 = new QTreeWidgetItem(config00);
    order02->setText(0,"00000519256    2007-270315");
    order02->setBackground(0,QBrush(QColor(39, 241, 245, 80),Qt::SolidPattern));
    order02->setFlags(order02->flags() & ~Qt::ItemIsDropEnabled);
    config00->addChild(order02);

    // config00->parent()->indexOfChild(config00)
    auto i_conf00{t_model->index(0,0 )};
    // t_model->createIndex(config00->parent()->indexOfChild(config00),0,config00)
    iterate( i_conf00,t_model,makeItemOrder);


    current_orders->addTopLevelItem(config00);
    //........................

    QTreeWidgetItem * config01 = new QTreeWidgetItem(current_orders);
    config01->setText(0,"Konfiguration 01");
    config01->setFlags(config01->flags()& ~Qt::ItemIsDragEnabled);
    // mit  & ~Qt::ItemIsDropEnabled  kein anders überlegen mehr

    QTreeWidgetItem * order000 = new QTreeWidgetItem(config01);
    order000->setText(0,"00000234156    2007-815301");
    order000->setBackground(0,QBrush(QColor(255, 255, 0, 80) ,Qt::SolidPattern));
    config01->addChild(order000);

    QTreeWidgetItem * order001 = new QTreeWidgetItem(config01);
    order001->setText(0,"00000637156    2007-815302");
    order001->setBackground(0,QBrush(QColor(255, 255, 0, 80),Qt::SolidPattern));
    config01->addChild(order001);

    current_orders->addTopLevelItem(config01);

    //..........
    QTreeWidgetItem * config02 = new QTreeWidgetItem(current_orders);
    config02->setText(0,"Konfiguration 02");
  //  config02->setBackground(0,QBrush(Qt::red,Qt::SolidPattern));
    config02->setFlags(config02->flags() &  ~Qt::ItemIsDragEnabled);
    // mit ~Qt::ItemIsDropEnabled & kein anders überlegen mehr


    QTreeWidgetItem * order0000 = new QTreeWidgetItem(config02);
    order0000->setText(0,"00000234156    3007-384301");
    order0000->setBackground(0,QBrush(QColor(255,0,0,80),Qt::SolidPattern));
    config02->addChild(order0000);

    QTreeWidgetItem * order0001 = new QTreeWidgetItem(config02);
    order0001->setText(0,"00000637156    3057-384301");
    order0001->setBackground(0,QBrush(QColor(255,0,0,80),Qt::SolidPattern));
    config02->addChild(order0001);

    QTreeWidgetItem * order0002 = new QTreeWidgetItem(config02);
    order0002->setText(0,"00000519256    4507-384301");
    order0002->setBackground(0,QBrush(QColor(255,0,0,80),Qt::SolidPattern));
    config02->addChild(order0002);


    QTreeWidgetItem * order0003 = new QTreeWidgetItem(config02);
    order0003->setText(0,"00000519256    6057-384301");
    order0003->setBackground(0,QBrush(QColor(255,0,0,80),Qt::SolidPattern));
    config02->addChild(order0003);

    current_orders->addTopLevelItem(config02);


      dock->setWidget(current_orders);
      addDockWidget(Qt::RightDockWidgetArea, dock);
}

void MainWindow::insertChild()
{
    const QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return;
    }

    if (!model->insertRow(0, index))
        return;

    for (int column = 0; column < model->columnCount(index); ++column) {
        const QModelIndex child = model->index(0, column, index);
        model->setData(child, QVariant(tr("[No data]")), Qt::EditRole);
        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, QVariant(tr("[No header]")), Qt::EditRole);
    }

    view->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
    updateActions();
}

bool MainWindow::insertColumn()
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert a column in the parent item.
    bool changed = model->insertColumn(column + 1);
    if (changed)
        model->setHeaderData(column + 1, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);

    updateActions();

    return changed;
}

void MainWindow::insertRow()
{
    const QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    updateActions();

    for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        const QModelIndex child = model->index(index.row() + 1, column, index.parent());
        model->setData(child, QVariant(tr("[No data]")), Qt::EditRole);
    }
}

bool MainWindow::removeColumn()
{
    QAbstractItemModel *model = view->model();
    const int column = view->selectionModel()->currentIndex().column();

    // Insert columns in each child of the parent item.
    const bool changed = model->removeColumn(column);
    if (changed)
        updateActions();

    return changed;
}

void MainWindow::removeRow()
{
    const QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

void MainWindow::updateActions()
{
    const bool hasSelection = !view->selectionModel()->selection().isEmpty();
    removeRowAction->setEnabled(hasSelection);
    removeColumnAction->setEnabled(hasSelection);

    const bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    insertRowAction->setEnabled(hasCurrent);
    insertColumnAction->setEnabled(hasCurrent);

    if (hasCurrent) {
        view->closePersistentEditor(view->selectionModel()->currentIndex());

        const int row = view->selectionModel()->currentIndex().row();
        const int column = view->selectionModel()->currentIndex().column();
        if (view->selectionModel()->currentIndex().parent().isValid())
            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        else
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
    }
}
