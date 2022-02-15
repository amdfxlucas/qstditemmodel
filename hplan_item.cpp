#include "hplan_item.h"

plan_item::plan_item()
    :QStandardItem()
{

}

plan_item::plan_item(PlanItemTypes type)
    : QStandardItem()
{
    init_flags(type);
}

void plan_item::init_flags(PlanItemTypes type)
{

    switch(type)
    {
    case YearType:
        setup_year_type();
        break;
    case MonthType:
        setup_month_type();
        break;
    case WeekType:
        setup_week_type();
        break;
    case DayType:
        setup_day_type();
        break;
    case OrderType:
        setup_order_type();
        break;

    };


}

int plan_item::type() const
{
    return data(DataRoles::TypeRole).value<int>() ;
}


void plan_item::setup_order_type()
{
   setData(PlanItemTypes::OrderType,DataRoles::TypeRole);

    setFlags(Qt::ItemNeverHasChildren);
}


void plan_item::setup_day_type()
{
    setData(PlanItemTypes::DayType,DataRoles::TypeRole);
}

void plan_item::setup_year_type()
{
    setData(PlanItemTypes::YearType,DataRoles::TypeRole);
        setDragEnabled(false)   ;
}

void plan_item::setup_week_type()
{
    setData(PlanItemTypes::WeekType,DataRoles::TypeRole);
}


void plan_item::setup_month_type()
{
    setData(PlanItemTypes::MonthType,DataRoles::TypeRole);
}


plan_item::~plan_item()
{

}

plan_item::plan_item(PlanItemTypes type,const QString& text)
    : QStandardItem(text)
{
    init_flags(type);
}

plan_item::plan_item(const QString& text)
    : QStandardItem(text)
{

}

plan_item::plan_item(int rows,int columns)
    : QStandardItem(rows,columns)
{

}

