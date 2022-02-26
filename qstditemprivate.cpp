
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


class QStdItemModelLessThan
{
public:
   inline QStdItemModelLessThan()
       { }

   inline bool operator()(const QPair<QStdItem*, int> &l,
                          const QPair<QStdItem*, int> &r) const
   {
       return *(l.first) < *(r.first);
   }
};

class QStdItemModelGreaterThan
{
public:
   inline QStdItemModelGreaterThan()
       { }

   inline bool operator()(const QPair<QStdItem*, int> &l,
                          const QPair<QStdItem*, int> &r) const
   {
       return *(r.first) < *(l.first);
   }
};

bool QStdItemPrivate::hasChildren()const
{
    return rows>0 && columns>0;
}

inline int QStdItemPrivate::childIndex(const QStdItem *child) const
{
   const int lastChild = children.size() - 1;

   int &childsLastIndexInParent = child->d_func()->lastKnownIndex;

   if (childsLastIndexInParent != -1 && childsLastIndexInParent <= lastChild)
   {
       if (children.at(childsLastIndexInParent) == child)
           return childsLastIndexInParent;
   } else
   {
       childsLastIndexInParent = lastChild / 2;
   }

   // assuming the item is in the vicinity of the previous index, iterate forwards and
   // backwards through the children
   int backwardIter = childsLastIndexInParent - 1;
   int forwardIter = childsLastIndexInParent;
   for (;;)
   {
       if (forwardIter <= lastChild)
       {
           if (children.at(forwardIter) == child) {
               childsLastIndexInParent = forwardIter;
               break;
           }
           ++forwardIter;
       } else if (backwardIter < 0) {
           childsLastIndexInParent = -1;
           break;
       }
       if (backwardIter >= 0) {
           if (children.at(backwardIter) == child) {
               childsLastIndexInParent = backwardIter;
               break;
           }
           --backwardIter;
       }
   }
   return childsLastIndexInParent;
}

Qt::ItemFlags QStdItemPrivate::setFlags(Qt::ItemFlags f){ return setData(static_cast<int>(f),Qt::UserRole-1).value<Qt::ItemFlags>();}
QVariant QStdItemPrivate::setData(int m_role,const QVariant& m_value)
{
    QVariant old_value;

              m_role = (m_role == Qt::EditRole) ? Qt::DisplayRole : m_role;

              const QList<int> roles((m_role == Qt::DisplayRole) ?
                                          QList<int>({Qt::DisplayRole, Qt::EditRole}) :
                                          QList<int>({m_role}));

              for (auto it = values.begin(); it != values.end(); ++it)
              {
                  if ((*it).role == m_role)
                  {
                      if (m_value.isValid())
                      {
                          if ((*it).value.userType() == m_value.userType() && (*it).value == m_value)
                              return m_value;  // nothing to do here , because old value is identical to new value

                          old_value= (*it).value; // save the old value to be able to restore it later

                              (*it).value = m_value;


                      } else {
                          // Don't need to assign proper it after erase() since we
                          // return unconditionally in this code path.

                          old_value= (*it).value;


                          values.erase(it);

                      }

                      if (model)
                          model->d_func()->itemChanged(q_func(), roles);
                      return old_value;
                  }
              }


              // 'this_item' had no data values set prior to this fcn call
              values.append(QStdItemData(m_role, m_value));
              if (model)
                  model->d_func()->itemChanged(q_func(), roles);

              return QVariant();
}


QPair<int, int> QStdItemPrivate::position() const
{
   if (QStdItem *par = parent)
   {
       int idx = par->d_func()->childIndex(q_func());
       if (idx == -1)
       {     return QPair<int, int>(-1, -1);}

       // remap linear-row-major index to row,col multi-Index
       return QPair<int, int>(idx / par->columnCount(), idx % par->columnCount());
   }
   // ### support header items?
   return QPair<int, int>(-1, -1);
}


