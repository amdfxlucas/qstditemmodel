#include "reference_controller.h"
#include "qstditemmodel.h"
#include "qstditemmodel_p.h"


bool reference::operator==(const reference& ref)const
{
    // two references are identical, if they belong to the same command
  return m_cmd==ref.m_cmd;
}

int reference_controller::cmd_count()const
{
    return m_ref.size();
}

void reference_controller::clear()
{
    m_ref.clear();
}

bool reference:: referToSameItem(const reference& ref)const
{
    // two references compare equal, if they reference the same item in the model
    // eigther by pointer , path or model index


    return (m_item->uuid()==ref.m_item->uuid() )  && this!= &ref;
}

/*
inline bool operator==(const reference& a, const reference& b)
{
    return a.operator==(b);
}*/

void reference_controller::lock(bool lck)
{
    locked=lck;
}

void reference_controller::modelDestroyed(QObject* m)
{
    /*
    auto deleted_model{dynamic_cast<QStdItemModel*>(m)};

    auto i=m_ref.begin();
    auto end{m_ref.end()};

    while(i!=end )
    {
        if( i.value() ->m_model == deleted_model  )
        {
           auto count{m_ref.remove(i.key(),i.value())};
           Q_ASSERT(count==1);
        }
        ++i;
    }
    */

}

uint qHash(unsigned long long i)
{
    return i;
}

 reference_controller reference_controller:: _instance_{};

reference::reference(QStdItem* i,
                     QStdItem::StdItemCmd* cmd,
                     reference_controller* const ctrl)
    : m_controller(ctrl),
      m_cmd(cmd),
      m_item(i),
      QObject(ctrl) // make the reference a child of the reference_controller
                    // references can not leak this way
{
    item_uuid=i->uuid();

    if(i->model())
    {
        m_model =i->model();

        if(m_model->contains(item_uuid))
        {   m_index= i->index();
            m_path = QStdItemModel::pathFromIndex(m_index);
        }
    }

    connect(this,&reference::validReference,
            m_controller, &reference_controller::validReference);

    connect(i->d_func(),&QStdItemPrivate::free_uuid,
            ctrl,&reference_controller::free_uuid);

}

void reference_controller::free_uuid(unsigned long long uuid)
{
    // QStdItemPrivate emits 'free_uuid(unsigned long long uuid)' signal in its Destructor
    // what causes reference_controller to delete all references, which refer to this uuid
    m_ref.remove(uuid);
}

reference_controller::reference_controller()
    : QObject()
{

}

void reference_controller::invalidReference(reference* ref)
{
    // removes an unneded reference after ~StdItemCmd was called


    if(!locked)
    {
    auto c{m_ref.remove(ref->item_uuid,ref) };
    }

}

reference* reference_controller::new_reference(QStdItem *i, QModelIndex idx, QStdItem::StdItemCmd *cmd)
{





    return   (*m_ref.insert( i->uuid(), new reference(i,cmd,this)) );
}


reference* reference_controller::new_reference(QStdItem *i,  QStdItem::StdItemCmd *cmd)
{


 return   (*m_ref.insert( i->uuid(),new reference(i,cmd,this) ));
}

void reference_controller::validReference(reference* ref)
{
    if(locked){return;}

    // precondition: the reference 'ref' must be valid !
    // its path, index, model, and item-pointer

    auto uid{ref->item_uuid};


    auto update_cmd_refs = [&r= *ref](reference* cmd_ref)
                            { // both references r , cmd_ref reference the same item  !
                              // but belong to different commands
                                cmd_ref->m_model=r.m_model;
                                cmd_ref->m_index=r.m_index;
                                cmd_ref->m_path=r.m_path;
                                cmd_ref->m_item=r.m_item;

    };

    // only update those references whose commands reference the same item-uuid

 /* auto i = m_ref.find(uid);

    while (i != m_ref.end() && i.key() == uid)
    {
        update_cmd_refs( i.value());
        i++;
    }
    */



    // das ist langsamer, aber m??sste eigentlich alle erwischen
    auto i=m_ref.begin();
    auto end{m_ref.end()};

    while(i!=end )
    {
        if( i.value() ->referToSameItem( *ref)  )
        {
            update_cmd_refs(i.value() );
        }
        ++i;
    }

}

void reference::setModel(QStdItemModel* m)
{
    m_model=m;
    m_index = m->indexFromItem(m_item);
    m_path= QStdItemModel::pathFromIndex(m_index);

    emit validReference(this);
    // notify the reference_controller,
    // which then updates any other commands references
    // which might also reference the same item (with this uuid )
}

void reference::setPath(const Path&)
{

}

void reference::setItem(QStdItem*)
{

}

void reference::setIndex(const QModelIndex&)
{

}
