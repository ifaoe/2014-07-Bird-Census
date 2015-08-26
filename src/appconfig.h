#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <string>
#include <stdexcept>
#include <QtGlobal>
#include <QString>
#include <QMap>
#include <QFileInfo>
#include <libconfig.h++>
#include "defs.h"
#include <qgsvectorlayer.h>


using namespace libconfig;

const char ACFG_ERR_MISSING_PARAM[] =
      "Fehlender Parameter %s in Gruppe %s!\n"
      "Datei: %s/ nach Zeile: %d\nParameter: %s";

const char ACFG_ERR_MISSING_PARAM_GROUP[] =
      "Fehlende Parametergruppe %s in Sektion .%s!\n"
      "Datei: %s/ nach Zeile: %d\nParametergruppe: %s";

const char ACFG_ERR_MISSING_QUERY[] =
      "Fehlende Abfrage %s in Gruppe sqlQueries: {...} !\n";

//const QString ACFG_SQL_QRY_READ_IMAGES =
//		"SELECT tp, px, py, ux, uy, lx, ly, rcns_id FROM raw_census WHERE session ='$(session)' "
//		"AND cam  = '%1' AND img = '%2' AND usr = '%3' AND epsg = $(utmSector) order by rcns_id";
const QString ACFG_SQL_QRY_READ_RIMAGE =
		"SELECT rimg_id, tm_when, tm_seen FROM raw_images WHERE session ='$(session)'"
		" AND cam  = '%1'  AND img = '%2' AND usr = '%3' AND epsg  = $(utmSector)";
const QString ACFG_SQL_QRY_READ_RIMAGE_TILE =
		"SELECT rtls_id, ux, uy, w, h, tm_when, tm_seen FROM raw_tiles WHERE session ='$(session)'"
		" AND cam  = '%1' AND img = '%2' AND usr = '%3' AND epsg  = $(utmSector)";
const QString ACFG_SQL_QRY_READ_RCENSUS = "SELECT rcns_id, usr, tp, ux, uy, lx, ly  FROM raw_census WHERE session ='$(session)' "
		"AND cam  = '%1' AND img = '%2' AND usr='%3' AND epsg = $(utmSector) order by rcns_id";
const QString ACFG_SQL_QRY_READ_RCENSUS_ADMIN =
		"SELECT rcns_id, usr, tp, ux, uy, lx, ly FROM raw_census WHERE session ='$(session)' "
		"AND cam  = '%1' AND img = '%2' AND epsg = $(utmSector) order by rcns_id";
const QString ACFG_SQL_QRY_DEL_RCENSUS      =
		"DELETE FROM raw_census WHERE rcns_id =%1 AND session='$(session)' AND cam='%2' AND img='%3'";
const QString ACFG_SQL_QRY_READ_IMGENV      =
		"SELECT st_astext(geo_rc_bb) as bb, st_astext(geo_rc_sb) as sb, cam1_cfg as cfg1, "
		"cam2_cfg as cfg2 FROM sync_utm$(utmSector) WHERE cam%1_id = '%2' AND session = '$(flight)'";
const QString ACFG_SQL_QRY_READ_FDATA       =
		"SELECT lon, lat, cog, gps_dt, gps_tm FROM sync_utm$(utmSector) WHERE cam%1_id = '%2' "
		"AND session = '$(flight)'";
const QString ACFG_SQL_QRY_READ_SDATA       =
		"SELECT lon, lat, cog, gps_dt, gps_tm FROM sync_utm$(utmSector) WHERE session = '$(flight)'"
		" AND gps_trc = %1 AND $(filter)";
const QString ACFG_SQL_QRY_READ_ID_MAP      =
		"SELECT sync_id, cam1_id, cam2_id, gps_trc FROM sync_utm$(utmSector) WHERE %1 = '%2' "
		"AND session = '$(flight)'";
const QString ACFG_SQL_QRY_READ_DONE        =
		"SELECT distinct(img) FROM raw_images WHERE cam = '%1' AND session = '$(session)' AND rdy=1";
const QString ACFG_SQL_QRY_READ_IMAGES =
		"SELECT trc, cam, img FROM image_properties WHERE project_list @> ARRAY['$(session)'] AND %1 "
		"ORDER BY trc, cam, img";
const QString ACFG_SQL_QRY_READ_IMAGES_NOT_READY =
		"SELECT i.trc, i.cam, i.img, r.rdy FROM "
		"(SELECT trc, cam, img FROM image_properties WHERE project_list @> ARRAY['$(session)'] )as i "
		"LEFT JOIN (SELECT cam, img, rdy FROM raw_images WHERE session='$(session)' AND rdy=1) as r "
		"ON i.cam=r.cam AND i.img=r.img "
		"WHERE %1 AND r.rdy IS NULL ORDER BY i.trc, i.cam, i.img";
const QString ACFG_SQL_QRY_READ_VALIDPOLY =
		"SELECT ST_AsText(cut_env) FROM image_properties WHERE cam='%1' AND img='%2' AND session = '$(flight)'";

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

    QString replacePrjSettings(const QString src);

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
    QString prjType() const;
    QString prjFlight() const;
    QString prjFilter() const;
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

    QStringList getAdmins() const;

    void setPrjPath(QString path);
    void setPrjSession(QString session);
    void setPrjFlight(QString flight);
    void setPrjFilter(QString filter);
    void setPrjType(QString type);
    void setPrjUtmSector(quint8 utmSetor);

    void readQueries();

    QString curCam;
    QString curImg;

private:
    const Defs *defaultSettings;
    Config user_config;
    Config    system_config;
    Config	  query_config;
    // -----------------------------------------------------
    QString   qsAppTitle;
    QString   qsAppVersion;
    QString   qsAppAuthor;
    QString   qsAppCopy;
    QStringList usrAdmins;

    // -----------------------------------------------------
    QString   qsQgsPrefixPath = "/usr";

    // -----------------------------------------------------
    QString   qsPrjPath;
    QString   qsPrjSession;
    QString   qsPrjFlight;
    QString   qsPrjFilter;
    QString   qsPrjType;
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
};

#endif // APPCONFIG_H