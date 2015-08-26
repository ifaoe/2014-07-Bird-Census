#include "appconfig.h"
#include "QgsMapMarker.h"

AppConfig::AppConfig(const Defs *aDefaultSettings) :
    defaultSettings(aDefaultSettings)
   {


    // Lesen und parsen der Konfiguration
    const char* cfgFile = defaultSettings->getConfig().toStdString().c_str();

    try {
        system_config.readFile( CONFIG_IFAOE_BIRD_CENSUS.toStdString().c_str());
    } catch(const FileIOException &fioex)  {
        qFatal("Fehler in Konfiguration %s!", cfgFile );
    } catch(const ParseException &pex) {
        qFatal("Fehler in Konfiguration %s in Zeile %d\n Details: %s! ",
            pex.getFile(), pex.getLine(), pex.getError());
    }

    try {
        query_config.readFile( QUERY_IFAOE_BIRD_CENSUS.toStdString().c_str() );
    } catch(const FileIOException &fioex)  {
        qFatal("Fehler in Konfiguration %s!", QUERY_IFAOE_BIRD_CENSUS.toStdString().c_str() );
    } catch(const ParseException &pex) {
        qFatal("Fehler in Konfiguration %s in Zeile %d\n Details: %s! ",
            pex.getFile(), pex.getLine(), pex.getError());
    }

    try {
        user_config.readFile( defaultSettings->getConfig().toStdString().c_str() );
    } catch(const FileIOException &fioex)  {
        qFatal("Fehler in Konfiguration %s!", QUERY_IFAOE_BIRD_CENSUS.toStdString().c_str() );
    } catch(const ParseException &pex) {
        qFatal("Fehler in Konfiguration %s in Zeile %d\n Details: %s! ",
            pex.getFile(), pex.getLine(), pex.getError());
    }

    // @TODO Include directory setzen
    // const char* cfgInclude = defaultSettings->getEtc().toStdString().c_str();
    // cfg.setIncludeDir( cfgInclude );
    const Setting& system_config_root = system_config.getRoot();

    // Einlesen der Grundparameter
    readQString(system_config_root, "title", qsAppTitle, "**NONE**",
                "Name des Programms", true);
    readQString(system_config_root, "version", qsAppVersion, "0.0",
                "Versionsnummer des Programms - bug tracking", true);
    readQString(system_config_root, "author",  qsAppAuthor, "**NOBODY**",
                "Kontaktdaten Programmauthor, in der Regel die E-Mail" , true);
    readQString(system_config_root, "copyright",  qsAppCopy, "**NOBODY**",
                "Kopierrechte" , true);

    QString tmpAdmins;
    readQString(system_config_root, "admins", tmpAdmins, "", "Administatoren", true);
    usrAdmins = tmpAdmins.split(",");

    // Einlesen der GUI Parameter
    const Setting & user_config_root = user_config.getRoot();
    const Setting& gui = readGroup(user_config_root,"gui", "Gruppe der GUI Einstellungen");

    const Setting& guiWin = readGroup(gui, "win",  "Gruppe der GUI Fenstereinstellungen");

    readQuint16(guiWin,"width",  winWidth,  800, "Fensterbreite", true);
    readQuint16(guiWin,"height", winHeight, 600, "FensterHoehe", true);
    readQuint16(guiWin,"left",   winLeft,     0, "Fensterposition Links", true);
    readQuint16(guiWin,"top",    winTop,      0, "Fensterposition Oben", true);

    // Einlesen der Grundparameter
    const Setting& qgis = readGroup(system_config_root, "qgis",
                                   "Gruppe der GGIS Einstellungen");

    readQString(qgis,"prefixPath",  qsQgsPrefixPath, "/usr",
             "Pfadprefix fuer die Qgis-Installation LINUX /usr oder /usr/local",
             true);

    const Setting& image = readGroup(user_config_root, "image", "Bildeinstellungen");
    readQuint8(image, "bandRed",     qui8ImgBandRed, 1, "Index roter Kanal", true);
    readQuint8(image, "bandBlue",    qui8ImgBandBlue, 2, "Index blauer Kanal", true);
    readQuint8(image, "bandGreen",  qui8ImgBandGreen, 3, "Index griener Kanal", true);
    readQuint16(image, "redMin",  qui16ImgMinRed, 3, "Minimaler Wert roter Kanal", true);
    readQuint16(image, "blueMin",  qui16ImgMinBlue, 3, "Minimaler Wert blauer Kanal", true);
    readQuint16(image, "greenMin",  qui16ImgMinGreen, 3, "Minimaler Wert gruener Kanal", true);
    readQuint16(image, "redMax",  qui16ImgMaxRed, 3, "Maximaler Wert roter Kanal", true);
    readQuint16(image, "blueMax",  qui16ImgMaxBlue, 3, "Maximaler Wert blauer Kanal", true);
    readQuint16(image, "greenMax",  qui16ImgMaxGreen, 3, "Maximaler Wert gruener Kanal", true);
    readQuint16(image, "tileWidth",  qui16ImgTileWidth, 800, "Breite der Image Tiles", true);
    readQuint16(image, "tileHeight",  qui16ImgTileHeight, 800, "Hoehe der Image Tiles", true);




}

