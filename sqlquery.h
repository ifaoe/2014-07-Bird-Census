#ifndef SQLQUERY_H
#define SQLQUERY_H

#include "defs.h"
#include <QString>
#include <QStringList>

class SqlQuery  {
public:
    SqlQuery(const QString &aQuery, const QString &aDesc);
    QString query = TK_QSTR_NONE;
    QString desc  = TK_QSTR_NONE;
};

#endif // SQLQUERY_H
