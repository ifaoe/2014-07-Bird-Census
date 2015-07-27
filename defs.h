#ifndef DEFS_H
#define DEFS_H

#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QProcessEnvironment>

const char KEY_IFAOE_BIRD_CENSUS[] = "IFAOE_BIRD_CENSUS_V01";
const QString ENV_IFAOE_BIRD_CENSUS = "ENV_IFAOE_BIRD_CENSUS";
const QString ROOT_IFAOE_BIRD_CENSUS = "/usr/local/ifaoe/settings/daisi/birdcensus";
const QString QUERY_IFAOE_BIRD_CENSUS = "/usr/local/ifaoe/settings/daisi/birdcensus/queries.cfg";
const QString CONFIG_IFAOE_BIRD_CENSUS = "/usr/local/ifaoe/settings/daisi/birdcensus/applikation.cfg";
const char FMT_CNS_LIST[] = "%2s %05d %05d %08.2f %08.2f %09.6f %9.6f %08d IX%d";
const double SELECT_DIST = 0.10;
const bool ASK_MODIFY = false;

class Defs
{
public:
    Defs(int argc, char *argv[]);
    QString getRoot() const;
    QString getHome() const;
    QString getUser() const;
    QString getSymbolPath() const;
    QString getConfig() const;

private:
    QStringList args;
    QString mEnv;
    QString mHome;
    QString mUser;
    QString mRoot;
    QFileInfo fRoot;
    QFileInfo fEtc;
    QFileInfo fSymbol;
    QFileInfo fConf;
};

#endif // DEFS_H
