#ifndef REFERENCE_CONTROLLER_H
#define REFERENCE_CONTROLLER_H

#include <QModelIndex>
#include "path.h"
#include "qstditem.h"

#include <QMultiHash>

class QStdItem;


class reference_controller;

class reference
 : public QObject
{Q_OBJECT

public  slots:

signals:

    void validReference(reference*);


    void invalidReference(reference*);

    // emited by SetModelCmd
  //  void path_available_for_item(Path p,QStdItem* item);

    //void path_available_for_item(Path p, unsigned long long uuid);

    //void IndexAvailableForItem(QModelIndex,QStdItem*);


     //void IndexAvailableForItem(QModelIndex,unsigned long long uuid);

    // vielleicht bei removeRow /Column emittiert
//    void IndexInvalidated(QModelIndex)    ;
 //   void IndexInvalidated(unsigned long long uuid)    ;



    // a Command requests in its destructor for its reference to be deleted
    // the controller may now dispose of it
//    void CommandAboutToBeDeleted(QStdItem::StdItemCmd*);

private:
    bool valid;

    reference_controller* const  m_controller;

    // who references the item
    QStdItem::StdItemCmd* const  m_cmd;



    //which item is referenced
    unsigned long long item_uuid;
    QModelIndex m_index;
    QStdItem* m_item{nullptr};
    QStdItemModel* m_model{nullptr};
    Path m_path;
    friend class reference_controller;
    friend class QStdItem::StdItemCmd;
    friend uint qHash(const reference& r);


    // friend bool operator==(const reference& , const reference& );

    reference();
    reference(QModelIndex,QStdItem*,QStdItem::StdItemCmd*);
    reference(QStdItem*,QStdItem::StdItemCmd*, reference_controller* );
    // only the reference_controler is allowed to construct references

public:
bool operator==(const reference& ) const;

bool referToSameItem(const reference& )const;


// wenn diese funktionen nicht public sind kommen die anderen kommandos nicht mehr an sie heran, weil friendship nicht vererbt wird

// the reference_controller as well as the Commands can manipulate the reference through its setters
void setPath(const Path&);

// other commands that referenced the same Item 'it' ( with the same uuid )
// will benefit from
void setItem(QStdItem* it);
void setModel(QStdItemModel*);
void setValid(bool);
void setIndex(const QModelIndex&);



};

uint qHash(const reference& );


inline uint qHash(const reference& r)
{
    return reinterpret_cast<std::uintptr_t>(r.m_cmd);
}




class reference_controller
      : public QObject
{Q_OBJECT
public slots:

    void free_uuid(unsigned long long uuid);

    void validReference(reference*);

    void invalidReference(reference*);

  //  void ReceiveIndexForItem(QModelIndex,QStdItem*);

public:
int cmd_count()const;
void clear();

    reference* new_reference(QStdItem* i, QModelIndex idx, QStdItem::StdItemCmd* cmd);
     reference* new_reference(QStdItem* i,  QStdItem::StdItemCmd* cmd);

    static reference_controller* get_instance()
    {return &_instance_;}

    void lock(bool lck);

public slots:
    void modelDestroyed(QObject*); // frees all references into this model

private:

    bool locked;

    static reference_controller _instance_;
    reference_controller();

    // oder das hier, QList ist auch Hashable
    //     QMultiHash<Path,reference> m_refs;


    // cool wäre natürlich, wenn jedes item eine global eindeutige UUID hätte
     QMultiHash<unsigned long long, reference*> m_ref;

    // QMultiHash<QModelIndex,reference> m_refs;
    // alle referenzen, die den gleichen ModelIndex referenzieren, landen in einem bucket

};

#endif // REFERENCE_CONTROLLER_H
