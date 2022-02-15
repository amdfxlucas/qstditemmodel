#ifndef MYQTREEVIEW_H
#define MYQTREEVIEW_H
#include <QTreeView>

class MyQTreeView : public QTreeView
{
public:
    MyQTreeView(QWidget* parent = nullptr);



    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // MYQTREEVIEW_H
