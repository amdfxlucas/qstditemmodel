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

#endif // SCOPE_TAGGER_H
