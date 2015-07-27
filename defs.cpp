#include "defs.h"

Defs::Defs(int argc, char *argv[]) {

	QString user_config_file = "";

	for (int i = 0; i < argc; i++) {
		QString qArg = QString::fromAscii(argv[i]);
		args.append(qArg);
		if (qArg == "-c" || qArg == "--config") {
			i++;
			if (i < argc) {
				user_config_file = QString::fromAscii(argv[i]);
			} else {
				qFatal("Fehlende Konfiguration fuer switch --config!");
			}
		}
	}

	QProcessEnvironment environ = QProcessEnvironment::systemEnvironment();
	mHome = environ.value("HOME", "");
	mUser = environ.value("USER", "");
	mRoot = environ.value("IFAOE_BIRD_CENSUS", ROOT_IFAOE_BIRD_CENSUS);
	mEnv = environ.value(ENV_IFAOE_BIRD_CENSUS, "");
	if (mEnv.isEmpty()) mEnv = mRoot;

	fRoot = QFileInfo(mRoot);
	if (!fRoot.isDir() && !fRoot.isReadable()) {
		qFatal("%s: Unbekanntes Arbeitsverzeichnis %s ", KEY_IFAOE_BIRD_CENSUS,
				mRoot.toStdString().c_str());
	}

	QString tmp = mRoot + "/symbole";
	fSymbol = QFileInfo(tmp);
	if (!fSymbol.isDir() && !fSymbol.isReadable()) {
		qFatal("%s: Unbekanntes Verzeichnis %s fuer Symbole !",
				KEY_IFAOE_BIRD_CENSUS, tmp.toStdString().c_str());
	}

	tmp = user_config_file.isEmpty() ? mHome + "/.bird-census.d/user.cfg" : user_config_file;
	fConf = QFileInfo(tmp);
	if (!fConf.isFile() && !fConf.isReadable()) {
		qFatal("%s: Unbekanntes Startdatei %s ", KEY_IFAOE_BIRD_CENSUS,
				tmp.toStdString().c_str());
	}
}

QString Defs::getConfig() const {
	return fConf.absoluteFilePath();
}

QString Defs::getSymbolPath() const {
	return fSymbol.absoluteFilePath();
}
QString Defs::getRoot() const {
	return mRoot;
}
QString Defs::getHome() const {
	return mHome;
}
QString Defs::getUser() const {
	return mUser;
}

