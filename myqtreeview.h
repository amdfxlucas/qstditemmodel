#ifndef MYQTREEVIEW_H
#define MYQTREEVIEW_H
#include <QTreeView>
#include "path.h"

class  MyQTreeView
        : public QTreeView
{
public:
    MyQTreeView(QWidget* parent = nullptr);

    void setExpanded(const Path& path,bool enable);

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
