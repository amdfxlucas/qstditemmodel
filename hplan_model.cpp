#include "hplan_model.h"
#include "hplan_item.h"

hplan_model::hplan_model(QObject* parent)
    : QStandardItemModel(parent)
{

}


bool hplan_model::is_workday(const QModelIndex& index) const
{/*this functions puropse is to detect saturdays and sundays
   in order for them to be hidden by the TreeView*/


    if(!index.isValid() )return true;

    auto item{itemFromIndex(index)};
    auto item_type{item->data(DataRoles::TypeRole)};

    if(item_type==PlanItemTypes::DayType)
    {
        auto var{item->data(DataRoles::IsWorkdayRole) };

        if(var.isValid())
        {
            return var.toBool();
        }
        else {return true;}
    }

    return true;
}


Qt::ItemFlags hplan_model::flags(const QModelIndex &index) const
{
    /* ich überschreibe diese Methode, damit ich die ItemFlags von der PlanItemType DataRole abhängig machen kann.
OrderType .. soll '~Qt::ItemIsDropEnabled' sein, aber er muss sich verschieben lassen, also Qt::ItemIsDragEnables (default)
*/

    if(index.isValid())
    {
        auto item{itemFromIndex(index)};


        switch(item->data(DataRoles::TypeRole).toInt() )
        {
        case PlanItemTypes::OrderType :
        {
           // item->setFlags( item->flags() & ~Qt::ItemIsDropEnabled);

            return item->flags() & ~Qt::ItemIsDropEnabled ;
            break;
        }
        case PlanItemTypes::MonthType:
        {
            //item->setFlags(item->flags() & ~Qt::ItemIsDropEnabled & ~Qt::ItemIsDragEnabled);
            return item->flags() & ~Qt::ItemIsDropEnabled & ~Qt::ItemIsDragEnabled;
            break;
        }
        case PlanItemTypes::WeekType:
        {
               return item->flags() & ~Qt::ItemIsDropEnabled & ~Qt::ItemIsDragEnabled ;
               break;
        }
        case PlanItemTypes::DayType:
        {
            // ist der Tag ein regulärer Arbeitstag ?
            if( item->data(DataRoles::IsWorkdayRole).toBool() )
            {
            return item->flags() & ~Qt::ItemIsDropEnabled & ~Qt::ItemIsDragEnabled;
            }
            else
            {
                return item->flags() & ~Qt::ItemIsDragEnabled ; // an arbeitstagen lassen sich produkte dropen


            }
        }
        };

    }

    return QStandardItemModel::flags(index);

}
