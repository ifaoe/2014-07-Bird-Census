#include "db.h"
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "spa/spa.h"
#include "ui_functions.h"
#include <iostream>

Db::Db(AppConfig *aConfig) :
    config(aConfig)
{
}

void Db::initConfig() {

    const Setting &root = config->root();
    const Setting &section = config->readGroup(root, "database", "Datenbankkonfiguration");
    AppConfig::readQString(section,
                        DB_CFGIO_KEY_DRVN,
                        driver,
                        DB_CFGIO_DEF_DRVN,
                        DB_CFGIO_HLP_DRVN,
                        false);

    db = QSqlDatabase::addDatabase(driver);
    if (! db.isValid() ) {
        qFatal(DB_ERR_CFG_DRIVER,
               DB_CFGIO_KEY_DRVN, driver.toStdString().c_str(),
               sectionKey, section.getSourceFile(), section.getSourceLine());
    }

    AppConfig::readQString(section,
                        DB_CFGIO_KEY_HOST,
                        host,
                        DB_CFGIO_DEF_HOST,
                        DB_CFGIO_HLP_HOST, false);

    AppConfig::readQString(section,
                        DB_CFGIO_KEY_NAME, name,
                        DB_CFGIO_DEF_EMPTY,
                        DB_CFGIO_HLP_NAME, true);

    AppConfig::readInteger(section,
                        DB_CFGIO_KEY_PORT,
                        port,
                        DB_CFGIO_DEF_PORT,
                        DB_CFGIO_HLP_PORT, false);

    AppConfig::readQString(section,
                        DB_CFGIO_KEY_USER,
                        user,
                        DB_CFGIO_DEF_EMPTY,
                        DB_CFGIO_HLP_PASS, true);

    AppConfig::readQString(section,
                        DB_CFGIO_KEY_PASS,
                        pass,
                        DB_CFGIO_DEF_EMPTY,
                        DB_CFGIO_HLP_PASS, false);

    db.setHostName(host);
    db.setDatabaseName(name);
    db.setUserName(user);
    db.setPassword(pass);
    db.setPort(port);
    QString strPort = ":"+QString::number(port);
    uri = user+":"+pass+"@"+driver+"://"+host+strPort+"/"+name;
    if ( db.open() ) {
//        out->log("OPEN DATABASE "+uri);
    	qDebug() << "OPEN DATABASE "+uri;
    } else {

        QString msg = db.lastError().text();
        qFatal(DB_ERR_OPEN_DB,
               uri.toStdString().c_str(),
               msg.toStdString().c_str());
    }
}

// -------------------------------------------------------
QgsGeometry* Db::readImageEnvelope(const QString cam,
                                  const QString image) {

    QgsGeometry* geom = 0;
    QString query = config->replacePrjSettings(ACFG_SQL_QRY_READ_IMGENV.arg(cam).arg(image));
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {
//        out->error(req.lastError().text());
    	qDebug() << req.lastError().text();
        return 0;
    }
    while (req.next()) {
        QString bb = req.value(0).toString();
        QString sb = req.value(1).toString();
        QString c1 = req.value(2).toString();
        QString c2 = req.value(3).toString();
        QString env = ""; QString cfg = "";
        if (cam.compare("1")==0) { cfg = c1; } else { cfg = c2; }
        if (cfg.compare("SB")==0) { env = sb; } else { env = bb; }
        geom = QgsGeometry::fromWkt(env);
    }
    return geom;

}

