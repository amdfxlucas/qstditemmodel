#include "qstditemmodel.h"
#include "qstditemmodel_p.h"
#include "scope_tagger.h"

void QStdItemModel::SetHHeaderItemCmd::undo()
{
impl(false);
}

void QStdItemModel::SetHHeaderItemCmd::impl(bool redo)
{
      UndoStackLock lock{model()->undo_stack()}; // RAII class

    QStdItemModelPrivate* d = model()->d_func()           ;

    if (m_column < 0)
        return;

    //if (_this_model_->columnCount() <= m_column)

  //  if (prev_col_count <= m_column)
    if(change_col_count)
    {   if( redo )
        {model()->setColumnCount(m_column + 1);}
        else
        {
            model()->setColumnCount(prev_col_count );
        }
    }

    QStdItem *oldItem = d->columnHeaderItems.at(m_column);

    if (m_item == oldItem)
    {

        return;
    }

    if (m_item)
    {
        if (m_item->model() == nullptr)
        {
            m_item->setModel(model());
        } else
        {
            qWarning("QStdItem::setHorizontalHeaderItem: Ignoring duplicate insertion of item %p",
                     m_item);


            return;
        }
    }

    if (oldItem)
        oldItem->setModel(nullptr);
    //delete oldItem;


    d->columnHeaderItems.replace(m_column, m_item);
    m_item= oldItem;
    emit model()->headerDataChanged(Qt::Horizontal, m_column, m_column);
}

void QStdItemModel::SetHHeaderItemCmd::redo()
{

    impl(true);
}


QVariant QStdItemModel::QStdItemModelCmd::returnValue()const
{
    return QVariant();
}

  QVariant QStdItemModel::CutItemCmd::returnValue()const
{
    return QVariant(model()->pathToIndex(m_ret_path) );
}

   QStdItemModel::CutItemCmd::~CutItemCmd()
   {
       // delete m_cutItemBackup;
   }

void QStdItemModel::CutItemCmd::undo()
{
    UndoStackLock lock{model()->undo_stack() };

    on_scope_exit t{ [this](){ model()->undo_stack()->beginMacro("QStdItemModel::CutItemCmd::undo");
                       qDebug().noquote()<< "<QStdItemModel::CutItemCmd::undo>";},
                     [this](){model()->undo_stack()->endMacro();
                       qDebug().noquote()<< "<QStdItemModel::CutItemCmd::undo>";}
                   };

      auto parent{model()->pathToIndex(model()->parentPath(m_path) ) };

      if(!is_index_valid)
      {
          return;
      }

      int row{m_path.last().first};

      model()->beginInsertRows(parent,row,row);

      // reinsert the models cut_item, right where we removed it (in redo() )
        model()->itemFromIndex(parent)->setChild(row,model()->d_func()->cut_item);

      model()->endInsertRows();

      // restore the cut_item to the one it was before
      model()->d_func()->cut_item = m_cutItemBackup;


      if (row > 0) {

          m_ret_path = m_path;
          return;
      }

      //  we cut the first child of invisibleRoot item
         m_ret_path = model()->pathFromIndex(model()->createIndex(0,0,model()->invisibleRootItem() ) );

}

void  QStdItemModel::CutItemCmd::redo()
{
    UndoStackLock lock{model()->undo_stack() };

  on_scope_exit t{ [this](){ model()->undo_stack()->beginMacro("QStdItemModel::CutItemCmd::redo");
                   qDebug().noquote()<< "<QStdItemModel::CutItemCmd::redo>";},
                   [this](){model()->undo_stack()->endMacro();
                   qDebug().noquote()<<"</QStdItemModel::CutItemCmd::redo>";}
                 };

    auto index{model()->pathToIndex(m_path)};

    if (!is_index_valid)
    {    m_ret_path = m_path;
        // return index;
        return;
    }

   // delete cutItem;
    m_cutItemBackup = model()->d_func()->cut_item;


    model()->d_func()->cut_item = model()->itemFromIndex(index);

    Q_ASSERT(model()->d_func()->cut_item);

    QStdItem *parent =model()->d_func()->cut_item->parent();
    if(!parent){parent=model()->invisibleRootItem();}

    Q_ASSERT(parent);

 //   int row = parent->rowOfChild(model()->d_func()->cut_item );
    int row= model()->d_func()->cut_item->row();

    Q_ASSERT(row == index.row());

    model()->beginRemoveRows(index.parent(), row, row);

        QStdItem *child = parent->takeChild(row);

    model()->endRemoveRows();

    Q_ASSERT(child == model()->d_func()->cut_item);

    child = 0; // Silence compiler unused variable warning

    if (row > 0) {
        --row;
        m_ret_path = model()->pathFromIndex( model()->createIndex(row, 0, parent->child(row)) );
        return;
    }

    if (parent != model()->invisibleRootItem())
    {
        // QStdItem *grandParent = parent->parent();
        // Q_ASSERT(grandParent);
        //return model()->createIndex(grandParent->rowOfChild(parent), 0, parent);

        m_ret_path = model()->pathFromIndex(model()->createIndex(parent->row(),0,parent ) );
        return;
    }

    // we removed top-level item at row==0 [first child of invisible root item]
    m_ret_path = model()->pathFromIndex(QModelIndex() );


}


void QStdItemModel::PasteItemCmd::redo()
{
     auto index{model()->pathToIndex(m_path)};
/*
    if (!index.isValid() || !cutItem)
    {//    return index;
        m_ret_path=m_path;
    }

    QStdItem *sibling = model()->itemFromIndex(index);

    Q_ASSERT(sibling);

    QStdItem *parent = sibling->parent();

    Q_ASSERT(parent);

   // int row = parent->rowOfChild(sibling) + 1;
     int row = sibling->row() + 1;

    model()->beginInsertRows(index.parent(), row, row);

    parent->setChild(row, cutItem);

    QStdItem *child = cutItem;
    cutItem = 0;

    model()->endInsertRows();

    // return model()->createIndex(row, 0, child);

    m_ret_path= model()->pathFromIndex(model()->createIndex(row, 0, child) );
*/
}