QStdItem* QStdItemPrivate::setChild(int row, int column, QStdItem *item,
                                   bool emitChanged)
{

    scope_tagger t{ "QStdItemPrivate::setChild(int row,int column,QStdItem* item,bool emitChanged"};




   Q_Q(QStdItem);
   if (item == q)
   {
       qWarning("QStdItem::setChild: Can't make an item a child of itself %p",           item);
         return nullptr;
   }

   if ((row < 0) || (column < 0))
   {
       return nullptr;
   }

   if (rows <= row)
       q->setRowCount(row + 1);

   if (columns <= column)
       q->setColumnCount(column + 1);

   int index = childIndex(row, column);

   Q_ASSERT(index != -1);
   QStdItem *oldItem = children.at(index);

   if (item == oldItem)
  {
       return nullptr;}

   if (model && emitChanged) {
       emit model->layoutAboutToBeChanged();
   }

   if (item)
   {
       if (item->d_func()->parent == nullptr)
       {
           item->d_func()->setParentAndModel(q, model);
       } else
       {
           qWarning("QStdItem::setChild: Ignoring duplicate insertion of item %p",            item);

           return nullptr;
       }
   }

   // setting the model to nullptr invalidates the persistent index which we want to avoid
   if (!item && oldItem)
    { //  oldItem->d_func()->setModel(nullptr);
         oldItem->setModel(nullptr);
   }

   children.replace(index, item);

   // since now indexFromItem() does no longer return a valid index, the persistent index
   // will not be invalidated anymore
   if (oldItem)
   { //   oldItem->d_func()->setModel(nullptr);
          oldItem->setModel(nullptr);
   }
  // delete oldItem;

   if (item)
       item->d_func()->lastKnownIndex = index;

   if (model && emitChanged)
       emit model->layoutChanged();

   if (emitChanged && model)
   {
       if (item)
       {
           model->d_func()->itemChanged(item);
       } else
       {
           const QModelIndex idx = model->index(row, column, q->index());
           emit model->dataChanged(idx, idx);
       }
   }

return oldItem;
}


void QStdItemPrivate::changeFlags(bool enable, Qt::ItemFlags f)
{
     qDebug()<< "<QStdItemPrivate::changeFlags(bool enable,Qt::ItemFlags)> ";

   Q_Q(QStdItem);
   Qt::ItemFlags flags = q->flags();
   if (enable)
       flags |= f;
   else
       flags &= ~f;
   q->setFlags(flags);

   qDebug()<< "</QStdItemPrivate::changeFlags(bool enable,Qt::ItemFlags)> ";
}


void QStdItemPrivate::childDeleted(QStdItem *child)
{
   qDebug()<< "<QStdItemPrivate::childDeleted(QStdItem* child)> ";

   int index = childIndex(child);
   Q_ASSERT(index != -1);
   const auto modelIndex = child->index();
   children.replace(index, nullptr);
   emit model->dataChanged(modelIndex, modelIndex);

   qDebug()<< "</QStdItemPrivate::childDeleted(QStdItem* child)> ";
}

namespace {

   struct ByNormalizedRole
   {
       static int normalizedRole(int role)
       {
           return role == Qt::EditRole ? Qt::DisplayRole : role;
       }

      bool operator()(const QStdItemData& standardItemData, const std::pair<const int &, const QVariant&>& roleMapIt) const
      {
          return standardItemData.role < normalizedRole(roleMapIt.first);
      }
      bool operator()(const std::pair<const int&, const QVariant &>& roleMapIt, const QStdItemData& standardItemData) const
      {
          return normalizedRole(roleMapIt.first) < standardItemData.role;
      }

   };

   /*
       Based on std::transform with a twist. The inputs are iterators of <int, QVariant> pair.
       The variant is checked for validity and if not valid, that element is not taken into account
       which means that the resulting output might be shorter than the input.
   */
   template<class Input, class OutputIt>
   OutputIt roleMapStandardItemDataTransform(Input first1, Input last1, OutputIt d_first)
   {
       while (first1 != last1) {
           if ((*first1).second.isValid())
               *d_first++ = QStdItemData(*first1);
           ++first1;
       }
       return d_first;
   }


