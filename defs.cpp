#include "defs.h"

Defs::Defs(int argc, char *argv[]) {

	QString cfgFile = TK_QSTR_NONE;

	for (int i = 0; i < argc; i++) {
		QString qArg = QString::fromAscii(argv[i]);
		args.append(qArg);
		if (qArg.compare("-c") == 0 || qArg.compare("--config") == 0) {
			i++;
			if (i < argc) {
				cfgFile = QString::fromAscii(argv[i]);
			} else {
				qFatal("Fehlende Konfiguration fuer switch --config!");
			} 
		}
	}

	QProcessEnvironment environ = QProcessEnvironment::systemEnvironment();
	mHome = environ.value("HOME", TK_QSTR_NONE);
	mUser = environ.value("USER", TK_QSTR_NONE);
	mRoot = environ.value("IFAOE_BIRD_CENSUS",
			mHome + "/" + ROOT_IFAOE_BIRD_CENSUS);
	mEnv = environ.value(ENV_IFAOE_BIRD_CENSUS, TK_QSTR_NONE);
	if (mEnv != TK_QSTR_NONE) {
		mRoot = mEnv;
	}

	fRoot = QFileInfo(mRoot);
	if (!fRoot.isDir() && !fRoot.isReadable()) {
		qFatal("%s: Unbekanntes Arbeitsverzeichnis %s ", KEY_IFAOE_BIRD_CENSUS,
				mRoot.toStdString().c_str());
	}

	QString tmp = mRoot + "/Konfiguration";
	fEtc = QFileInfo(tmp);
	if (!fEtc.isDir() && !fEtc.isReadable()) {
		qFatal("%s: Unbekanntes Einstellungsverzeichnis %s!",
				KEY_IFAOE_BIRD_CENSUS, tmp.toStdString().c_str());
	}

	tmp = mRoot + "/Konfiguration/Symbole";
	fSymbol = QFileInfo(tmp);
	if (!fSymbol.isDir() && !fSymbol.isReadable()) {
		qFatal("%s: Unbekanntes Verzeichnis %s fuer Symbole !",
				KEY_IFAOE_BIRD_CENSUS, tmp.toStdString().c_str());
	}

	if (cfgFile.compare(TK_QSTR_NONE) == 0) {
		tmp = mRoot + "/Konfiguration/user.cfg";
	} else {
		tmp = cfgFile;
	}
	fConf = QFileInfo(tmp);
	if (!fConf.isFile() && !fConf.isReadable()) {
		qFatal("%s: Unbekanntes Startdatei %s ", KEY_IFAOE_BIRD_CENSUS,
				tmp.toStdString().c_str());
	}
}

QString Defs::getConfig() const {
	return fConf.absoluteFilePath();
}
QString Defs::getEtcPath() const {
	return fEtc.absoluteFilePath();
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

