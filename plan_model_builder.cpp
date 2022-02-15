#include "plan_model_builder.h"
#include "hplan_item.h"
#include <QCalendar>
#include <QDate>
plan_model_builder::plan_model_builder(QString year):_year(year)
{

}

hplan_model& plan_model_builder::build(hplan_model& model)
{

    QList<QString> month_str{"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    //QList<QString> weekdays {"Mo","Di","Mi"};

    QCalendar cal;
    const auto number_of_year{_year.toInt()};
    int day_count{cal.daysInYear(number_of_year)};

 /*QList<QStandardItem*>  months{
     new plan_item(PlanItemTypes::MonthType,"Jan"),
             new plan_item(PlanItemTypes::MonthType,"Feb"),
             new plan_item(PlanItemTypes::MonthType,"Mar"),
             new plan_item(PlanItemTypes::MonthType,"Apr"),
             new plan_item(PlanItemTypes::MonthType,"May"),
             new plan_item(PlanItemTypes::MonthType,"Jun"),
             new plan_item(PlanItemTypes::MonthType,"Jul"),
             new plan_item(PlanItemTypes::MonthType,"Aug"),
             new plan_item(PlanItemTypes::MonthType,"Sep"),
             new plan_item(PlanItemTypes::MonthType,"Oct"),
             new plan_item(PlanItemTypes::MonthType,"Nov"),
             new plan_item(PlanItemTypes::MonthType,"Dec")

    };*/


    auto year_item{new plan_item(PlanItemTypes::YearType,_year)};
    year_item->setData(number_of_year,DataRoles::YearRole);
    year_item->setFlags(year_item->flags() & ~Qt::ItemIsDropEnabled & ~Qt::ItemIsDragEnabled);
    // das setzten der ItemFlags sollte denke ich besser in plan_model::flags(QModelInde) erfolgen

    auto root_item{year_item->parent()};
   //  root_item->setFlags(root_item->flags() & ~Qt::ItemIsDropEnabled & ~Qt::ItemIsEnabled );

    model.appendRow(year_item);

 std::vector<int> daysInMon(12);

 for(auto i{1}; i< 12;++i)
 {
     daysInMon[i-1]= cal.daysInMonth(i,_year.toInt() );
 }


 //   model.insertRow(0,months);


    int lastmonth=0;
 int month=1;
 bool is_new_month=false;

 int week=1;

 int lastweek=0;

 bool is_new_week=false;

int weekofmonth=0;

 QDate firstdayofyear{cal.dateFromParts(_year.toInt(),month,1 )};
 QDate lastdayofyear{firstdayofyear.addDays(day_count)};

   // for(int d{1};d<=day_count; ++d)
 for(QDate d{firstdayofyear};d<=lastdayofyear; d= d.addDays(1) )
    {
      //  QDate qdate = cal.dateFromParts(_year.toUInt(),month,d);


        int dayofweek = d.dayOfWeek(cal);
        int dayofmonth = d.day(cal);
         month=d.month(cal);

         is_new_month= month!=lastmonth;
         if(is_new_month)
         {  lastmonth=month;

              auto month_item{new plan_item(PlanItemTypes::MonthType,month_str[month-1] )};
              // achtung: der constructor setzt hier lediglich die 'Qt::DisplayRole'
             //model.appendRow(month_item );
              year_item->appendRow(month_item );
         }


         week = d.weekNumber();//returns the iso week number 1 to 53
         is_new_week = week!=lastweek;

      //   auto month_item{model.item(model.rowCount()-1)};
            auto month_item{year_item->child(year_item->rowCount()-1)};

         month_item->setData(month,DataRoles::MonthOfYear);

         //month_item->setFlags(month_item->flags() & ~Qt::ItemIsDropEnabled & ~Qt::ItemIsDragEnabled);
         // moved to hplan_model::flags(QModelIndex)

         if(is_new_week || is_new_month)
         {lastweek=week;


             ++weekofmonth;

                          if(is_new_month){weekofmonth=1;}// else{++weekofmonth;}


                    auto week_item {new plan_item(PlanItemTypes::WeekType, "KW "+ QString::number(week)  )};

                     month_item->appendRow(week_item);// achtung: der constructor setzt hier lediglich die 'Qt::DisplayRole'



         }

         auto week_item{month_item->child(month_item->rowCount()-1) }; // sowas wie QStandardItem::lastChild() wäre cool


         week_item->setData(week,DataRoles::WeekOfYear);
         week_item->setData(weekofmonth,DataRoles::WeekOfMonth);
      //   week_item->setFlags(week_item->flags() & ~Qt::ItemIsDropEnabled & ~Qt::ItemIsDragEnabled );
         // moved to hplan_model::flags(QModelIndex)

         auto day_item{new plan_item(PlanItemTypes::DayType, d.toString() )}; // achtung: der constructor setzt hier lediglich die 'Qt::DisplayRole'

         // moved to hplan_model::flags(QModelIndex)
         // ist der Tag ein regulärer Arbeitstag ?
        /* if(dayofweek < 5)
         {
             day_item->setData(true,DataRoles::IsWorkdayRole);

         }else{
             day_item->setFlags(day_item->flags() & ~Qt::ItemIsDropEnabled);
         }
         */


         day_item->setData(dayofweek,DataRoles::WeekdayRole);
         day_item->setData(d,DataRoles::DateRole);
         day_item->setData(dayofmonth,DataRoles::DayOfMonth);

         week_item->appendRow( day_item);


        // if(dayofmonth==daysInMon[month-1])       {           ++month;        }

    }
return model;
}
