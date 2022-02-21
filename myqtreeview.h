#ifndef MYQTREEVIEW_H
#define MYQTREEVIEW_H
#include <QTreeView>
#include "import_export.h"


class TEST_LIB_EXPORT  MyQTreeView
        : public QTreeView
{
public:
    MyQTreeView(QWidget* parent = nullptr);


    void dragMoveEvent(QDragMoveEvent* e);
    void dropEvent(QDropEvent* event);

  //  void startDrag(Qt::DropActions supportedActions);

    void dragEnterEvent(QDragEnterEvent* event);
    Qt::DropAction supportedDropActions();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void dragLeaveEvent(QDragLeaveEvent *event);


protected:
    void paintEvent(QPaintEvent *event);
private:
    QPoint dragStartPosition;
};

#endif // MYQTREEVIEW_H
