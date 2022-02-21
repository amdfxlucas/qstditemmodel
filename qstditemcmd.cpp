#include "qstditemmodel.h"
#include "qstditemmodel_p.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qpair.h>
#include <QtCore/qvariant.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qbitarray.h>
#include <QtCore/qmimedata.h>
#include <QtCore/qiodevice.h>
#include <QtCore/private/qduplicatetracker_p.h>
#include"scope_tagger.h"


#include <qdebug.h>
#include <algorithm>

#include "qstditemcommands.h"


Path QStdItem::StdItemCmd::this_path()const
{
    return m_ref->m_path;
}

QStdItem::StdItemCmd::~StdItemCmd()
{
    // request the controller to delete our reference
    // its no longer needed
    m_reference()->invalidReference(m_reference());
}

constexpr UndoStack* QStdItem::StdItemCmd::get_stack()const
{
    UndoStack* ptr_stack{nullptr};
    const auto ptr_model{this_model()};

    //  && reinterpret_cast<QStdItemModelPrivate*>(ptr_model->d_ptr.get())->root.data()!=this_item()

    if(ptr_model
            )
    {

        auto ptr{ptr_model->m_stack};
        if(ptr)
        {
        ptr_stack=ptr;
        }
    }
    return ptr_stack;
}

QStdItem::StdItemCmd::StdItemCmd(QStdItem* it,QUndoCommand* parent)
    : QUndoCommand(parent)
{
    m_ref = reference_controller::get_instance()->new_reference(it,this);
}


constexpr QStdItem* QStdItem::StdItemCmd::this_item()const
{
    return m_ref->m_item;
}

QModelIndex QStdItem::StdItemCmd::this_index()const
{
    return m_ref->m_index;
}

constexpr QStdItemModel* QStdItem::StdItemCmd::this_model()const
{
    return m_ref->m_model;
}





void QStdItem::SetModelCmd::redo()
{
    scope_tagger t{"QStdItem::SetModelCmd::redo"};

   impl();
}

void QStdItem::SetModelCmd::impl()
{
    auto item{this_item()};

    QStdItemPrivate* const d= item->d_func();

    // UndoStackLock lock{this_model() ? this_model()->undo_stack() : nullptr}; // RAII class

    auto old_model{d->model};


    d->setModel(m_model);

    m_reference()->setModel(m_model)  ;// löst ein signal aus , was den reference_controller benachrichtigt,
                   // damit er alle anderen commands mit der selben item uuid updaten kann

    m_model=old_model;

}

void QStdItem::SetModelCmd::undo()
{
    scope_tagger t{"QStdItem::SetModelCmd::undo"};
    impl();
}

void QStdItem::setModel(QStdItemModel* m)
{
    scope_tagger t{"QStdItem::setModel"};

    if(model())
    {
        model()->undo_stack()->push(new SetModelCmd(this,m) );
    }else{
        std::unique_ptr<SetModelCmd> tmp{ std::make_unique<SetModelCmd>(this,m) };
        tmp->redo();
    }
}

QStdItem::RemoveColumnsCmd::  ~RemoveColumnsCmd()
{

    auto item{this_item()};

    QStdItemPrivate* const d= item->d_func();

   UndoStackLock lock{this_model() ?  this_model()->undo_stack() : nullptr}; // RAII class

    // free the removedItems the Command might still have
   // ownership of
   while(!m_items.isEmpty())
   {
       auto item{ m_items.takeLast()};
       if(!item){break;}
     //  item->d_func()->setModel(nullptr);
       item->setModel(nullptr);

       delete item;
   }
}


QStdItem::RemoveRowsCmd::~RemoveRowsCmd()
{


    auto item{this_item()};

    QStdItemPrivate* const d= item->d_func();

   UndoStackLock lock{this_model() ?  this_model()->undo_stack() : nullptr}; // RAII class

    // free the removedItems the Command might still have
   // ownership of
   while(!m_items.isEmpty())
   {
       auto item{ m_items.takeLast()};
       if(!item){break;}
       //item->d_func()->setModel(nullptr);
       item->setModel(nullptr);

       delete item;
   }
}