   /*
       Based on std::set_union with a twist. The idea is to create a union of both inputs
       with an additional constraint: if an input contains an invalid variant, it means
       that this one should not be taken into account for generating the output.
   */
   template<class Input1, class Input2,
            class OutputIt, class Compare>
   OutputIt roleMapStandardItemDataUnion(Input1 first1, Input1 last1,
                                         Input2 first2, Input2 last2,
                                         OutputIt d_first, Compare comp)
   {
       for (; first1 != last1; ++d_first) {
           if (first2 == last2) {
               return roleMapStandardItemDataTransform(first1, last1, d_first);
           }
           if (comp(*first2, *first1)) {
               *d_first = *first2++;
           } else {
               if ((*first1).second.isValid())
                   *d_first = QStdItemData(*first1);
               if (!comp(*first1, *first2))
                   ++first2;
               ++first1;
           }
       }
       return std::copy(first2, last2, d_first);
   }
}

void QStdItemPrivate::setItemData(const QMap<int, QVariant> &roles)
{
 auto print_start =[](){  qDebug()<< "<QStdItemPrivate::setItemData(const QMap<int,QVariant>& roles)> ";};
 auto print_end= [](){ qDebug()<< "</QStdItemPrivate::setItemData(const QMap<int,QVariant>& roles)> ";};

 print_start();

   Q_Q(QStdItem);

   auto byRole = [](const QStdItemData& item1, const QStdItemData& item2) {
       return item1.role < item2.role;
   };

   std::sort(values.begin(), values.end(), byRole);

   /*
       Create a list of QStdItemData that will contain the original values
       if the matching role is not contained in roles, the new value if it is and
       if the new value is an invalid QVariant, it will be removed.
   */
   QList<QStdItemData> newValues;
   newValues.reserve(values.size());
   roleMapStandardItemDataUnion(roles.keyValueBegin(),
                                roles.keyValueEnd(),
                                values.cbegin(), values.cend(),
                                std::back_inserter(newValues), ByNormalizedRole());

   if (newValues != values)
   {
       values.swap(newValues);
       if (model)
       {
           QList<int> roleKeys;
           roleKeys.reserve(roles.size() + 1);
           bool hasEditRole = false;
           bool hasDisplayRole = false;

           for (auto it = roles.keyBegin(); it != roles.keyEnd(); ++it)
           {
               roleKeys.push_back(*it);
               if (*it == Qt::EditRole)
                   hasEditRole = true;
               else if (*it == Qt::DisplayRole)
                   hasDisplayRole = true;
           }

           if (hasEditRole && !hasDisplayRole)
               roleKeys.push_back(Qt::DisplayRole);
           else if (!hasEditRole && hasDisplayRole)
               roleKeys.push_back(Qt::EditRole);
           model->d_func()->itemChanged(q, roleKeys);
       }
   }
  print_end();
}


const QMap<int, QVariant> QStdItemPrivate::itemData() const
{
   QMap<int, QVariant> result;
   QList<QStdItemData>::const_iterator it;

   for (it = values.cbegin(); it != values.cend(); ++it)
   {
       // Qt::UserRole - 1 is used internally to store the flags
       if (it->role != Qt::UserRole - 1)
           result.insert(it->role, it->value);
   }
   return result;
}

bool QStdItemPrivate::hasChild(unsigned long long int uuid)const
{
    auto start{this};

    auto iterate = [&uuid](const auto* start_item)
    {

        auto iterate_impl = [&uuid](const QStdItemPrivate* start_item,auto& impl)
        {


                if(start_item->uuid() == uuid) return true;

                if(start_item->hasChildren())
                {
             for(const auto item : start_item->children)
            {
                if(item)
                {
                 //  if(item->hasChildren())
                 //   {
                 //      if(item->uuid() == uuid) return true;

                      return impl(item->d_func(),impl);
                //   }
               //   else
               //  {
               //     return item->uuid() == uuid;
               //   }

                }
              }
                }


             return false;
        };

        return iterate_impl(start_item,iterate_impl);
    };

    return iterate(start);
}