// -------------------------------------------------------------------------
QString AppConfig::symbolFileSld(const QString name) const {
    return defaultSettings->getSymbolPath()+"/"+name+".sld";
}
// -------------------------------------------------------------------------
QString AppConfig::symbolFileQml(const QString name) const {
    return defaultSettings->getSymbolPath()+"/"+name+".qml";
}


// -------------------------------------------------------------------------
AppConfig::~AppConfig() {

}


quint16 AppConfig::guiWinWidth() const { return winWidth; }
quint16 AppConfig::guiWinHeight() const { return winHeight; }
quint16 AppConfig::guiWinLeft() const { return winLeft; }
quint16 AppConfig::guiWinTop() const { return winTop; }

QString AppConfig::appTitle() const { return qsAppTitle; }
QString AppConfig::appVersion() const { return qsAppVersion; }
QString AppConfig::appAuthor() const { return qsAppAuthor; }
QString AppConfig::appCopy() const { return qsAppCopy; }
QString AppConfig::appUser() const { return defaultSettings->getUser(); }

QString AppConfig::qgsPrefixPath() const { return qsQgsPrefixPath; }
QString AppConfig::prjPath() const { return qsPrjPath; }
QString AppConfig::prjSession() const { return qsPrjSession; }
QString AppConfig::prjType() const { return qsPrjType; }
QString AppConfig::prjFlight() const { return qsPrjFlight; }
QString AppConfig::prjFilter() const { return qsPrjFilter; }
quint8 AppConfig::prjUtmSector() const { return qui8PrjUtmSector; }

quint16 AppConfig::imgTileWidth() const { return qui16ImgTileWidth; }
quint16 AppConfig::imgTileHeight() const { return qui16ImgTileHeight; }

quint8 AppConfig::imgBandRed() const { return qui8ImgBandRed; }
quint16 AppConfig::imgMinRed() const { return qui16ImgMinRed; }
quint16 AppConfig::imgMaxRed() const { return qui16ImgMaxRed; }

quint8 AppConfig::imgBandGreen() const { return qui8ImgBandGreen; }
quint16 AppConfig::imgMinGreen() const { return qui16ImgMinGreen; }
quint16 AppConfig::imgMaxGreen() const { return qui16ImgMaxGreen; }

quint8 AppConfig::imgBandBlue() const { return qui8ImgBandBlue; }
quint16 AppConfig::imgMinBlue() const { return qui16ImgMinBlue; }
quint16 AppConfig::imgMaxBlue() const { return qui16ImgMaxBlue; }


Setting& AppConfig::root() const {  return system_config.getRoot(); }

