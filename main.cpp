#include <QApplication>
#include <qgsapplication.h>
#include "mainwindow.h"
#include "defs.h"

int main(int argc, char *argv[])
{
    // Applikation initialisieren
    QApplication app(argc, argv);

    Defs *defaultSettings = new Defs(argc,argv);
    AppConfig* config = new AppConfig(defaultSettings);

    // Qgis Pfad setzen und Provider laden
    QgsApplication::setPrefixPath(config->qgsPrefixPath(), true);
    QgsApplication::initQgis();

    // Applikation starten
    MainWindow win(config);
    win.show();
    int result = app.exec();
    win.saveData();

    // Qgis Bibliotheken freigeben
    QgsApplication::exitQgis();

    // Konfiguration freigeben
    delete config;
    delete defaultSettings;

    // Beenden
    return result;
}