void QStdItemPrivate::sortChildren(int column, Qt::SortOrder order)
{
   auto print_start = [](){qDebug()<< "<QStdItemPrivate::sortChildren(int column,Qt::SortOrder )> ";};
   auto print_end = [](){qDebug()<< "</QStdItemPrivate::sortChildren(int column,Qt::SortOrder )> ";};

   print_start();

   Q_Q(QStdItem);
   if (column >= columnCount())
   {print_end();return;}

   QList<QPair<QStdItem*, int> > sortable;
   QList<int> unsortable;

   sortable.reserve(rowCount());
   unsortable.reserve(rowCount());

   for (int row = 0; row < rowCount(); ++row)
   {
       QStdItem *itm = q->child(row, column);
       if (itm)
           sortable.append(QPair<QStdItem*,int>(itm, row));
       else
           unsortable.append(row);
   }

   if (order == Qt::AscendingOrder) {
       QStdItemModelLessThan lt;
       std::stable_sort(sortable.begin(), sortable.end(), lt);
   } else {
       QStdItemModelGreaterThan gt;
       std::stable_sort(sortable.begin(), sortable.end(), gt);
   }

   QModelIndexList changedPersistentIndexesFrom, changedPersistentIndexesTo;
   QList<QStdItem*> sorted_children(children.count());

   for (int i = 0; i < rowCount(); ++i)
   {
       int r = (i < sortable.count()
                ? sortable.at(i).second
                : unsortable.at(i - sortable.count()));

       for (int c = 0; c < columnCount(); ++c)
       {
           QStdItem *itm = q->child(r, c);
           sorted_children[childIndex(i, c)] = itm;
           if (model)
           {
               QModelIndex from = model->createIndex(r, c, q);

               if (model->d_func()->persistent.indexes.contains(from))
               {
                   QModelIndex to = model->createIndex(i, c, q);
                   changedPersistentIndexesFrom.append(from);
                   changedPersistentIndexesTo.append(to);
               }
           }
       }
   }

   children = sorted_children;

   if (model) {
       model->changePersistentIndexList(changedPersistentIndexesFrom, changedPersistentIndexesTo);
   }

   QList<QStdItem*>::iterator it;
   for (it = children.begin(); it != children.end(); ++it)
   {
       if (*it)
           (*it)->d_func()->sortChildren(column, order);
   }
   print_end();
}

/*!
 \internal
 set the model of this item and all its children
 */
void QStdItemPrivate::setModel(QStdItemModel *mod)
{
   scope_tagger t{"QStdItemPrivate::setModel"};

   if (children.isEmpty())
   {
       if (model)
           model->d_func()->invalidatePersistentIndex(model->indexFromItem(q_ptr));
       model = mod;
   } else
   {
       QStack<QStdItem*> stack;
       stack.push(q_ptr);
       while (!stack.isEmpty())
       {
           QStdItem *itm = stack.pop();
           if (itm->d_func()->model)
           {
               itm->d_func()->model->d_func()->invalidatePersistentIndex(itm->d_func()->model->indexFromItem(itm));
           }
           itm->d_func()->model = mod;
           const QList<QStdItem*> &childList = itm->d_func()->children;

           for (int i = 0; i < childList.count(); ++i)
           {
               QStdItem *chi = childList.at(i);
               if (chi)
                   stack.push(chi);
           }
       }
   }

}



/*none of the items from 'items' is discarded,
every one of them will be inserted into the item */
bool QStdItemPrivate::insertRows(int row, const QList<QStdItem*> &items)
{
 scope_tagger t {"QStdItemPrivate::insertRows(int row, const QList<QStdItem*>& items)"} ;



   Q_Q(QStdItem);

   if ((row < 0) || (row > rowCount()) || items.isEmpty())
   {return false;}

   int count = items.count();

   if (model)
       model->d_func()->rowsAboutToBeInserted(q, row, row + count - 1);

   if (rowCount() == 0)
   {
       if (columnCount() == 0)
           q->setColumnCount(1);

       children.resize(columnCount() * count);
       rows = count;
   } else
   {
       rows += count;
       int index = childIndex(row, 0);
       if (index != -1)
           children.insert(index, columnCount() * count, nullptr);
       // QList::insert(qsizetype i, qsizetype count, QList::parameter_type value)
       // This is an overloaded function.
       // Inserts count copies of value at index position i in the list.
   }

   for (int i = 0; i < items.count(); ++i)
   {
       QStdItem *item = items.at(i);

       item->d_func()->model = model;
       item->d_func()->parent = q;

       int index = childIndex(i + row, 0);

       children.replace(index, item);

       if (item)
           item->d_func()->lastKnownIndex = index;
   }

   if (model)
       model->d_func()->rowsInserted(q, row, count);


   return true;
}

