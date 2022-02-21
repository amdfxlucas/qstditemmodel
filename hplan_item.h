#ifndef HPLAN_ITEM_H
#define HPLAN_ITEM_H
#include <QStandardItem>
//   enum QStandardItem::ItemType
// UserType = 1000 minimum value for custom types
enum PlanItemTypes{YearType=1001,MonthType,WeekType,DayType,OrderType};

// enum Qt::ItemDataRole -> Qt::UserRole = 0x1000 the first role, that can be used for application specific purposes
enum DataRoles{TypeRole=257,            // ein enumerator aus 'PlanItemTypes'
               QuantityRole,            // auftragsvolumen eines produktes in m
               WONRole,                     // string WorkOrderNummer
               SortRole,
               DessinRole,
               YearRole,                // int 2022 Jahr
               MonthOfYear,             // int zwischen 1-12 Monath des Jahres
               DateRole,                // nein QDate !! //string '23.02.22'
               WeekdayRole,             // nein Int !! // string : 'Mo' , 'Di'
               DayOfYearRole,           // INT zwischen 1 und 365  Tag des Jahres
               DayOfMonth,              // int zwischen 1 und 31 Tag des Monats
               WeekOfYear,              // int zwischen 1 und 53 Kalenderwoche des Jahres
               WeekOfMonth,             // int
               TimeRole,                // string/boost::icl::discrete_interval '08:30-12:50' zeitinterval wann ein auftrag gescheduled ist
               StartTimeRole,           // string '08:32' wann der time slot des auftrages beginnt
               EndTimeRole,             // string '12:50' wann der time solot des auftrages endet
               IsWorkdayRole,           // boo   ist dieser tag ein arbeitstag oder nicht
              ProdSpeedRole,            // produktions geschwindigkeit für ein produkt in m/min
              IncuredTrashRole,         // the an einem Tag/Woche/Monat angefallene Menge an Ausschuss
              ProdTimeRole,             // produktions zeit für order in 'min', berrechnet aus der QuantityRole
               ConfigIDRole,            // which machine configuration does this order belong to // needed by the QStyledItemDelegate to color the order
               CommentRole              // editierbarer Kommentar z.B. 'Versuch- neues Produkt'
              };

class plan_item
        : public QStandardItem
{
public:
    plan_item(PlanItemTypes);
    plan_item();
    virtual ~plan_item();
    plan_item(int rows,int columns=1); // der default PlanItemType ist OrderType ?!


    // plan_item(PlanItemTypes,int rows,int columns=1);

      // plan_item(PlanItemTypes parentType, PlanItemTypes childType ,int rows,int columns=1);


    plan_item(const QString & text);

    plan_item(PlanItemTypes type, const QString & text);

    virtual  int type() const override;

    // flags() braucht nicht virtuell sein, und auch nicht überschrieben, werden,
    // weil die implementierung von    QStandardItem die virtuelle Methode 'data(UserRole-1)' aufruft
protected:
    void init_flags(PlanItemTypes);

    void setup_day_type();
      void setup_order_type();
        void setup_week_type();
          void setup_month_type();
          void setup_year_type();
};

#endif // HPLAN_ITEM_H
