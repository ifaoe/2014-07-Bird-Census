#include <QApplication>
#include <qgsapplication.h>
#include "mainwindow.h"
#include "defs.h"
#include "appconfig.h"
#include "sessionselector.h"

int main(int argc, char *argv[])
{
    // Applikation initialisieren
    QApplication app(argc, argv);

    Defs *defaultSettings = new Defs(argc,argv);
    AppConfig* config = new AppConfig(defaultSettings);

    // Einlesen der Datenbankparameter
    Db * db = new Db(config);
    db->initConfig();

    SessionSelector * dialog = new SessionSelector(db, config);
    dialog->exec();
    delete dialog;
    // Qgis Pfad setzen und Provider laden
    QgsApplication::setPrefixPath(config->qgsPrefixPath(), true);
    QgsApplication::initQgis();

    // Applikation starten
    MainWindow win(config, db);
    win.show();
    int result = app.exec();
    win.saveData();

    // Qgis Bibliotheken freigeben
    QgsApplication::exitQgis();

    // Konfiguration freigeben
    delete config;
    delete defaultSettings;
    delete db;


    // Beenden
    return result;
}
