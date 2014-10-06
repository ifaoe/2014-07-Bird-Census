#ifndef DEFS_H
#define DEFS_H

#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QProcessEnvironment>

const char TK_CSTR_NONE[] = "**NONE**";
const QString TK_QSTR_NONE = "**NONE**";
const char KEY_IFAOE_BIRD_CENSUS[] = "IFAOE_BIRD_CENSUS_V01";
const QString ENV_IFAOE_BIRD_CENSUS = "ENV_IFAOE_BIRD_CENSUS";
const QString ROOT_IFAOE_BIRD_CENSUS = ".bird-census.d";
const QString PRJ_DIR_BIRD_CENSUS = "/usr/local/ifaoe/Settings/Daisi/.bird-census.d/Projekte/";
const QString KEY_BIRD_SWIM = "VS";
const QString KEY_BIRD_FLY = "VF";
const QString KEY_MAMMAL = "MM";
const QString KEY_WAVE = "WV";
const QString KEY_SUN = "SN";
const QString KEY_UFO = "UO";
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
    QString getEtcPath() const;
    QString getSymbolPath() const;
    QString getConfig() const;

private:
    QStringList args;
    QString mEnv  = TK_QSTR_NONE;
    QString mHome = TK_QSTR_NONE;
    QString mUser = TK_QSTR_NONE;
    QString mRoot = TK_QSTR_NONE;
    QFileInfo fRoot;
    QFileInfo fEtc;
    QFileInfo fSymbol;
    QFileInfo fConf;
};

#endif // DEFS_H