// -------------------------------------------------------
// Many apriori assumptions for athmospheric data
// precision good enough for now
double Db::getSolarAzimuth(const QString cam, const QString image) {
    QString query = config->replacePrjSettings(ACFG_SQL_QRY_READ_FDATA.arg(cam).arg(image));
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {
//        out->error(req.lastError().text());
    	qDebug() << req.lastError().text();
        return 0.0;
    }
    spa_data sdata;
    QString date, time;
    double cog = 0;
    while(req.next()) {
            sdata.longitude  = req.value(0).toDouble();
            sdata.latitude   = req.value(1).toDouble();
            cog =req.value(2).toDouble();
            date = req.value(3).toString();
            time = req.value(4).toString();

    }
    time_struct tdata;
    tdata = boost::posix_time::to_tm(
                boost::posix_time::time_from_string(date.toStdString() + " " + time.toStdString()));

    sdata.year     = tdata.tm_year + 1900;
    sdata.month    = tdata.tm_mon + 1;
    sdata.day      = tdata.tm_mday;
    sdata.hour     = tdata.tm_hour;
    sdata.minute   = tdata.tm_min;
    sdata.second   = tdata.tm_sec;

    // bulletin: http://maia.usno.navy.mil/ser7/ser7.dat,
    // where delta_t = 32.184 + (TAI-UTC) + DUT1
    // TODO: get data from server
    sdata.delta_t  = 32.184 + 35. - 0.3;

    // elevation: water level
    sdata.elevation= 0.;

    // manual time zone
    // TODO: get timezone from meta data
    // WORKAROUND: take UTC
    sdata.timezone = 0;

    // weather data
    // TODO: Get weatherdata from crawler
    sdata.pressure         = 1200;
    sdata.temperature      = 15;
    sdata.slope            = 0;
    sdata.azm_rotation     = 0;

    sdata.atmos_refract    = 0.5667;
    sdata.function         = SPA_ZA;
    spa_calculate( &sdata );

    double solar_dir = fmod(sdata.azimuth - cog, 360.0);
    if (solar_dir < 0) {
            return solar_dir + 360.0;
    } else {
            return solar_dir;
    }
}

// -------------------------------------------------------
// Many apriori assumptions for athmospheric data
// precision good enough for now
double Db::getTracAzimuth(const QString trc) {
    QString query = config->replacePrjSettings(ACFG_SQL_QRY_READ_SDATA.arg(trc));
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {
//        out->error(req.lastError().text());
    	qDebug() << req.lastError().text();
        return 0.0;
    }
    spa_data sdata;
    QString date, time;
    double solar_dir = 0.0, cog = 0.0;
    while(req.next()) {
		sdata.longitude  = req.value(0).toDouble();
		sdata.latitude   = req.value(1).toDouble();
		cog =req.value(2).toDouble();
		date = req.value(3).toString();
		time = req.value(4).toString();
		time_struct tdata;
		tdata = boost::posix_time::to_tm(
					boost::posix_time::time_from_string(date.toStdString() + " " + time.toStdString()));


		sdata.year     = tdata.tm_year + 1900;
		sdata.month    = tdata.tm_mon + 1;
		sdata.day      = tdata.tm_mday;
		sdata.hour     = tdata.tm_hour;
		sdata.minute   = tdata.tm_min;
		sdata.second   = tdata.tm_sec;

		// bulletin: http://maia.usno.navy.mil/ser7/ser7.dat,
		// where delta_t = 32.184 + (TAI-UTC) + DUT1
		// TODO: get data from server
		sdata.delta_t  = 32.184 + 35. - 0.3;

		// elevation: water level
		sdata.elevation= 0.;

		// manual time zone
		// TODO: get timezone from meta data
		// WORKAROUND: take UTC
		sdata.timezone = 0;

		// weather data
		// TODO: Get weatherdata from crawler
		sdata.pressure         = 1200;
		sdata.temperature      = 15;
		sdata.slope            = 0;
		sdata.azm_rotation     = 0;

		sdata.atmos_refract    = 0.5667;
		sdata.function         = SPA_ZA;
		spa_calculate( &sdata );

		double tmp_dir = fmod(sdata.azimuth - cog, 360.0);
		if (tmp_dir < 0)
			tmp_dir += 360.0;
		if (tmp_dir > 180.0)
			tmp_dir -= 360.0;
		solar_dir += tmp_dir;
    }
    solar_dir /= double(req.size());
    if (solar_dir < 0)
    	return solar_dir + 360;
    else
    	return solar_dir;
}

// --------------------------------------------------------
int Db::readIdMapping(int * sync_int, QString * cam1_img, QString * cam2_img) {
    QString arg_name, arg_value, sync_id;
    sync_id = QString::number(*sync_int);
    if ( *sync_int == 0 ) {
        if ( cam1_img->isEmpty() ) {
            arg_name  = "cam2_id";
            arg_value = *cam2_img;
        } else {
            arg_name  = "cam1_id";
            arg_value = *cam1_img;
        }
    } else {
        arg_name  = "sync_id";
        arg_value = sync_id;
    }
    QString query =
    		config->replacePrjSettings( ACFG_SQL_QRY_READ_ID_MAP.arg(arg_name).arg(arg_value));
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {
//        out->error(req.lastError().text());
        qDebug() << req.lastError().text();
        return -1;
    }
    while (req.next()) {
        *sync_int = req.value(0).toInt();
        *cam1_img = req.value(1).toString();
        *cam2_img = req.value(2).toString();
        return req.value(3).toInt();
    }
    return -1;
}

