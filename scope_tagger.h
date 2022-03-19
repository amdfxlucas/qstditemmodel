#ifndef SCOPE_TAGGER_H
#define SCOPE_TAGGER_H

#include <QString>
#include <QDebug>


class scope_tagger
{
private: QString m_str;
public: scope_tagger(const QString& str): m_str(str){qDebug() << "<"+str +">";};

    ~scope_tagger(){qDebug()<< "</" +m_str+ ">"; }
};


template <class Callable1,class Callable2>
class on_scope_exit
{
private:

    Callable1 _lbd_enter;
    Callable2 _lbd_exit;

public:
    on_scope_exit(Callable1&& lambda01,Callable2&& lambda02)
        : _lbd_enter(std::forward<Callable1>(lambda01)),
          _lbd_exit(std::forward<Callable2>(lambda02))
    {
        _lbd_enter();
    }
    ~on_scope_exit()
    {
        _lbd_exit();
    }
};

#endif // SCOPE_TAGGER_H
