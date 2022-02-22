#include "myqtreeview.h"

#include<QDrag>
#include<QMouseEvent>
#include <QApplication>
#include "scope_tagger.h"
#include "qstditemmodel.h"

void MyQTreeView::paintEvent(QPaintEvent *event)
{            QTreeView::paintEvent(event);

}

MyQTreeView::MyQTreeView(QWidget* parent )
    : QTreeView(parent)
{



}

void MyQTreeView::dragMoveEvent(QDragMoveEvent* e)
{scope_tagger t{"MyQTreeView::dragMoveEvent"};

    QTreeView::dragMoveEvent(e);
}

void MyQTreeView::dropEvent(QDropEvent* event)
{
    scope_tagger t{"MyQTreeView::dropEvent"};
    QTreeView::dropEvent(event);

}

void MyQTreeView::dragEnterEvent(QDragEnterEvent* event)
{
    scope_tagger t {"MyQTreeView::dragEnterEvent"};
    QTreeView::dragEnterEvent(event);
}

Qt::DropAction MyQTreeView::supportedDropActions()
{
    return Qt::MoveAction;
}

void MyQTreeView::mousePressEvent(QMouseEvent *event)
{
    scope_tagger t{"MyQTreeView::mousePressEvent"};

    if (event->button() == Qt::LeftButton)
        dragStartPosition = event->pos();

    QTreeView::mousePressEvent(event);
}

   void MyQTreeView::setExpanded(const Path& path,bool enable)
   {
       QTreeView::setExpanded( static_cast<QStdItemModel*>(model())->pathToIndex(path),enable);
   }

void MyQTreeView::mouseMoveEvent(QMouseEvent *event)
{
    scope_tagger t{"MyQTreeView::mouseMoveEvent"};
   auto _model{reinterpret_cast<QStdItemModel*>(model())};



    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - dragStartPosition).manhattanLength()
         < QApplication::startDragDistance())
    {    return;}


    event->accept();

    _model->undo_stack()->beginMacro("DragDrop");
    qDebug()<< "<begin_macro DragDrop>";

    startDrag(Qt::MoveAction);

    qDebug().noquote() << "</begin_macro DragDrop>";
    _model->undo_stack()->endMacro();
}

void MyQTreeView::dragLeaveEvent(QDragLeaveEvent *event)
{
    scope_tagger t{"MyQTreeView::dragLeaveEvent"};

    QTreeView::dragLeaveEvent(event);
}