// -------------------------------------------------------
bool Db::readRawImageTile(const quint8 epsg, const QString cam, const QString file,
                      const QString usr, const QString session,
                      int &id,
                      QString &ux, QString &uy, QString &w, QString &h,
                      QString &tmWhen, QString &tmSeen) {
	Q_UNUSED(session); Q_UNUSED(epsg);
    QString query =
    		config->replacePrjSettings(ACFG_SQL_QRY_READ_RIMAGE_TILE.arg(cam).arg(file).arg(usr));
    qDebug() << query;
    QSqlQuery req(db);
    id =-1; tmWhen = ""; tmSeen = "";
    if ( ! req.exec(query) ) {
    	qDebug() << req.lastError().text();
        return false;
    }
    if (! req.next()) return true;
    id = req.value(0).toInt();
    ux = req.value(1).toString();
    uy = req.value(2).toString();
    w  = req.value(3).toString();
    h  = req.value(4).toString();
    tmWhen = req.value(5).toString();
    tmSeen = req.value(6).toString();
    return true;

}

// -------------------------------------------------------
bool Db::readRawImage(const quint8 epsg, const QString cam, const QString file,
                      const QString usr, const QString session,
                      int &id, QString &tmWhen, QString &tmSeen) {
	Q_UNUSED(session); Q_UNUSED(epsg);
    QString query =
    		config->replacePrjSettings(ACFG_SQL_QRY_READ_RIMAGE.arg(cam).arg(file).arg(usr));
    qDebug() << query;
    QSqlQuery req(db);
    id =-1; tmWhen = ""; tmSeen = "";
    if ( ! req.exec(query) ) {
    	qDebug() << req.lastError().text();
        return false;
    }
    if (! req.next()) return true;
    id = req.value(0).toInt();
    tmWhen = req.value(1).toString();
    tmSeen = req.value(2).toString();
    return true;
}


// -------------------------------------------------------
bool Db::writeRawImageTile(const bool insert, const int id,
                           const quint8 epsg, const QString cam, const QString file,
                           const QString usr, const QString session,
                           const QString x, const QString y,
                           const QString w, const QString h,
                           const QString tmWhen, const QString tmSeen) {
  QString lstr;
  if (insert) {
    lstr = "INSERT INTO raw_tiles "
           "(epsg, cam, img, usr, session, tm_when, tm_seen, ux, uy, w, h) VALUES "
           "(%1, '%2', '%3', '%4', '%5', '%6', '%7', '%9', '%10', '%11', '%12');";
    lstr = lstr.arg(epsg).arg(cam).arg(file).arg(usr)
           .arg(session).arg("").arg("").arg(x).arg(y).arg(w).arg(h);
  } else {
      lstr = "UPDATE raw_tiles SET "
             "epsg = %1, cam = '%2', "
             "usr = '%3', session = '%4', "
             "tm_when = '%5', tm_seen = '%6', "
             "ux = '%7', uy = '%8', w = '%9', h = '%10' "
             "WHERE rtls_id = %11;";
      lstr = lstr.arg(epsg).arg(cam).arg(usr).arg(session)
             .arg(tmWhen).arg(tmSeen).arg(x).arg(y).arg(w).arg(h).arg(id);
  }

  QSqlQuery write(db);
  if (!write.exec(lstr)) {
//      out->error(write.lastError().text());
	  qDebug() << write.lastError().text();
      return false;
  }
  return true;
}


