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
#include "qstditemmodel.h"
//#include "treemodel.h"
//  #include <QUndoView>
#include <QFile>

#include "scope_tagger.h"

#include "aqp.hpp"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);


     model = new QStdItemModel(this);

     undo_view->setStack(reinterpret_cast<QUndoStack*>(model->undo_stack()) );
      addDockWidget(Qt::RightDockWidgetArea,dock);

    view->setModel(model);
    view->setDefaultDropAction(Qt::MoveAction);
    view->setDragEnabled(true);
  //  view->setDragDropMode(QAbstractItemView::DragDrop);
      view->setDragDropMode(QAbstractItemView::InternalMove);

    for (int column = 0; column < model->columnCount(); ++column)
        view->resizeColumnToContents(column);

    connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::updateActions);

    connect(actionsMenu, &QMenu::aboutToShow, this, &MainWindow::updateActions);
    connect(insertRowAction, &QAction::triggered, this, &MainWindow::insertRow);
    connect(insertColumnAction, &QAction::triggered, this, &MainWindow::insertColumn);
    connect(removeRowAction, &QAction::triggered, this, &MainWindow::removeRow);
    connect(removeColumnAction, &QAction::triggered, this, &MainWindow::removeColumn);
    connect(insertChildAction, &QAction::triggered, this, &MainWindow::insertChild);



    connect(undo_action,&QAction::triggered,this,&MainWindow::undo);
    connect(redo_action,&QAction::triggered,this,&MainWindow::redo);


    connect(openAction,&QAction::triggered, this,&MainWindow::fileOpen);
    connect(newAction,&QAction::triggered, this,&MainWindow::fileNew);
    connect(saveAction,&QAction::triggered, this,&MainWindow::fileSave);
    connect(saveAsAction,&QAction::triggered, this,&MainWindow::fileSaveAs);

    connect(cutItemAction,&QAction::triggered,this,&MainWindow::cut);
    connect(pasteItemAction,&QAction::triggered,this,&MainWindow::paste);

    updateActions();
}

void MainWindow::cut()
{
    const QModelIndex index = view->selectionModel()->currentIndex();
    QStdItemModel *model = static_cast<QStdItemModel*>(view->model());


   view->setCurrentIndex( model->cut(index) );
}

void MainWindow::paste()
{

}

void MainWindow::setCurrentIndex(const QModelIndex &index)
{
    if (index.isValid()) {
        view->scrollTo(index);
        view->setCurrentIndex(index);
    }
}


bool MainWindow::fileSave()
{


    bool saved = false;
    if (model->filename().isEmpty())
    {    saved = fileSaveAs();
    }
    else
    {
        try
        {
            model->save();
            setWindowModified(false);
            setWindowTitle(tr("%1 - %2[*]")
                    .arg(QApplication::applicationName())
                    .arg(QFileInfo(model->filename()).fileName()));

            statusBar()->showMessage(tr("Saved %1")
                    .arg(model->filename()), 10000);
            saved = true;
        } catch (AQP::Error &error)
        {
            AQP::warning(this, tr("Error"),
                    tr("Failed to save %1: %2").arg(model->filename())
                    .arg(QString::fromUtf8(error.what())));
        }
    }
    updateActions();
    return saved;
}


bool MainWindow::fileSaveAs()
{
    QString filename = model->filename();
    QString dir = filename.isEmpty() ? "."
                                     : QFileInfo(filename).path();
    filename = QFileDialog::getSaveFileName(this,
            tr("%1 - Save As").arg(QApplication::applicationName()),
            dir,
            tr("%1 (*.debolon)").arg(QApplication::applicationName()));

    if (filename.isEmpty())
    {    return false;}

    if (!filename.toLower().endsWith(".debolon"))
    {    filename += ".debolon";}

    model->setFilename(filename);

    return fileSave();
}


bool MainWindow::okToClearData()
{
    if (isWindowModified())
        return AQP::okToClearData(&MainWindow::fileSave, this,
                tr("Unsaved changes"), tr("Save unsaved changes?"));
    return true;
}

void MainWindow::fileNew()
{
    if (!okToClearData())
    {return;}

    model->clear();
    model->setFilename(QString());

    setWindowModified(false);

    setWindowTitle(tr("%1 - Unnamed[*]")
            .arg(QApplication::applicationName()));

    updateActions();
}




void MainWindow::load(const QString &_filename,
                      const Path& path)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    try
    {
        model->loadFromFile(_filename);
        if (!path.isEmpty())
        {

            setCurrentIndex(model->pathToIndex(path));

        }

        for (int column = 0; column < model->columnCount(); ++column)
           {view->resizeColumnToContents(column);}

        setWindowModified(false);

        setWindowTitle(tr("%1 - %2[*]")
                .arg(QApplication::applicationName())
                .arg(QFileInfo(_filename).fileName()));

        statusBar()->showMessage(tr("Loaded %1").arg(_filename),
                                 10000);
    } catch (AQP::Error &error)
    {
        AQP::warning(this, tr("Error"), tr("Failed to load %1: %2")
                .arg(_filename).arg(QString::fromUtf8(error.what())));
    }

    updateActions();

    view->setFocus();
    QApplication::restoreOverrideCursor();
}


void MainWindow::fileOpen()
{
    if (!okToClearData())
    {return;}

    QString filename = model->filename();

    QString dir(filename.isEmpty() ? QString(".")
                : QFileInfo(filename).canonicalPath());

    filename = QFileDialog::getOpenFileName(this,
            tr("%1 - Open").arg(QApplication::applicationName()),
            dir, tr("production schedules (*.debolon)"));

    if (!filename.isEmpty())
        load(filename);
}

