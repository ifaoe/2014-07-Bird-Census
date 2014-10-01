#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <string>
#include <stdexcept>
#include <QtGlobal>
#include <QDebug>
#include <QString>
#include <QMap>
#include <QFileInfo>
#include <libconfig.h++>
#include "defs.h"
#include "sqlquery.h"
#include "sessionselector.h"

using namespace libconfig;

const char ACFG_ERR_MISSING_PARAM[] =
      "Fehlender Parameter %s in Gruppe %s!\n"
      "Datei: %s/ nach Zeile: %d\nParameter: %s";

const char ACFG_ERR_MISSING_PARAM_GROUP[] =
      "Fehlende Parametergruppe %s in Sektion .%s!\n"
      "Datei: %s/ nach Zeile: %d\nParametergruppe: %s";

const char ACFG_ERR_MISSING_QUERY[] =
      "Fehlende Abfrage %s in Gruppe sqlQueries: {...} !\n";

const QString ACFG_SQL_QRY_READ_IMAGES      = "readImages";
const QString ACFG_SQL_QRY_READ_RIMAGE      = "readRawImage";
const QString ACFG_SQL_QRY_READ_RIMAGE_TILE = "readRawImageTile";
const QString ACFG_SQL_QRY_READ_RCENSUS     = "readRawCensus";
const QString ACFG_SQL_QRY_READ_RCENSUS_ADMIN     = "readRawCensusAdmin";
const QString ACFG_SQL_QRY_DEL_RCENSUS      = "deleteRawCensus";
const QString ACFG_SQL_QRY_READ_IMGENV      = "readImgEnvelope";
const QString ACFG_SQL_QRY_READ_FDATA       = "readImgFlightData";
const QString ACFG_SQL_QRY_READ_ID_MAP      = "readIdMapping";
const QString ACFG_SQL_QRY_READ_DONE        = "readImageDone";

const char ACFG_ERR_DOUBLE_KEY[] =
      "Zweideutiger Schluessel %s in Gruppe %s!\n"
      "Datei: %s/ nach Zeile: %d\nTyp: %s";


class AppConfig  {

public:


    AppConfig(const Defs *aDefaultSettings);
    ~AppConfig();

    static void readQString(const Setting& section,
                     const char* key,
                     QString &result,
                     const QString &defValue,
                     const char* help,
                     bool mandatory);

    static void readInteger(const Setting &section, const char* key,
                     int &result, int defValue,
                     const char* help, bool mandatory);

    static void readQuint8(const Setting &section, const char* key,
                     quint8 &result, quint8 defValue,
                     const char* help, bool mandatory);

    static void readQuint16(const Setting &section, const char* key,
                     quint16 &result, quint16 defValue,
                     const char* help, bool mandatory);


    Setting& root() const;

    Setting& readGroup(const Setting
                       &section,
                       const char* key,
                       const char* help) const;

    void replacePrjSettings(QString &src);

    quint16 guiWinWidth() const;
    quint16 guiWinHeight() const;
    quint16 guiWinLeft() const;
    quint16 guiWinTop() const;

    QString symbolFileSld(const QString name) const;
    QString symbolFileQml(const QString name) const;

    QString appTitle() const;
    QString appVersion() const;
    QString appAuthor() const;
    QString appCopy() const;
    QString appUser() const;

    QString qgsPrefixPath() const;
    QString prjPath() const;
    QString prjSession() const;
    quint8  prjUtmSector() const;
    quint8  imgBandRed() const;
    quint16 imgMinRed() const;
    quint16 imgMaxRed() const;
    quint8  imgBandGreen() const;
    quint16 imgMinGreen() const;
    quint16 imgMaxGreen() const;
    quint8  imgBandBlue() const;
    quint16 imgMinBlue() const;
    quint16 imgMaxBlue() const;

    quint16 imgTileWidth() const;
    quint16 imgTileHeight() const;

    SqlQuery* getSqlQuery(QString key) const;

    QStringList getAdmins() const;

private:
    const Defs *defaultSettings;
    Config    cfg;
    Config    prj;
    // -----------------------------------------------------
    QString   qsAppTitle   = TK_QSTR_NONE;
    QString   qsAppVersion = TK_QSTR_NONE;
    QString   qsAppAuthor  = TK_QSTR_NONE;
    QString   qsAppCopy    = TK_QSTR_NONE;
    QStringList usrAdmins;

    // -----------------------------------------------------
    QString   qsQgsPrefixPath = "/usr";

    // -----------------------------------------------------
    QString   qsPrjPath    = TK_QSTR_NONE;
    QString   qsPrjSession = TK_QSTR_NONE;
    quint8    qui8PrjUtmSector = 0;

    // -----------------------------------------------------
    quint16 qui16ImgMinRed = 1;
    quint16 qui16ImgMaxRed = 65535;
    quint8  qui8ImgBandRed = 1;
    quint16 qui16ImgMinBlue = 1;
    quint16 qui16ImgMaxBlue = 65535;
    quint8  qui8ImgBandBlue = 3;
    quint16 qui16ImgMinGreen = 1;
    quint16 qui16ImgMaxGreen = 65535;
    quint8  qui8ImgBandGreen = 2;

    quint16 qui16ImgTileWidth  = 800;
    quint16 qui16ImgTileHeight = 800;
    // -----------------------------------------------------
    quint16 winWidth = 1000;
    quint16 winHeight = 1000;
    quint16 winLeft = 1;
    quint16 winTop  = 1;

    QMap<QString, SqlQuery*> sqlQueries;
    void readQueries(const Setting &section);
};

#endif // APPCONFIG_H