// -------------------------------------------------------------------------
/**
 * @brief AppConfig::readQueries
 * @param section
 * @todo move int SqlQuery class
 */
void AppConfig::readQueries() {
	const Setting& cfgRoot = query_config.getRoot();
    const Setting& queries = readGroup(cfgRoot, "sqlQueries",
                                       "Gruppe der SQL Abfragen");
    const char* HELP_TMPL = "Schablone fuer die SQL-Abfrage";
    const char* HELP_DESC = "Beschreibung der SQL-Abfrage";

    int len = queries.getLength();
    for (int i=0; i < len; ++i) {
        const Setting& set = queries[i];
        QString key = QString(set.getName());
        qDebug() << "Key: " << key;
        QString query, desc;
        readQString(set, "query", query, "", HELP_TMPL, true);
        readQString(set, "help", desc, "", HELP_DESC, true);
        replacePrjSettings(query);
    }
}

// -------------------------------------------------------------------------
QString AppConfig::replacePrjSettings(const QString str) {
    QString prj = QString::number(prjUtmSector());
    QString src = str;
    src.replace("$(utmSector)",prj);
    src.replace("$(path)",prjPath());
    src.replace("$(session)",prjSession());
    src.replace("$(flight)",prjFlight());
    src.replace("$(filter)",prjFilter());
    return src;
}


// -------------------------------------------------------------------------
Setting& AppConfig::readGroup(const Setting &section,
                              const char* key, const char* help) const {
    if (! section.exists(key) ) {
        qFatal(ACFG_ERR_MISSING_PARAM_GROUP, key, section.getPath().c_str(),
               section.getSourceFile(), section.getSourceLine(), help);
    }
    return section[key];
}

// -------------------------------------------------------------------------
void AppConfig::readQString( const Setting& section, const char* key,
                             QString &result, const QString &defValue,
                             const char *help, bool mandatory = true) {
        std::string dummy="";
        if (section.lookupValue(key, dummy)) {
          result=QString::fromStdString(dummy);
        } else {
            if (mandatory) {
                qFatal(ACFG_ERR_MISSING_PARAM, key, section.getPath().c_str(),
                       section.getSourceFile(), section.getSourceLine(),
                       help);
            } else {
                result=defValue;
            }
        }
}

// -------------------------------------------------------------------------
void AppConfig::readInteger(const Setting& section, const char* key,
                             int &result, int defValue,
                             const char *help, bool mandatory = true) {
    if (!section.lookupValue(key, result)) {
        if (mandatory) {
            qFatal(ACFG_ERR_MISSING_PARAM, key, section.getPath().c_str(),
                   section.getSourceFile(), section.getSourceLine(),
                   help);
        } else {
            result=defValue;
        }
    }
}

// -------------------------------------------------------------------------
void AppConfig::readQuint8(const Setting& section, const char* key,
                             quint8 &result, quint8 defValue,
                             const char *help, bool mandatory = true) {

    qint32 aInt;
    readInteger( section, key, aInt, defValue, help, mandatory);
    result = qBound(0,aInt,255);
}

// -------------------------------------------------------------------------
void AppConfig::readQuint16(const Setting& section, const char* key,
                             quint16 &result, quint16 defValue,
                             const char *help, bool mandatory = true) {

    qint32 aInt;
    readInteger( section, key, aInt, defValue, help, mandatory);
    result = qBound(0,aInt,65535);
}

QStringList AppConfig::getAdmins() const {
    return usrAdmins;
}

/*
 * set methods
 */

void AppConfig::setPrjSession(QString session) { qsPrjSession = session; }
void AppConfig::setPrjFlight(QString flight) { qsPrjFlight = flight; }
void AppConfig::setPrjFilter(QString filter) { qsPrjFilter = filter; }
void AppConfig::setPrjPath(QString path) { qsPrjPath = path; }
void AppConfig::setPrjType(QString type) { qsPrjType = type; }
void AppConfig::setPrjUtmSector(quint8 utmSector) { qui8PrjUtmSector = utmSector; }