void MainWindow::undo()
{
    QStdItemModel *model = static_cast<QStdItemModel*>(view->model());

    model->undo_stack()->undo();
    updateActions();
}

void MainWindow::redo()
{
    QStdItemModel *model = static_cast<QStdItemModel*>(view->model());

    model->undo_stack()->redo();
    updateActions();

}

void MainWindow::insertChild()
{
    const QModelIndex index = view->selectionModel()->currentIndex();
    QStdItemModel *model = static_cast<QStdItemModel*>(view->model());


    scope_tagger t{ "MainWindow::insertChild"};

    QString text{"insertChildAction"};
    model->undo_stack()->beginMacro("MainWindow::"+text);

    if (model->columnCount(index) == 0)
    {
        if (! (model->insertColumn(0, index)) )
        {
            model->undo_stack()->endMacro();
            return;
        }
    }



    if(!(model->insertRow(0, index) ) )
    {       model->undo_stack()->endMacro();
        return;
    }

    for (int column = 0; column < model->columnCount(index); ++column)
    {
        const QModelIndex child = model->index(0, column, index);
        model->setData(child, QVariant(tr("[No data]")), Qt::EditRole);

        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, QVariant(tr("[No header]")), Qt::EditRole);
    }

    view->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                            QItemSelectionModel::ClearAndSelect);


    model->undo_stack()->endMacro();
      updateActions();

}

bool MainWindow::insertColumn()
{
    scope_tagger t{ "MainWindow::insertColumn"};

      QStdItemModel *model = static_cast<QStdItemModel*>(view->model());
    int column = view->selectionModel()->currentIndex().column();

    model->undo_stack()->beginMacro("MainWindow::insertColumn");

    // Insert a column in the parent item.
    bool changed = model->insertColumn(column + 1);

    if (changed)
        model->setHeaderData(column + 1, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);

    model->undo_stack()->endMacro();

    updateActions();

    return changed;
}

void MainWindow::insertRow()
{
   scope_tagger t{"MainWindow::insertRow"};
   on_scope_exit tt( [this](){ model->undo_stack()->beginMacro("MainWindow::insertRow");
                                qDebug()<< "<begin Macro MainWindow::insertRow>";},
   [this](){  model->undo_stack()->endMacro();
                qDebug().noquote() << "</begin Macro MainWindow::insertRow>"; } );

    const QModelIndex index = view->selectionModel()->currentIndex();
    QStdItemModel *model = static_cast<QStdItemModel*>(view->model());

    if(index.row()+1 >= model->rowCount())
    {
        return;
    }

    if (!model->insertRow(index.row()+1, index.parent()))
    {
        return;
    }

    updateActions();

    for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        const QModelIndex child = model->index(index.row() + 1, column, index.parent());
        model->setData(child, QVariant(tr("[No data]")), Qt::EditRole);
    }


}

bool MainWindow::removeColumn()
{
    scope_tagger t{ "MainWindow::removeColumn"};

        QStdItemModel *model = static_cast<QStdItemModel*>(view->model());
    const int column = view->selectionModel()->currentIndex().column();

    model->undo_stack()->beginMacro("MainWindow::removeColumn");

    // Insert columns in each child of the parent item.
    const bool changed = model->removeColumn(column);
    model->undo_stack()->endMacro();

    if (changed)
        updateActions();


    return changed;
}

void MainWindow::removeRow()
{
   scope_tagger t{ "MainWindow::removeRow"};

    const QModelIndex index = view->selectionModel()->currentIndex();
        QStdItemModel *model = static_cast<QStdItemModel*>(view->model());

        model->undo_stack()->beginMacro("MainWindow::removeRow");

    if (model->removeRow(index.row(), index.parent()))
        updateActions();

    model->undo_stack()->endMacro();


}

void MainWindow::update_undo()
{
    //   QStdItemModel *model = static_cast<QStdItemModel*>(view->model());

    auto stack{model->undo_stack()};

    if(stack->canRedo())
    {
        redo_action->setEnabled(true);
            redo_action->setText("redo: "+ stack->redoText());
    }else
    {
           redo_action->setEnabled(false);
           redo_action->setText("redo");
    }

    if(stack->canUndo())
    {
        undo_action->setEnabled(true);
        undo_action->setText("undo: " + stack->undoText() );
    }
    else
    {
           undo_action->setEnabled(false);
           undo_action->setText("undo");
    }
}

void MainWindow::updateActions()
{
    update_undo();


    const bool hasSelection = !view->selectionModel()->selection().isEmpty();
    removeRowAction->setEnabled(hasSelection);
    removeColumnAction->setEnabled(hasSelection);

    const bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    insertRowAction->setEnabled(hasCurrent);
    insertColumnAction->setEnabled(hasCurrent);

    if (hasCurrent)
    {
        view->closePersistentEditor(view->selectionModel()->currentIndex());

        const int row = view->selectionModel()->currentIndex().row();
        const int column = view->selectionModel()->currentIndex().column();

        if (view->selectionModel()->currentIndex().parent().isValid())
        {    statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        }
        else
        {
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
        }
    }


    saveAction->setEnabled(isWindowModified());
    int rows = model->rowCount();
    saveAsAction->setEnabled(isWindowModified() || rows);



 /*   foreach (QAction *action, QList<QAction*>() << editDeleteAction
            << editMoveUpAction << editMoveDownAction << editCutAction
            << editPromoteAction << editDemoteAction)

        action->setEnabled(enable);
        */

    // editPasteAction->setEnabled(model->hasCutItem());

}