// -------------------------------------------------------
bool Db::writeRawImage(const bool insert, const int id,
                       const quint8 epsg, const QString cam, const QString file,
                       const QString usr, const QString session,
                       const QString tmWhen, const QString tmSeen) {
  QString lstr;
  if (insert) {
    lstr = "INSERT INTO raw_images "
           "(epsg, cam, img, usr, session, tm_when, tm_seen) VALUES "
           "(%1, '%2', '%3', '%4', '%5', '%6', '%7');";
    lstr = lstr.arg(epsg).arg(cam).arg(file).arg(usr).arg(session).arg("").arg("");
  } else {
      lstr = "UPDATE raw_images SET "
             "epsg = %1, cam = '%2', img = '%3', "
             "usr = '%4', session = '%5', "
             "tm_when = '%6', tm_seen = '%7' WHERE rimg_id = %8;";
      lstr = lstr.arg(epsg).arg(cam).arg(file).arg(usr).arg(session)
             .arg(tmWhen).arg(tmSeen).arg(id);
  }

  QSqlQuery write(db);
  if (!write.exec(lstr)) {
//      out->error(write.lastError().text());
	  qDebug() << write.lastError().text();
      return false;
  }
  return true;
}

int Db::getRecentId() {
    QString query = config->replacePrjSettings("SELECT last_value from raw_census_rcns_id_seq;");
    qDebug() << query;
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {
    	qDebug() << req.lastError().text();
        return -1;
    }
    if (req.next()) {
        return req.value(0).toInt();
    }
    return -1;

}

// -------------------------------------------------------
bool Db::writeRawCensus(const QString type,
                      const int epsg,
                      const QString cam,
                      const QString img,
                      const QString user,
                      const QString session,
					  const QString px, const QString py,
					  const QString ux, const QString uy,
					  const QString lx, const QString ly) {
	Q_UNUSED(type);

	QString lstr = "INSERT INTO raw_census "
			  "( tp, px, py, ux, uy, lx, ly, epsg, cam, img, usr, session ) "
			  " values ('%1',%2, %3, %4, %5, %6, %7, %8, "
			  "'%9', '%10' , '%11', '%12' );";
   lstr = lstr.arg(type).arg(px).arg(py).arg(ux).arg(uy).arg(lx).arg(ly)
		   .arg(epsg).arg(cam).arg(img).arg(user).arg(session);
   qDebug() << lstr;
	QSqlQuery write(db);
	if (!write.exec(lstr)) {
		qDebug() << write.lastError().text();
		return false;
	}


    return true;
}

// -------------------------------------------------------
bool Db::deleteRawCensus(int id, QString cam, QString img) {
    QString query = config->replacePrjSettings(ACFG_SQL_QRY_DEL_RCENSUS.arg(id).arg(cam).arg(img));
    qDebug() << query;
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {

    	qDebug() << req.lastError().text();
        return false;
    }
    return true;

}

QSet<QString> * Db::readImageDone(const QString cam) {
    QSet<QString> *imgrdy = new QSet<QString>;
    QString query = config->replacePrjSettings(ACFG_SQL_QRY_READ_DONE.arg(cam));
    qDebug() << query;
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {
    	qDebug() << req.lastError().text();
        return imgrdy;
    }
    while (req.next()) {
        imgrdy->insert(QString(req.value(0).toString()));
    }
    return imgrdy;
}

// -------------------------------------------------------
bool Db::writeImageDone(const int imgRdy, const int id) {
  QString lstr;

  lstr = "UPDATE raw_images SET rdy = %1 WHERE rimg_id = %2;";
  lstr = lstr.arg(imgRdy).arg(id);

  QSqlQuery write(db);
  if (!write.exec(lstr)) {
	  qDebug() << write.lastError().text();
      return false;
  }
  return true;
}

