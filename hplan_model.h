#ifndef HPLAN_MODEL_H
#define HPLAN_MODEL_H
#include <QStandardItemModel>

class hplan_model
:public QStandardItemModel
{
    //Q_OBJECT
public:
    hplan_model(QObject* parent=nullptr);

    bool is_workday(const QModelIndex& index) const;



    // QAbstractItemModel interface
public:
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
};

#endif // HPLAN_MODEL_H
