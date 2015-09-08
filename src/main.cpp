#include <QApplication>
#include <qgsapplication.h>
#include "mainwindow.h"
#include "ConfigHandler.h"
#include "ServerSelection.h"
#include <QInputDialog>

int main(int argc, char *argv[])
{
    // Applikation initialisieren
   	QCoreApplication::setOrganizationName("ifaoe");
   	QCoreApplication::setOrganizationDomain("ifaoe.de");
   	QCoreApplication::setApplicationName("daisi-bird-census");
	QIcon::setThemeName("gnome");
	QStringList theme_paths;
	theme_paths << "/usr/share/icons/";
	QIcon::setThemeSearchPaths(theme_paths);
    QApplication app(argc, argv);
    QFile file(":qdarkstyle/style.qss");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        app.setStyleSheet(file.readAll());
        file.close();
    }

    ConfigHandler* config = new ConfigHandler();
    config->InitSettings();

    // Einlesen der Datenbankparameter
    Db * db = new Db(config);

    // Qgis Pfad setzen und Provider laden
    QgsApplication::setPrefixPath(config->getQGisPrefixPath(), true);
    QgsApplication::initQgis();

    if (config->getPreferredDatabase().isEmpty() || !config->getDatabaseList().contains(config->getPreferredDatabase())) {
		ServerSelection server_selection(config);
		server_selection.exec();
		if (server_selection.result() == QDialog::Rejected)
			return 1;
    }
    db->OpenDatabase();

    // Applikation starten
    MainWindow win(config, db);
    if(config->getAppMaximized())
    	win.showMaximized();
	else
    	win.show();
    int result = app.exec();
    win.saveData();

    // Qgis Bibliotheken freigeben
    QgsApplication::exitQgis();

    // Konfiguration freigeben
    delete config;
    delete db;


    // Beenden
    return result;
}