// -------------------------------------------------------
void Db::readRawCensus(QTableWidget * tbl,
		const QString cam, const QString img, const QString user) {
    QString query;
    QString lyrName;
    QStringList usrAdmins = config->getAdmins();
    tbl->model()->removeRows(0,tbl->rowCount());
    if (usrAdmins.contains(user)) {
        query = config->replacePrjSettings(ACFG_SQL_QRY_READ_RCENSUS_ADMIN.arg(cam).arg(img));
    } else {
        query = config->replacePrjSettings(ACFG_SQL_QRY_READ_RCENSUS.arg(cam).arg(img).arg(user));
    }
    qDebug() << query;
    QSqlQuery req(db);
    if ( ! req.exec(query) ) qDebug() << req.lastError().text();

    // TODO: Clear feature list
    for (auto it = config->edtLayers->begin(); it != config->edtLayers->end(); ++it) {
    	it.value()->removeSelection();
    	QgsFeatureIds ids;
    	QgsFeatureIterator fit = it.value()->dataProvider()->getFeatures();
    	QgsFeature fet;
    	while(fit.nextFeature(fet)) {
    		ids.insert(fet.id());
    	}
    	it.value()->dataProvider()->deleteFeatures(ids);
    }
    while (req.next()) {
        QString tp = req.value(0).toString();
        int px = req.value(1).toInt();
        int py = req.value(2).toInt();
        double ux = req.value(3).toDouble();
        double uy = req.value(4).toDouble();
        double lx = req.value(5).toDouble();
        double ly = req.value(6).toDouble();
        int    id = req.value(7).toInt();

        tbl->insertRow( tbl->rowCount() );

    	QTableWidgetItem * wtp = new QTableWidgetItem(req.value(0).toString());
    	QTableWidgetItem * wpx = new QTableWidgetItem(req.value(1).toString());
    	QTableWidgetItem * wpy = new QTableWidgetItem(req.value(2).toString());
    	QTableWidgetItem * wux = new QTableWidgetItem(req.value(3).toString());
    	QTableWidgetItem * wuy = new QTableWidgetItem(req.value(4).toString());
    	QTableWidgetItem * wlx = new QTableWidgetItem(req.value(5).toString());
    	QTableWidgetItem * wly = new QTableWidgetItem(req.value(6).toString());
    	QTableWidgetItem * wid = new QTableWidgetItem(req.value(7).toString());
    	QTableWidgetItem * wusr = new QTableWidgetItem(req.value(8).toString());

    	wtp->setTextAlignment(Qt::AlignHCenter);
    	wpx->setTextAlignment(Qt::AlignHCenter);
    	wpy->setTextAlignment(Qt::AlignHCenter);
    	wux->setTextAlignment(Qt::AlignHCenter);
    	wuy->setTextAlignment(Qt::AlignHCenter);
    	wlx->setTextAlignment(Qt::AlignHCenter);
    	wly->setTextAlignment(Qt::AlignHCenter);
    	wid->setTextAlignment(Qt::AlignHCenter);
    	wusr->setTextAlignment(Qt::AlignHCenter);

		wtp->setFlags(wtp->flags() & ~Qt::ItemIsEditable);
		wpx->setFlags(wpx->flags() & ~Qt::ItemIsEditable);
		wpy->setFlags(wpy->flags() & ~Qt::ItemIsEditable);
		wux->setFlags(wux->flags() & ~Qt::ItemIsEditable);
		wuy->setFlags(wuy->flags() & ~Qt::ItemIsEditable);
		wlx->setFlags(wlx->flags() & ~Qt::ItemIsEditable);
		wly->setFlags(wly->flags() & ~Qt::ItemIsEditable);
		wid->setFlags(wid->flags() & ~Qt::ItemIsEditable);
		wusr->setFlags(wid->flags() & ~Qt::ItemIsEditable);

		tbl->setItem(req.at(), 0, wid);
		tbl->setItem(req.at(), 1, wtp);
		tbl->setItem(req.at(), 2, wux);
		tbl->setItem(req.at(), 3, wuy);
		tbl->setItem(req.at(), 4, wlx);
		tbl->setItem(req.at(), 5, wly);
		tbl->setItem(req.at(), 6, wpx);
		tbl->setItem(req.at(), 7, wpy);
		tbl->setItem(req.at(), 8, wusr);

    	QgsVectorLayer * layer = config->edtLayers->value(tp);
    	QgsFeature fet = QgsFeature(layer->dataProvider()->fields());

		fet.setGeometry( QgsGeometry::fromPoint(QgsPoint(ux,uy)) );
		fet.setAttribute("SID",id);
		fet.setAttribute("TP",tp);
		fet.setAttribute("PX",px);
		fet.setAttribute("PY",py);
		fet.setAttribute("UX",ux);
		fet.setAttribute("UY",uy);
		fet.setAttribute("LX",lx);
		fet.setAttribute("LY",ly);

		layer->startEditing();
		layer->addFeature(fet);
		layer->commitChanges();
    }
}

