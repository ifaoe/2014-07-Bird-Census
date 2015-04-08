#ifndef DB_H
#define DB_H

#include <libconfig.h++>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QString>
#include <QStringList>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QWidgetItem>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QStringListModel>
#include <qgsvectorlayer.h>
#include <qgsvectordataprovider.h>
#include <qgsfield.h>
#include <qgsgeometry.h>
#include <qgspoint.h>
#include <qgsfeature.h>
#include "appconfig.h"
#include <ctime>

typedef struct std::tm time_struct;
struct project;

const char DB_ERR_CFG_DRIVER[] =
              "Fehlerhafter Datenbanktreiber %s + %s in Gruppe %s!\n"
              "Datei: %s/ nach Zeile: %d";

const char DB_ERR_OPEN_DB[] =
              "Datenbank %s \n kann nicht geoeffnet werden!\n"
              "Details: %s";

const char DB_ERR_OPEN_PRJ[] =
              "Projektdaten koennen nicht geoffnet werden!\n"
              "Detail: %s";

const QString DB_CFGIO_DEF_EMPTY= "**NONE**";

const char DB_CFGIO_KEY_DRVN[]  = "driver";
const QString DB_CFGIO_DEF_DRVN = "QPSQL";
const char DB_CFGIO_HLP_DRVN[]  = "Datenbank-TreiberHost (QPSQL)";

const char DB_CFGIO_KEY_HOST[] = "host";
const char DB_CFGIO_DEF_HOST[] = "localhost";
const char DB_CFGIO_HLP_HOST[] = "Datenbank-Host (localhost)";

const char DB_CFGIO_KEY_NAME[] = "name";
const char DB_CFGIO_HLP_NAME[] = "Datenbankname (**NONE**)";

const char DB_CFGIO_KEY_USER[] = "user";
const char DB_CFGIO_HLP_USER[] = "Datenbanknutzer (**NONE**)";

const char DB_CFGIO_KEY_PASS[] = "pass";
const char DB_CFGIO_HLP_PASS[] = "Passwort (**NONE**)";

const char DB_CFGIO_KEY_PORT[] = "port";
const int  DB_CFGIO_DEF_PORT   = 5432;
const char DB_CFGIO_HLP_PORT[] = "Portnummer (5432)";

class Db {
public:
    explicit Db(AppConfig *config);

    void initConfig();

    bool getImages(QTableWidget *result, QString type);

    void readRawCensus(QTableWidget * tbl,
    		const QString cam, const QString img, const QString user);

    bool deleteRawCensus(int id, QString cam, QString img);

    QSet<QString> * readImageDone(const QString cam);

    bool writeImageDone(const int imgRdy, const int id);

    bool writeRawCensus(const QString type,
            const int epsg,
            const QString cam,
            const QString img,
            const QString user,
            const QString session,
			  const QString px, const QString py,
			  const QString ux, const QString uy,
			  const QString lx, const QString ly);

    int getRecentId();

    bool writeRawImage(const bool insert, const int id,
                       const quint8 epsg, const  QString cam, const QString file,
                       const QString usr, const QString session,
                       const QString tmWhen, const QString tmSeen);

    bool writeRawImageTile(    const bool insert, const int id,
                               const quint8 epsg, const QString cam, const QString file,
                               const QString usr, const QString session,
                               const QString x, const QString y,
                               const QString w, const QString h,
                               const QString tmWhen, const QString tmSeen);

    bool readRawImage(const quint8 epsg, const QString cam, const QString file,
                      const QString usr, const QString session,
                      int &id, QString &tmWhen, QString &tmSeen);

    bool readRawImageTile(const quint8 epsg, const QString cam, const QString file,
                          const QString usr, const QString session,
                          int &id,
                          QString &ux, QString &uy, QString &w, QString &h,
                          QString &tmWhen, QString &tmSeen);

    QgsGeometry* readImageEnvelope(const QString cam, const QString image);
    
    int readIdMapping(int * sync_int, QString * cam1_img, QString * cam2_img);

    double getSolarAzimuth(const QString cam, const QString image);

    double getTracAzimuth(const QString trc);

    QStringList getSessionList();

    QStringList getCamList(QString session);

    QStringList getTrcList(QString session);

    project * getSessionParameters(QString session);

private:
    AppConfig *config;
    const char *sectionKey;
    const char *sectionHelp;
    QString driver = DB_CFGIO_DEF_DRVN;
    QString name   = "daisi";
    QString host   = "localhost";
    int port       = DB_CFGIO_DEF_PORT;
    QString user   = TK_QSTR_NONE;
    QString pass   = TK_QSTR_NONE;
    QString uri    = TK_QSTR_NONE;
    QSqlDatabase db;
};

typedef struct project {
	QString project_id = "";
	QString flight_id = "";
	int utmSector = 32;
	QString path = "";
	QString filter = "true";
	QString session_type = "full";
} project;

#endif // DB_H
