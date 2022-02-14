#ifndef QSTDITEMCOMMANDS_H
#define QSTDITEMCOMMANDS_H

#include "qstditem.h"
#include "reference_controller.h"

/*
class  QStdItem::StdItemCmd
        :  public QUndoCommand
{public:
    StdItemCmd(QStdItem* it,
               QUndoCommand* parent= nullptr)
        : QUndoCommand(parent)
    {   _item=it;
        model= it->model();

       if(QModelIndex _ind{it->index()}; _ind.isValid())
       {
           m_indexPath = pathFromIndex(_ind );
       }

    }

    QStdItem* this_item()const;//{return model ? model->itemFromIndex(pathToIndex(m_indexPath,model) ) : _item;}
    QStdItemModel* this_model() const;//{return model;}
    QModelIndex this_index() const ;//{ return pathToIndex(m_indexPath, this_model() );}

    bool redoSuccessFlag()const{return redo_ret_code;   }
    void setSuccessFlag(bool flag){redo_ret_code=flag;}

private:



    bool redo_ret_code;
     QStdItem* _item; // this was a really bad idea ! not long and it would become a dangling pointer
    QStdItemModel* model; // this is ok, because the model outlives every of its items
  mutable  Path m_indexPath;


protected:
    void update();
};
*/


class QStdItem::StdItemCmd
        : public QUndoCommand
{
public:
    StdItemCmd(QStdItem* i,QUndoCommand* parent=nullptr);

    virtual ~StdItemCmd();

    QStdItemModel* this_model();
    QStdItem* this_item();
    QModelIndex this_index();
    Path this_path();

    reference* m_reference(){return m_ref;};
    bool redoSuccessFlag()const{return redo_ret_code;   }
    void setSuccessFlag(bool flag){redo_ret_code=flag;}
private:
    reference* m_ref;



private:



    bool redo_ret_code;

};



  class QStdItem::RemoveRowsCmd
          : public StdItemCmd
  {
  private:
      int m_row;
      int m_count{-1};
       QList<QStdItem*> m_items; // the removed items are keept safe here
          int prev_row_count;
          bool is_last_row{false};
  public:
      RemoveRowsCmd(QStdItem* i,
                    int row,
                    QUndoCommand* parent =nullptr)
          : StdItemCmd(i,parent),
            m_row(row)
      {
          m_count=1;
          prev_row_count= i->rowCount();

          if(i->rowCount()-1==row)
          {is_last_row=true;}
      }

      RemoveRowsCmd(QStdItem* i,
                    int row,
                    int count,
                    QUndoCommand* parent =nullptr)
          : StdItemCmd(i,parent),
            m_count(count),
            m_row(row)
      {
          prev_row_count= i->rowCount();

          if(i->rowCount()-1==row)
          {is_last_row=true;}
      }

      ~RemoveRowsCmd()    ;

      void redo() override;
      void undo() override;
  };


  class QStdItem::RemoveColumnsCmd
          : public StdItemCmd
  {
  private:
      bool is_last_column{false};
      int m_column;
      int m_count{-1};
      QList<QStdItem*> m_items; // the removed items are keept safe here
      bool is_valid_cmd{true};
      int prev_col_count;

  public:

      ~RemoveColumnsCmd();


      RemoveColumnsCmd(QStdItem* i,
                    int column,
                    QUndoCommand* parent =nullptr)
          : StdItemCmd(i,parent),
            m_column(column)
      {
              prev_col_count=i->columnCount();
              m_count=1;
              if(i->columnCount()-1==column){is_last_column=true;}

                if ( (m_column < 0) || ((m_column + m_count) > prev_col_count))
                {
                    is_valid_cmd=false;
                }
      }

      RemoveColumnsCmd(QStdItem* i,
                    int column,
                    int count,
                    QUndoCommand* parent =nullptr)
          : StdItemCmd(i,parent),
            m_count(count),
            m_column(column)
      {
          prev_col_count=i->columnCount();

              if(i->columnCount()-1==column)
              {is_last_column=true;}

                if ((m_count < 1) || (m_column < 0) || ((m_column + m_count) > prev_col_count))
                {is_valid_cmd=false;}
      }

      void redo() override;
      void undo() override;
  };


  class QStdItem::InsertRowCmd
          : public StdItemCmd
   {
  private:
      /*this command combines both the QStdItem::insertRows(int row,const QList<QStdItem*>& items) method
                                                which does NOT resize the column count to fit 'items'
                                          and
                                       QStdItem::insertRow(int row,const QList<QStdItem*>& items)
                                                          which does resize the column count to fit 'items'

this decision is represented by the 'resize_columns' data member
*/

      int m_count{-1};
      bool resize_columns{false};

      // ACHTUNG: DAS stimmt nicht mehr !
      // policy: resize the column-count to fit all items into a row ,or not


      int prev_col_count;
      int prev_row_count;
      int m_row; // at which row ,the insertion shall take place
      QList<QStdItem> m_items; // the items to be inserted
      bool is_valid_cmd{true};
      bool insert_past_end{false};


  public:
      // VARIANTE I Command
      InsertRowCmd(QStdItem* it,
                   int row,
                   const QList<QStdItem*>&items,
                   bool do_resize,
                   QUndoCommand* parent=nullptr)
          :StdItemCmd(it,parent) ,
            resize_columns(do_resize),
            m_row(row)

      {   prev_col_count = it->columnCount();
          prev_row_count= it->rowCount();

          // the commands stores deep copies of the inserted items
          // not pointer
          for(auto* it : items)
          {   m_items.emplace_back(*it);      }


          if(prev_col_count< m_items.count())
          {
              resize_columns=true;
          }
      }


      // VARIANTE II Command
      InsertRowCmd(QStdItem* it,
                   int row,
                   int count,
                   QUndoCommand* parent=nullptr)
          :StdItemCmd(it,parent) ,
            m_count(count),
            m_row(row)
      {
          prev_row_count= it->rowCount();
          prev_col_count=it->columnCount();

          if(prev_row_count < m_row)
          {
              insert_past_end=true;
          }
      }

      void redo() override;

      void undo() override;

  };



  class QStdItem::InsertColumnCmd
          : public StdItemCmd
  {
  private:
      int m_column;
      int m_count{-1};
      QList<QStdItem> m_items;
      int prev_row_count;
      int prev_col_count;
      bool is_valid_cmd{true};
      bool resize_rows{false};
      bool insert_past_end{false};

  public:
      // VARIANTE I
      InsertColumnCmd(QStdItem* i,
                      int col,
                      const QList<QStdItem*>& items,
                      QUndoCommand* parent=nullptr)
          : StdItemCmd(i,parent),
            m_column(col)
      {
          prev_row_count = i->rowCount();
           prev_col_count=i->columnCount();
          // the commands stores deep copies of the inserted items
          // not pointer
          for(auto* it : items)
          {   m_items.emplace_back(*it);      }


          if(! ( (m_column < 0) || (m_column >prev_col_count) ) )
          {
              is_valid_cmd=false;
          }

          if(prev_row_count < m_items.count())
          {resize_rows=true;}
      }


      // VARIANTE II
      InsertColumnCmd(QStdItem* i,
                   int column,
                   int count,
                   QUndoCommand* parent=nullptr)
          :StdItemCmd(i,parent) ,
            m_count(count),
            m_column(column)
      {
          prev_row_count= i->rowCount();
          prev_col_count=i->columnCount();



          /* diese Bedingung stammt aus:
            bool QStdItemPrivate::insertColumns(int column, int count, const QList<QStdItem*> &items)
            an die das VARIANTE I command delegiert

           */
       /*   if(! ((m_count < 1) || (m_column < 0) || (m_column > prev_col_count) || m_count == 0) )
          {
              is_valid_cmd=false;
          }
      */

          // wird nur bei VARIANTE I benötigt
        //  if(prev_row_count < m_items.count())
        //  {resize_rows=true;}

          if(prev_col_count< m_column)
          {
              insert_past_end=true;
          }
      }

      void undo() override;
      void redo() override;
  };

  class QStdItem::SetRowCountCmd
  :public StdItemCmd
  {
  private:
      int m_rows;
      void impl();
  public:
      SetRowCountCmd(QStdItem* i,int rows,QUndoCommand* parent=nullptr)
          :StdItemCmd(i,parent), m_rows(rows)
      {}

      void redo()override;
      void undo() override;

  };


  class QStdItem::SetColumnCountCmd
  :public StdItemCmd
  {
  private:
      int m_columns;
      void impl();
  public:
      SetColumnCountCmd(QStdItem* i,int columns,QUndoCommand* parent=nullptr)
          :StdItemCmd(i,parent), m_columns(columns)
      {}

      void redo()override;
      void undo() override;

  };


  class QStdItem::SetModelCmd
          : public StdItemCmd
  {
  private:
      void impl();
      QStdItemModel* m_model;
  public:
      SetModelCmd(QStdItem* i,QStdItemModel* ptr_model,QUndoCommand* parent=nullptr)
          :StdItemCmd(i,parent),
            m_model(ptr_model)
      {}
      void undo() override;
      void redo() override;

  };

  class QStdItem::SetChildCmd
          : public StdItemCmd
  {
  private:
      QStdItem* m_child;
      int m_row;
      int m_column;
      int prev_col_count;
      int prev_row_count;
      bool _emit_changed_=true;
      bool resize_columns{false};
      bool resize_rows{false};
  public:
      SetChildCmd(QStdItem* i,
                  int row,
                  int column,
                  QStdItem *item, // the child item to be inserted
                  bool emitChanged=true,
                  QUndoCommand* parent =nullptr)
          : StdItemCmd(i,parent ) ,
            m_row(row),
            m_column(column),
      m_child(item),
        _emit_changed_(emitChanged)
      {
          prev_row_count = i->rowCount();
          prev_col_count =i->columnCount();

          if(prev_row_count <= m_row)
          { resize_rows=true;}

          if(prev_col_count <= m_column)
          {
              resize_columns=true;
          }

          /* das ist die stelle in QStdItemPrivate::setChild(row,column
           * wo entschieden wird ob die childtable resized wird
             if (rows <= row){    q->setRowCount(row + 1);}

              if (columns <= column){       q->setColumnCount(column + 1);}
             */


      } // the command takes ownership of the child-item  'item'

      void redo() override;
      void undo() override;
  };

  class QStdItem::ClearDataCmd
          : public StdItemCmd
  {
  private:
      QList<QStdItemData> m_values;
      QList<int> m_roles;
  public:
      ClearDataCmd(QStdItem* i,QUndoCommand* parent=nullptr)
          : StdItemCmd(i,parent)
      {}
      void undo() override;
      void redo() override;

  };

  class QStdItem::SetDataCmd
          : public StdItemCmd
  {private:
      int m_role;
      QVariant m_value;
       std::unique_ptr<QMap<int,QVariant>> m_values;
      void impl();
      void impl_single_role();
      void impl_multi_role();

  public:
      SetDataCmd(QStdItem* i,
                 const QVariant& value,
                 int role,
                 QUndoCommand* parent=nullptr)
          : StdItemCmd(i,parent),
            m_role(role),
            m_value(value){}


      SetDataCmd(QStdItem* i,
            QMap<int,QVariant> data,
                 QUndoCommand* parent=nullptr)
          : StdItemCmd(i,parent),
            m_values(std::make_unique<QMap<int,QVariant>>(std::move(data)) )
           {}

      void undo() override;

      void redo() override;
  };



#endif // QSTDITEMCOMMANDS_H
