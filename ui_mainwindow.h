/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.1.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeView>
#include <QListWidget>
#include <QDockWidget>

#include "myqtreeview.h"
#include <QTreeWidget>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QListView>

// #include <QTableView>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *exitAction;
    QAction *insertRowAction;
    QAction *removeRowAction;
    QAction *insertColumnAction;
    QAction *removeColumnAction;
    QAction *insertChildAction;
    QWidget *centralwidget;
     QDockWidget *dock;
    QVBoxLayout *vboxLayout;

    //QTableView* view;
    //QListView *view ;
    MyQTreeView *view;
    //QTreeView* view;

    QMenuBar *menubar;
    QMenu *fileMenu;
    QMenu *actionsMenu;
    QStatusBar *statusbar;

    void create_dock_widgets( QMainWindow *MainWindow )
    {
        dock = new QDockWidget("Customers", MainWindow);
         dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  /*     QListWidget*  customerList = new QListWidget(dock);
         customerList->addItems(QStringList()
                 << "John Doe, Harmony Enterprises, 12 Lakeside, Ambleton"
                 << "Jane Doe, Memorabilia, 23 Watersedge, Beaton"
                 << "Tammy Shea, Tiblanka, 38 Sea Views, Carlton"
                 << "Tim Sheen, Caraba Gifts, 48 Ocean Way, Deal"
                 << "Sol Harvey, Chicos Coffee, 53 New Springs, Eccleston"
                 << "Sally Hobart, Tiroli Tea, 67 Long River, Fedula");


         dock->setWidget(customerList);
         MainWindow->addDockWidget(Qt::RightDockWidgetArea, dock); */
    }

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(573, 468);
        exitAction = new QAction(MainWindow);
        exitAction->setObjectName(QString::fromUtf8("exitAction"));

        insertRowAction = new QAction(MainWindow);
        insertRowAction->setObjectName(QString::fromUtf8("insertRowAction"));

        removeRowAction = new QAction(MainWindow);
        removeRowAction->setObjectName(QString::fromUtf8("removeRowAction"));

        insertColumnAction = new QAction(MainWindow);
        insertColumnAction->setObjectName(QString::fromUtf8("insertColumnAction"));

        removeColumnAction = new QAction(MainWindow);
        removeColumnAction->setObjectName(QString::fromUtf8("removeColumnAction"));

        insertChildAction = new QAction(MainWindow);
        insertChildAction->setObjectName(QString::fromUtf8("insertChildAction"));

        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));

        vboxLayout = new QVBoxLayout(centralwidget);
        vboxLayout->setSpacing(0);
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));

        //view = new QTreeView(centralwidget);
        view = new MyQTreeView(centralwidget);

        // view = new QListView(centralwidget);
        // in einem ListView sieht man nur die TopLevel items als rows,
        // aber keines der childern der items
        // (bzw nur die unmittelbaren kinder des root_items )
        // und auch nur deren 0-te Column

       // view = new QTableView(centralwidget);
        // bei einem TableView sieht man auch nur die unmittelbaren children des root_items als rows,
        // dafür aber alle columns


        view->setObjectName(QString::fromUtf8("view"));
        view->setAlternatingRowColors(true);
        view->setSelectionBehavior(QAbstractItemView::SelectItems);
        view->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

        // -----only possible with TreeView --------
        view->setAnimated(false);
        view->setAllColumnsShowFocus(true);

        vboxLayout->addWidget(view);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 573, 31));
        fileMenu = new QMenu(menubar);
        fileMenu->setObjectName(QString::fromUtf8("fileMenu"));
        actionsMenu = new QMenu(menubar);
        actionsMenu->setObjectName(QString::fromUtf8("actionsMenu"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(fileMenu->menuAction());
        menubar->addAction(actionsMenu->menuAction());
        fileMenu->addAction(exitAction);
        actionsMenu->addAction(insertRowAction);
        actionsMenu->addAction(insertColumnAction);
        actionsMenu->addSeparator();
        actionsMenu->addAction(removeRowAction);
        actionsMenu->addAction(removeColumnAction);
        actionsMenu->addSeparator();
        actionsMenu->addAction(insertChildAction);

        create_dock_widgets(MainWindow);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Editable Tree Model", nullptr));
        exitAction->setText(QCoreApplication::translate("MainWindow", "E&xit", nullptr));
#if QT_CONFIG(shortcut)
        exitAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        insertRowAction->setText(QCoreApplication::translate("MainWindow", "Insert Row", nullptr));
#if QT_CONFIG(shortcut)
        insertRowAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+I, R", nullptr));
#endif // QT_CONFIG(shortcut)
        removeRowAction->setText(QCoreApplication::translate("MainWindow", "Remove Row", nullptr));
#if QT_CONFIG(shortcut)
        removeRowAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+R, R", nullptr));
#endif // QT_CONFIG(shortcut)
        insertColumnAction->setText(QCoreApplication::translate("MainWindow", "Insert Column", nullptr));
#if QT_CONFIG(shortcut)
        insertColumnAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+I, C", nullptr));
#endif // QT_CONFIG(shortcut)
        removeColumnAction->setText(QCoreApplication::translate("MainWindow", "Remove Column", nullptr));
#if QT_CONFIG(shortcut)
        removeColumnAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+R, C", nullptr));
#endif // QT_CONFIG(shortcut)
        insertChildAction->setText(QCoreApplication::translate("MainWindow", "Insert Child", nullptr));
#if QT_CONFIG(shortcut)
        insertChildAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        fileMenu->setTitle(QCoreApplication::translate("MainWindow", "&File", nullptr));
        actionsMenu->setTitle(QCoreApplication::translate("MainWindow", "&Actions", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