// -------------------------------------------------------
bool Db::getImages(QTableWidget *result, QString type){
	QString query;
	if (type == "full") {
		qDebug() << "Getting all referenced images.";
		query = config->replacePrjSettings(ACFG_SQL_QRY_READ_IMAGES_FULL);
	} else if (type == "10p") {
		qDebug() << "Getting 10% of all referenced images.";
		query = config->replacePrjSettings(ACFG_SQL_QRY_READ_IMAGES_10P);
	} else {
		qDebug() << "Unknown session type. Getting all Images.";
		query = config->replacePrjSettings(ACFG_SQL_QRY_READ_IMAGES_FULL);
	}

    qDebug() << query;
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {
    	qDebug() << req.lastError().text();
        return false;
    }
    QSet<QString> *rdy_cam1 = readImageDone(QString::fromUtf8("1"));
    QSet<QString> *rdy_cam2 = readImageDone(QString::fromUtf8("2"));
    result->setRowCount(req.size());
    result->setHorizontalHeaderLabels(QStringList() << "GID" << "TID" << "CAM1" << "CAM2");
    result->setColumnWidth(0, 50);
    result->setColumnWidth(1, 30);
    result->setColumnWidth(2, 75);
    result->setColumnWidth(3, 75);
    result->hideColumn(4);
    while (req.next()) {
        QTableWidgetItem *GID = new QTableWidgetItem(QString("%1").arg(req.value(1).toString()));
        QTableWidgetItem *TID = new QTableWidgetItem(QString("%1").arg(req.value(2).toString()));
        QTableWidgetItem *CAM1 = new QTableWidgetItem(QString("%1").arg(req.value(3).toString()));
        QTableWidgetItem *CAM2 = new QTableWidgetItem(QString("%1").arg(req.value(4).toString()));
        QTableWidgetItem *SID = new QTableWidgetItem(QString("%1").arg(req.value(0).toString()));
        GID->setTextAlignment(Qt::AlignHCenter);
        TID->setTextAlignment(Qt::AlignHCenter);
        CAM1->setTextAlignment(Qt::AlignHCenter);
        CAM2->setTextAlignment(Qt::AlignHCenter);
        result->setItem(req.at(), 0, GID);
        result->setItem(req.at(), 1, TID);
        result->setItem(req.at(), 2, CAM1);
        if ( rdy_cam1->contains( req.value(3).toString() )) {
            result->item(req.at(), 2)->setBackgroundColor(Qt::green);
        }
        result->setItem(req.at(), 3, CAM2);
        if ( rdy_cam2->contains( req.value(4).toString() )) {
            result->item(req.at(), 3)->setBackgroundColor(Qt::green);
        }
        result->setItem(req.at(), 4, SID);
    }

    return true;
}

QStringList Db::getSessionList() {
    QStringList sessionlist;
    QString query = config->replacePrjSettings("SELECT project_id FROM projects where active=1");
    qDebug() << query;
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {
    	qDebug() << req.lastError().text();
        return sessionlist;
    }
    while (req.next()) {
        sessionlist.append(QString(req.value(0).toString()));
    }
    return sessionlist;
}

project * Db::getSessionParameters(QString session) {
    project * prj = new project;
    QString query =
    		"SELECT project_id, flight_id, utm_sector, path, image_filter, session_type FROM "
    		"projects where project_id='" + session + "' ORDER BY project_id";
    qDebug() << query;
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {
    	qDebug() << req.lastError().text();
        return prj;
    }
    if(req.next()) {
    		prj->project_id = QString(req.value(0).toString());
    		prj->flight_id = QString(req.value(1).toString());
    		prj->utmSector = req.value(2).toInt();
    		prj->path = QString(req.value(3).toString());
    		prj->filter = QString(req.value(4).toString());
    		prj->session_type = QString(req.value(5).toString());
    }
    return prj;
}

QStringList Db::getCamList(QString session) {
//	QString query = "SELECT distinct cam FROM sync_utm32"
	//STUB TODO: Get cams
	// right now only 2 cams
	QStringList cams;
	cams.append("1");
	cams.append("2");
	return cams;
}

QStringList Db::getTrcList(QString session) {
	QString query =
			QString("SELECT distinct gps_trc FROM sync_utm32 where session='%1'").arg(session);
	QSqlQuery req(db);
}