/*

   QModelIndex QStdItem::StdItemCmd::this_index() const

   {  m_indexPath = pathFromIndex(_item->index() );
      return pathToIndex(m_indexPath, this_model() );
   }
*/


   /* ALTE zu 90% korrekte variante
   QStdItem* QStdItem::StdItemCmd::this_item()const
   {
      auto idx{_item->index()};
        m_indexPath = pathFromIndex(idx );
      //return model ? model->itemFromIndex(pathToIndex(m_indexPath,model) ) : _item;

        auto index{pathToIndex(m_indexPath,model)};
        return model ? ( model->itemFromIndex(index ) ? model->itemFromIndex(index ) : _item ): _item;
   }*/

/*
   QStdItem* QStdItem::StdItemCmd::this_item()const
   {
   // if this command has a logical path to the item,
       // it takes preceedence over the stored pointer to the item

       QModelIndex idx;

       if(model )
       {
           if(!m_indexPath.isEmpty())
           {
               idx=pathToIndex(m_indexPath,model);
           }
           else
           {
            idx= _item->index();
           }
       }else
       {
           idx=_item->index();
       }

      //auto idx{ model&& !m_indexPath.isEmpty() ? pathToIndex(m_indexPath,model): _item->index()};

       if(idx.isValid())
       {m_indexPath = pathFromIndex(idx );
       }


      //return model ? model->itemFromIndex(pathToIndex(m_indexPath,model) ) : _item;

        auto index{pathToIndex(m_indexPath,model)};

        return model ? ( model->itemFromIndex(index ) ? model->itemFromIndex(index ) : _item ): _item;
   }


      void QStdItem::StdItemCmd::update()
      {

      }


   QStdItemModel* QStdItem::StdItemCmd::this_model() const{return model;}

*/


   void QStdItem::RemoveRowsCmd::redo()
   {
      scope_tagger t{ "QStdItem::RemoveRowsCmd::redo"}   ;

      auto item{this_item()};

      QStdItemPrivate* const d= item->d_func();

     UndoStackLock lock{this_model() ? this_model()->undo_stack() : nullptr};


   //   if(m_count!=-1)
   //   {



         if ((m_count < 1) || (m_row < 0) || ((m_row + m_count) > item->rowCount()))
             return;

         if (d->model)
             d->model->d_func()->rowsAboutToBeRemoved(item, m_row, m_row + m_count - 1);

         int i = d->childIndex(m_row, 0);
         int n = m_count * d->columnCount();

         for (int j = i; j < n+i; ++j)
         {
             QStdItem *oldItem = d->children.at(j);


             m_items.append(oldItem);
         }

         d->children.remove(qMax(i, 0), n);
         d->rows -= m_count;

         if (d->model)
             d->model->d_func()->rowsRemoved(item, m_row, m_count);


   //   }else   {     }


   }

   void QStdItem::RemoveRowsCmd::undo()
   {
      scope_tagger t{ "QStdItem::RemoveRowsCmd::undo"};

      auto item{this_item()};

      QStdItemPrivate* const d= item->d_func();

     UndoStackLock lock{this_model() ? this_model()->undo_stack() : nullptr };

   if(!is_last_row)
   {
     if ((m_count < 1) || (m_row < 0) || ((m_row + m_count) > prev_row_count))
         return;

     if (d->model)
     {//    d->model->d_func()->rowsAboutToBeInserted(item, m_row-1, m_row + m_count - 1);
         // laufzeitfehler, wenn m_row==0
         d->model->d_func()->rowsAboutToBeInserted(item, m_row>0 ? m_row-1 :0, m_row + m_count - 1);
     }

     int i = d->childIndex(m_row, 0);
     int n = m_count * d->columnCount();

     for (int j = i; j < n+i; ++j)
     {
        d->children.insert(j ,m_items.takeFirst() );


     }

     //d->children.remove(qMax(i, 0), n);

     d->rows += m_count;

      if (d->model)
      {//      d->model->d_func()->rowsInserted(item, m_row-1, m_count);
          // laufzeitfehler, bei m_rows==0
               d->model->d_func()->rowsInserted(item, m_row>0 ? m_row-1 :0 , m_count);
      }

   }
   else
   {

       if ((m_count < 1) || (m_row < 0) || ((m_row + m_count) > prev_row_count))
           return;

     /*    das schmeisst runtime error, wenn m_row ==0
      *   if (d->model)
           d->model->d_func()->rowsAboutToBeInserted(item, m_row-1, m_row + m_count - 1); */

       if (d->model)
        {     d->model->d_func()->rowsAboutToBeInserted(item, m_row, m_row + m_count - 1);
       }


       //int i = d->childIndex(m_row-1, 0);
       int n = m_count * d->columnCount();

       int i{(prev_row_count-1)*d->columnCount() };

       for (int j = i; j < n+i; ++j)
       {
          d->children.insert(j ,m_items.takeFirst() );


       }

       //d->children.remove(qMax(i, 0), n);

       d->rows += m_count;

        if (d->model)
        {    d->model->d_func()->rowsInserted(item, (m_row>1) ? m_row-1 : 0, m_count);
        }



   }

   }


   void QStdItem::RemoveColumnsCmd::redo()
   {
       scope_tagger t{"QStdItem::RemoveColumnsCmd::redo"};

      auto item{this_item()};

      QStdItemPrivate* const d= item->d_func();

     UndoStackLock lock{this_model() ?  this_model()->undo_stack() : nullptr}; // RAII class


    // if(m_count!=-1)   {

     if(!is_valid_cmd){     return;}

     if (d->model)
         d->model->d_func()->columnsAboutToBeRemoved(item, m_column, m_column + m_count - 1);


     // traverse all 'rowCount()' rows from bottom to top,
     // so that we can safely execute the 'REMOVE' statement
     // without invalidating the successive calls to 'GET_INDEX'
     for (int row = d->rowCount() - 1; row >= 0; --row)
     {
         int i = d->childIndex(row, m_column); // GET_INDEX

         // incrementing the linear row-major index 'j'
         // means traversing the current row 'row' to the right
         // for (int j=i; j<i+m_count; ++j)
         for( int j{i+m_count-1}; j>=i ; --j )  // right to left traversal of the row 'row'
         {
             QStdItem *oldItem = d->children.at(j);

            // if (oldItem){oldItem->d_func()->setModel(nullptr);}

             //delete oldItem;
           //  m_items.append(oldItem);
             m_items.prepend(oldItem);
             // the Command takes ownership of the removedItems

         }
         d->children.remove(i, m_count); // REMOVE
     }
     d->columns -= m_count;

     if (d->model)
         d->model->d_func()->columnsRemoved(item, m_column, m_count);

    // }else  {       }

m_reference()->validReference(m_reference());
   }



   void QStdItem::RemoveColumnsCmd::undo()
   {
     scope_tagger t{"QStdItem::RemoveColumnsCmd::undo"};

      auto item{this_item()};

      QStdItemPrivate* const d= item->d_func();

     UndoStackLock lock{this_model()->undo_stack()}; // RAII class

   if(!is_valid_cmd){return;}

     if(d->model)
     {
         auto _col{m_column>0 ?  m_column-1 : 0};
         //d->model->d_func()->columnsAboutToBeInserted(item,m_column-1,m_count);
                   // verursacht laufzeitfehler, für m_column==0

         d->model->d_func()->columnsAboutToBeInserted(item,_col,m_count);
     }

     if(!is_last_column)
     {
     // int i{m_column}; // d->childIndex(row=0,m_column)

     for(   int row{0}; row< item->rowCount(); ++row)
     {
       //   int i = d->childIndex(row,m_column) + row*m_count ;


            int i = d->childIndex(row,m_column) + row*m_count ;
         // compensate the index, by the offset of already inserted items




          // in row major indexing , the 'm_count' new column items in this row,
          // are consecutive
         for(int j{0}; j<m_count ;++j)
        {
          auto item = m_items.takeFirst();

          d->children.insert(i+j ,item);

         }

          }

     }else
     {
         for(   int row{0}; row< item->rowCount(); ++row)
         {
         // dann liegt einer dieser fälle vor
         /*  ((row < 0) || (column < 0)     || (row >= rowCount()) || (column >= columnCount()) )*/
           // das passiert denke ich immer, wenn das RemoveColumnsCmd die letzte spalte betraf/ bzw betrifft


             int i = d->childIndex(row,m_column-1) + row*m_count +1;

             for(int j{0}; j<m_count ;++j)
            {
              auto item = m_items.takeFirst();

              d->children.insert(i+j ,item);

             }



         }

     }


     d->columns+=m_count;


    if(d->model)
    {

      //  d->model->d_func()->columnsInserted(item,m_column,m_count);


          auto _col{m_column>0 ?  m_column-1 : 0};
          d->model->d_func()->columnsInserted(item,_col ,m_count);
    }

m_reference()->validReference(m_reference());
   }






   void QStdItem::InsertColumnCmd::redo()
   {
       scope_tagger t{"QStdItem::InsertColumnCmd::redo"};
      auto item{this_item()};

      QStdItemPrivate* const d= item->d_func();

     UndoStackLock lock{this_model() ? this_model()->undo_stack(): nullptr}; // RAII class

                    //    QList<QStdItem*> _pitems;

                      //                 for(auto& i :m_items)
                      //                 {_pitems.append(&i);}

       if(!is_valid_cmd)
      {     return;}

      // VARIANTE I Command
     if(m_count==-1)
     {
         // if (m_column < 0)         return;

         if (resize_rows)
          {   item->setRowCount(m_items.count());
         }

          setSuccessFlag(  d->insertColumns(m_column, 1, m_items) );

     } // VARIANTE II Command
     else
     {
         int count{m_count};
         int column{m_column};


             //if (prev_col_count < m_column)
              if(insert_past_end)
             {
                 count += m_column - prev_col_count;
                 column = prev_col_count;
             }
           setSuccessFlag(  d->insertColumns(column, count, QList<QStdItem*>()) ) ;


     }

                    m_reference()->validReference(m_reference());
   }

     // VARIANTE II
     QStdItem::InsertColumnCmd::InsertColumnCmd(
                 QStdItem* i,
                  int column,
                  int count,
                  QUndoCommand* parent)
         :StdItemCmd(i,parent) ,
           m_count(count),
           m_column(column)
     {
         prev_row_count= i->rowCount();
         prev_col_count=i->columnCount();

   setText(QString("InsertColumnCmd TypeII - col: %1,count: %2").arg(column).arg(count));

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


     // VARIANTE I
     QStdItem::InsertColumnCmd::InsertColumnCmd(QStdItem* i,
                     int col,
                     const QList<QStdItem*>& items,
                     QUndoCommand* parent)
         : StdItemCmd(i,parent),
           m_column(col),
           item_count(items.count()),
           m_items(items)
     {
         setText(QString("InsertColumnCmd TypI - col: %1,count: %2").arg(col).arg(items.count()));


         prev_row_count = i->rowCount();
          prev_col_count=i->columnCount();

         // the commands stores deep copies of the inserted items
         // not pointer
       //  for(auto* it : items)
       //  {   m_items.emplace_back(*it);      }


         if(! ( (m_column < 0) || (m_column >prev_col_count) ) )
         {
             is_valid_cmd=false;
         }

         if(prev_row_count < m_items.count())
         {resize_rows=true;}
     }

   void QStdItem::InsertColumnCmd::undo()
   {scope_tagger t{"QStdItem::InsertColumnCmd::undo"};

      auto item{this_item()};

       QStdItemPrivate* const d= item->d_func();

      UndoStackLock lock{this_model() ? this_model()->undo_stack() : nullptr}; // RAII class

       if(!is_valid_cmd)return;


       // VARIANTE I Command
      if(m_count==-1)
      {
          // if (m_column < 0)           return;

          m_items= item->takeColumn(m_column);
          // save the items before any of the subsequent command frees them

          if (resize_rows)
          {    item->setRowCount(prev_row_count );
          }

        //  d->removeColumns(m_column, m_items.count() );
            item->removeColumns(m_column,item_count );
      }else
          // VARIANTE II Command [no items to save here, since only nullptr were inserted in the first place]
      {

         // if(prev_col_count< m_column)
          if(insert_past_end)
          {
              item->setColumnCount(prev_col_count);
          }
          else
          {

                item->removeColumns(m_column,m_count);
          }

      }
    m_reference()->validReference(m_reference());
   }



   void QStdItem::InsertRowCmd::redo()
   {
       scope_tagger t{ "QStdItem::InsertRowCmd::redo"};

      auto item{this_item()};

      QStdItemPrivate* const d= item->d_func();

     UndoStackLock lock{this_model() ? this_model()->undo_stack() : nullptr}; // RAII class

  // QList<QStdItem*> _pitems;

              //   for(auto& i :m_items)
              //   {_pitems.append(&i);}

        // VARIANTE I Command
      if(m_count ==-1)
      {

          if (m_row < 0)
              return;

          if(resize_columns)
          {


        //  if (prev_col_count < m_items.count()) // das macht ja jetzt 'resize_columns'
          item->setColumnCount(item_count);

           // increases column count if neccessary
          setSuccessFlag( d->insertRows(m_row, 1, m_items) );
          }else
          {
          setSuccessFlag( d->insertRows(m_row, m_items) );
         }
      }
      else
          // VARIANTE II Command
      {
          int count{m_count};
          int row{m_row};

       //   if (item->rowCount() < m_row)
          if(insert_past_end)
          {
              count += m_row - prev_row_count;
              row = prev_row_count;
          }
          setSuccessFlag( d->insertRows(row, count, QList<QStdItem*>()) );
      }


   }

     // VARIANTE I Command
     QStdItem::InsertRowCmd::InsertRowCmd(
                 QStdItem* it,
                  int row,
                  const QList<QStdItem*>&items,
                  bool do_resize,
                  QUndoCommand* parent)
         :StdItemCmd(it,parent) ,
           resize_columns(do_resize),
           m_row(row),
           m_items(items),
           item_count(items.count())
     {
         setText(QString("InsertRowCmd Typ I - row: %1, count: %2, do_resize: %3").arg(row).arg(items.count() ).arg(do_resize) );

         prev_col_count = it->columnCount();
         prev_row_count= it->rowCount();

         // the commands stores deep copies of the inserted items
         // not pointer
        // for(auto* it : items)
        // {   m_items.emplace_back(*it);      }


         if(prev_col_count< m_items.count())
         {
             resize_columns=true;
         }
     }


     // VARIANTE II Command
     QStdItem::InsertRowCmd::InsertRowCmd(
                 QStdItem* it,
                  int row,
                  int count,
                  QUndoCommand* parent)
         :StdItemCmd(it,parent) ,
           m_count(count),
           m_row(row)
     {
          setText(QString("InsertRowCmd Typ II - row: %1, count: %2").arg(row).arg(count) );

         prev_row_count= it->rowCount();
         prev_col_count=it->columnCount();

         if(prev_row_count < m_row)
         {
             insert_past_end=true;
         }
     }


   void QStdItem::InsertRowCmd::undo()
   {
      scope_tagger t{"QStdItem::InsertRowCmd::undo"};
      auto item{this_item()};

      QStdItemPrivate* const d= item->d_func();

     UndoStackLock lock{this_model() ? this_model()->undo_stack() : nullptr}; // RAII class

     // handelt es sich bei diesem Cmd um einen 'insertRows(int row,int count )' Aufruf ?
     // oder um einen 'insertRows(int row,const QList<QStdItem*>& items)' Aufruf

    // VARIANTE I Command
     if(m_count==-1)
     {
       if (m_row < 0)
             return;

          if(resize_columns)
          {
              m_items = item->takeRow(m_row);

              if (prev_col_count < item_count )
              {item->setColumnCount(prev_col_count);
              }


              item->removeRows(m_row,m_items.count()) ;
          }
          else
          {
              m_items = item->takeRow(m_row);

               item->removeRows(m_row,item_count ) ;
          }
     }
     else
         // es handelt sich bei diesem Cmd um einen 'insertRows(int row,int count )' Aufruf
         // dann müssen nämlich keine items in m_items gerettet werden, weil nur nullptr eingefügt wurde
         // VARIANTE II Command
     {

         //if (prev_row_count < m_row)
         if(insert_past_end)
         {
          item->setRowCount(prev_row_count);
         }
         else
         {
            item->removeRows(m_row,m_count);
         }


     }


   }

   void QStdItem::SetColumnCountCmd::redo()
   {
      qDebug()<< "<QStdItem::SetColumnCountCmd::redo> ";
    impl();

      qDebug()<< "</QStdItem::SetColumnCountCmd::redo> ";

   }
   void QStdItem::SetColumnCountCmd::impl()
   {

      auto item{this_item()};

      QStdItemPrivate* const d= item->d_func();

     UndoStackLock lock{this_model() ? this_model()->undo_stack() : nullptr}; // RAII class


     int cc = item->columnCount();
     // Nothing to do here
     if (cc == m_columns)
         return;

     if (cc < m_columns)
     {    item->insertColumns(qMax(cc, 0), m_columns - cc);}
     else
     {item->removeColumns(qMax(m_columns, 0), cc - m_columns);}

     // backup the previous column count
     m_columns=cc;
   }

   void QStdItem::SetColumnCountCmd::undo()
   {
      scope_tagger t{ "QStdItem::SetColumnCountCmd::undo"};
    //  redo(); // setColumnCount is an involution
      impl();

   }


   void QStdItem::SetRowCountCmd::redo()
   {
       scope_tagger t{ "QStdItem::SetRowCountCmd::redo"};

     impl();

   }

     void QStdItem::SetRowCountCmd::impl()
     {

         auto item{this_item()};

         QStdItemPrivate* const d= item->d_func();

        UndoStackLock lock{this_model() ? this_model()->undo_stack():nullptr }; // RAII class

        // what is the current-row-count
         int rc = item->rowCount();

         // Nothing to do here
         if (rc == m_rows)
             return;

         if (rc < m_rows)
         {   // this does not increase column count ?! right
             item->insertRows(qMax(rc, 0), m_rows - rc);}
         else
         {        item->removeRows(qMax(m_rows, 0), rc - m_rows);}


         m_rows=rc;
     }


   void QStdItem::SetRowCountCmd::undo()
   {
      scope_tagger t{ "QStdItem::SetRowCountCmd::undo"};
     //redo(); // setRowCount is an involution
      impl();

   }


  QStdItem::SetChildCmd:: SetChildCmd(
              QStdItem* i,
               int row,
               int column,
               QStdItem *item, // the child item to be inserted
               bool emitChanged,
               QUndoCommand* parent )
       : StdItemCmd(i,parent ) ,
         m_row(row),
         m_column(column),
   m_child(item),
     _emit_changed_(emitChanged)
   {
       setText(QString("SetChildCmd - row: %1, col: %2").arg(row).arg(column));

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

   void QStdItem::SetChildCmd::redo()
   {

     scope_tagger t{ "QStdItem::SetChildCmd::redo"};

     // Q_D(QStdItem);
       QStdItemPrivate* const d= this_item()->d_func();

      UndoStackLock lock{this_model() ? this_model()->undo_stack() : nullptr}; // RAII class

       // this_model()->undo_stack()->beginMacro(QString("QStdItem::setChild(%1,%2)").arg(m_row).arg(m_column));


       // setChild will call setRowCount , setColumnCount to resize child table if eighter: columns <m_column || rows < m_row


          d->setChild(m_row, m_column, m_child, true); // setChild is 'void' , No ret_code
          // this method calls many main class functions through its q-pointer,
          // which are internally implemented in terms of UndoCommands themselfes
          // but with the push_lock in place, its safe. the stack will execute any pushed command,
          // but it wont


      // this_model()->undo_stack()->endMacro();


   }

   void QStdItem::SetChildCmd::undo()
   {
      scope_tagger t{ "QStdItem::SetChildCmd::undo"};

      auto item{this_item()};
       QStdItemPrivate* const d= this_item()->d_func();

       int row_cnt{item->rowCount()};
       int col_cnt{item->columnCount()};

        //   this_model()->undo_stack()->beginMacro(QString("QStdItem::setChild(%1,%2)").arg(m_row).arg(m_column));

        UndoStackLock lock{this_model() ? this_model()->undo_stack() : nullptr};

         m_child = item->takeChild(m_row,m_column) ; // the command regains ownership of the child item
        // maybe even removeRow here ?


       // HIER muss ich die vorherige columnCount wiederherstellen,
                        // wenn sie beim einfügen des items erhöht wurde

                        /* das ist die stelle in QStdItemPrivate::setChild(row,column
                         * wo entschieden wird ob die childtable resized wird
                           if (rows <= row){    q->setRowCount(row + 1);}

                            if (columns <= column){       q->setColumnCount(column + 1);}
                           */


                        if(resize_columns)
                        {
                            //item->setColumnCount(m_column-1);
                            item->setColumnCount(prev_col_count);
                        }

                        if(resize_rows)
                        {
                            item->setRowCount(prev_row_count);
                        }


        // this would push() another (a 'TakeChildCmd') onto the models stack,
        // if takeChild() would use an UndoComand internally

       /*TakeChildCmd tmpCmd{this_item(),m_row,m_column};
       tmpCmd.redo();
        m_child= tmpCmd.get_child() ;*/


   }

   void QStdItem::ClearDataCmd::redo()
   {

     scope_tagger t{ "QStdItem::ClearDataCmd::redo"};


      QStdItemPrivate* const d= this_item()->d_func();
       UndoStackLock lock{this_model() ? this_model()->undo_stack() : nullptr}; // RAII class

      if (d->values.isEmpty())
          return;
      m_values = d->values; // create a backup copy before clearing the data


      // create a list of data roles, which were set on the item
      for(const auto& data : m_values)
      {
          m_roles.append(data.role);
      }


      d->values.clear();

      if (d->model)
          d->model->d_func()->itemChanged(this_item() , QList<int>{});


   }



   void QStdItem::ClearDataCmd::undo()
   {
     scope_tagger t {"QStdItem::ClearDataCmd::undo"};

       QStdItemPrivate* const d= this_item()->d_func();

        UndoStackLock lock{this_model()->undo_stack()}; // RAII class

       d->values= m_values;

       m_values.clear();

       if (d->model)
           d->model->d_func()->itemChanged(this_item() ,m_roles);

       m_roles.clear();



   }


   void QStdItem::SetDataCmd::redo()
   {
   scope_tagger t{ "QStdItem::SetDataCmd::redo"};

    impl();


   }

      void QStdItem::SetDataCmd::impl()
      {

          UndoStackLock lck{get_stack()};

          // ist dieses Commando ein 'setItemData(QMap<int,QVariant>& ) Aufruf ?
          if(m_values)
          {
              impl_multi_role();
          }else{ // oder ein setData(int role,const QVariant& value) Aufruf
              impl_single_role();
          }

      }


         void QStdItem::SetDataCmd::impl_multi_role()
         {
             auto item{this_item()};
             QStdItemPrivate * const d = item->d_func();


             // sichere die Daten, die das item jetzt gerade trägt
             std::unique_ptr<QMap<int,QVariant>> tmp{std::make_unique<QMap<int,QVariant>>(item->itemData())};

             // dann setze die neuen Daten auf dem item
             d->setItemData(*m_values.release());

             // die alten daten sind jetzt die neuen -> involution !
             m_values.swap(tmp);


         }

      void QStdItem::SetDataCmd::impl_single_role()
      {
          QVariant old_value;

        //  Q_D(QStdItem);
          QStdItemPrivate * const d = this_item()->d_func();


          m_role = (m_role == Qt::EditRole) ? Qt::DisplayRole : m_role;

          const QList<int> roles((m_role == Qt::DisplayRole) ?
                                      QList<int>({Qt::DisplayRole, Qt::EditRole}) :
                                      QList<int>({m_role}));

          for (auto it = d->values.begin(); it != d->values.end(); ++it)
          {
              if ((*it).role == m_role)
              {
                  if (m_value.isValid())
                  {
                      if ((*it).value.userType() == m_value.userType() && (*it).value == m_value)
                          return;  // nothing to do here , because old value is identical to new value

                      old_value= (*it).value; // save the old value to be able to restore it later

                          (*it).value = m_value;

                      m_value=old_value;
                  } else {
                      // Don't need to assign proper it after erase() since we
                      // return unconditionally in this code path.

                      old_value= (*it).value;
                      m_value=old_value;

                      d->values.erase(it);

                  }

                  if (d->model)
                      d->model->d_func()->itemChanged(this_item(), roles);
                  return;
              }
          }


          // 'this_item' had no data values set prior to this fcn call
          d->values.append(QStdItemData(m_role, m_value));

          m_value= old_value; // the old_value before the fcn call was an empty inValid QVariant, as is 'old_value'

          if (d->model)
              d->model->d_func()->itemChanged(this_item() , roles);
      }

   void QStdItem::SetDataCmd::undo()
   {
      scope_tagger t{"QStdItem::SetDataCmd::undo"};
      //redo(); // theese two fcns are involutions
      impl();


   }
