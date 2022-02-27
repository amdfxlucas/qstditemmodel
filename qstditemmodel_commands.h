#ifndef QSTDITEMMODEL_COMMANDS_H
#define QSTDITEMMODEL_COMMANDS_H
#include "qstditemmodel.h"



class QStdItemModel::QStdItemModelCmd
        : public QUndoCommand
{
private:
      QStdItemModel* m_model;
public:
  QStdItemModel*  model()const {return m_model;}

  QStdItemModelCmd(QStdItemModel*m,
                   QUndoCommand*p=nullptr)
      :    QUndoCommand(p),
      m_model(m){}

  virtual  QVariant returnValue()const;
};

class QStdItemModel::CutItemCmd
        :public QStdItemModelCmd
{
private:
    bool is_parent_single_column{false};
      QStdItem* m_cutItemBackup;
    Path m_ret_path; // return value of QStdItemModel::cut(..)
    Path m_path;    // path to model index where the item was removed
    bool is_index_valid{true};
public:
    virtual ~CutItemCmd();

    CutItemCmd(QStdItemModel* m,
               const QModelIndex& idx,
               QUndoCommand* p=nullptr);

    virtual void  undo()override;
    virtual void redo() override;

     virtual  QVariant returnValue()const override;

};



class QStdItemModel::PasteItemCmd
        :public QStdItemModelCmd
{private:

    bool is_single_column{false};
    Path m_ret_path; // return value of QStdItemModel::paste(..)
    Path m_path;
    // path to model index where the item will be inserted
    // if strategy is 'AsChild' , the item pointed to by m_path
    // will become the cut_item's parent
    // if it is 'AsSibling' , the item will be inserted as a sibling
    // of the item pointed to by m_path

    QStdItemModel* m_model;

public:



    PasteItemCmd(QStdItemModel* m,
               const QModelIndex& idx,
                 Behaviour b = AsSibling,
               QUndoCommand* p=nullptr);

    PasteItemCmd(QStdItemModel* m,
               const Path& idx,
                 Behaviour b= AsSibling,
               QUndoCommand* p=nullptr);

    virtual void  undo()override;
    virtual void redo() override;
private:
    Behaviour strategy;

};

class QStdItemModel::SetHHeaderItemCmd
        : public QStdItemModelCmd
{
public:
    SetHHeaderItemCmd(QStdItemModel* model,
                      int col,
                      QStdItem* item,
                      QUndoCommand* parent=nullptr)
        :QStdItemModelCmd(model,parent),
          m_item(item),
          m_column(col)
    {
        prev_col_count=model->columnCount();
        if(prev_col_count<=col){change_col_count=true;}
    }
    void undo() override;
    void redo() override;

private:
    void impl(bool un_or_redo);
    int m_column;
    int prev_col_count;
    QStdItem* m_item;
    bool change_col_count{false};

};


class QStdItemModel::SetVHeaderItemCmd
        : public QStdItemModelCmd
{
public:
    SetVHeaderItemCmd(QStdItemModel* model,
                      int row,
                      QStdItem* item,
                      QUndoCommand* parent=nullptr)
        :QStdItemModelCmd(model,parent),
          m_item(item),
          m_row(row)
    {
        prev_row_count=model->rowCount();
        if(prev_row_count<=row){change_row_count=true;}
    }
    void undo() override;
    void redo() override;

private:
    void impl(bool un_or_redo);
    int m_row;
    int prev_row_count;
    QStdItem* m_item;
    bool change_row_count{false};

};

#endif // QSTDITEMMODEL_COMMANDS_H