/*if columnCount()x count is less then items.count() the remaining items from
'items' wont be inserted into the item but instead be discarded*/
bool QStdItemPrivate::insertRows(int row, int count, const QList<QStdItem*> &items)
{
   scope_tagger t{ "QStdItemPrivate::insertRows(int row,int count, const QList<QStdItem*>& items)"};

   Q_Q(QStdItem);

   if ((count < 1) || (row < 0) || (row > rowCount()) || count == 0)
   {
       return false;
   }

   if (model)
       model->d_func()->rowsAboutToBeInserted(q, row, row + count - 1);

   if (rowCount() == 0)
   {
       children.resize(columnCount() * count);
       rows = count;
   } else
   {
       rows += count;

       // 'index' is a row-major index into the childs
       int index = childIndex(row, 0);

       // allocate 'count' new rows of length 'columnCount()'
       if (index != -1)
           children.insert(index, columnCount() * count, nullptr);
   }

   if (!items.isEmpty())
   {
       int index = childIndex(row, 0);

       // any item, that doesnt fit into the newly allocated space of 'count' rows with length 'columnCount()'
       // will be discarded
       int limit = qMin(items.count(), columnCount() * count);

       for (int i = 0; i < limit; ++i)
       {
           QStdItem *item = items.at(i);

           if (item)
           {
               if (item->d_func()->parent == nullptr)
               {
                   item->d_func()->setParentAndModel(q, model);
               } else
               {
                   qWarning("QStdItem::insertRows: Ignoring duplicate insertion of item %p",
                            item);
                   item = nullptr;
               }
           }

           children.replace(index, item);

           if (item) {    item->d_func()->lastKnownIndex = index;}

           ++index;
       }
   }

   if (model)
       model->d_func()->rowsInserted(q, row, count);


   return true;
}


bool QStdItemPrivate::insertColumns(int column, int count, const QList<QStdItem*> &items)
{
  scope_tagger t{"QStdItemPrivate::insertColumns(int column, int count, const QList<QStdItem*>& items)"};

   Q_Q(QStdItem);
   if ((count < 1) || (column < 0) || (column > columnCount()) || count == 0)
   {
       return false;
   }
   if (model)
       model->d_func()->columnsAboutToBeInserted(q, column, column + count - 1);

   if (columnCount() == 0)
   {
       children.resize(rowCount() * count);
       columns = count;
   } else
   {
       columns += count;
       int index = childIndex(0, column);

       for (int row = 0; row < rowCount(); ++row)
       {
           children.insert(index, count, nullptr);
           index += columnCount();
       }
   }

   if (!items.isEmpty())
   {
       int limit = qMin(items.count(), rowCount() * count);
       for (int i = 0; i < limit; ++i)
       {
           QStdItem *item = items.at(i);
           if (item)
           {
               if (item->d_func()->parent == nullptr)
               {
                   item->d_func()->setParentAndModel(q, model);
               } else {
                   qWarning("QStdItem::insertColumns: Ignoring duplicate insertion of item %p",
                            item);
                   item = nullptr;
               }
           }
           int r = i / count;
           int c = column + (i % count);
           int index = childIndex(r, c);
           children.replace(index, item);
           if (item)
               item->d_func()->lastKnownIndex = index;
       }
   }
   if (model)
       model->d_func()->columnsInserted(q, column, count);


   return true;
}

