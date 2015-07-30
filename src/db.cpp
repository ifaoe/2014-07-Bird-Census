#include "db.h"
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <QHeaderView>

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
QgsGeometry* Db::readValidPolygon(const QString cam, const QString image) {

    QgsGeometry* geom = 0;
    QString query = config->replacePrjSettings(ACFG_SQL_QRY_READ_VALIDPOLY).arg(cam).arg(image);
    QSqlQuery req(db);
    qDebug() << query;
    if ( ! req.exec(query) ) {
    	qDebug() << req.lastError().text();
        return 0;
    }
    if (req.next()) {
        geom = QgsGeometry::fromWkt(req.value(0).toString());
    }
    return geom;

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
bool Db::deleteRawCensus(int id, QString cam, QString img, QString user) {
    QString query = config->replacePrjSettings(ACFG_SQL_QRY_DEL_RCENSUS
    		.arg(id).arg(cam).arg(img).arg(user));
    qDebug() << query;
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {

    	qDebug() << req.lastError().text();
        return false;
    }
    return true;

}

void Db::readImageDone(const QString cam, QStringList & ready_list) {
    QString query = config->replacePrjSettings(ACFG_SQL_QRY_READ_DONE.arg(cam));
    qDebug() << query;
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {
    	qDebug() << req.lastError().text();
        return;
    }
    while (req.next()) {
    	ready_list.append(req.value(0).toString());
    }
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
void Db::UpdateObjectQuery(const QString cam, const QString img, QSqlQueryModel * model) {
    QStringList usrAdmins = config->getAdmins();
    QString query;
    if (config->prjSession().startsWith("Testdatensatz"))
    	query = config->replacePrjSettings(ACFG_SQL_QRY_READ_RCENSUS_ADMIN.arg(cam).arg(img));
    else
    	query = config->replacePrjSettings(ACFG_SQL_QRY_READ_RCENSUS.arg(cam).arg(img).arg(config->appUser()));
    qDebug() << query;
    model->setQuery(query);
    return;
}

// -------------------------------------------------------
bool Db::getImages(QTableWidget *result, QString type, QString filter, bool missing){
	Q_UNUSED(type);
	QString query;
	result->clearSelection();
	result->clearContents();
//	result->clear();
	result->setRowCount(0);
	if (missing)
		query = config->replacePrjSettings(ACFG_SQL_QRY_READ_IMAGES_NOT_READY).arg(filter);
	else
		query = config->replacePrjSettings(ACFG_SQL_QRY_READ_IMAGES).arg(filter);

    qDebug() << query;
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {
    	qDebug() << req.lastError().text();
        return false;
    }
    QStringList rdy_cam1, rdy_cam2;
    readImageDone(QString("1"), rdy_cam1);
    readImageDone(QString("2"),rdy_cam2 );
    result->setRowCount(req.size());
    result->setColumnCount(3);
    result->horizontalHeader()->setStretchLastSection(true);
    result->setColumnWidth(0, 50);
    result->setColumnWidth(1, 50);
//    result->setColumnWidth(2, 50);
    while (req.next()) {
        QTableWidgetItem *wtrc = new QTableWidgetItem(QString(req.value(0).toString()));
        QTableWidgetItem *wcam = new QTableWidgetItem(QString(req.value(1).toString()));
        QTableWidgetItem *wimg = new QTableWidgetItem(QString(req.value(2).toString()));

        wtrc->setTextAlignment(Qt::AlignHCenter);
        wcam->setTextAlignment(Qt::AlignHCenter);
        wimg->setTextAlignment(Qt::AlignHCenter);
        result->setItem(req.at(), 0, wtrc);
        result->setItem(req.at(), 1, wcam);
        result->setItem(req.at(), 2, wimg);
        if ( req.value(1).toInt() == 1) {
        	if( rdy_cam1.contains( req.value(2).toString() )) {
        		result->item(req.at(), 0)->setBackgroundColor(Qt::green);
        		result->item(req.at(), 1)->setBackgroundColor(Qt::green);
        		result->item(req.at(), 2)->setBackgroundColor(Qt::green);
        	}
        } else if (req.value(1).toInt() == 2) {
			if (rdy_cam2.contains(req.value(2).toString() )) {
        		result->item(req.at(), 0)->setBackgroundColor(Qt::green);
        		result->item(req.at(), 1)->setBackgroundColor(Qt::green);
        		result->item(req.at(), 2)->setBackgroundColor(Qt::green);
			}
		}
    }

    return true;
}

QStringList Db::getSessionList() {
    QStringList sessionlist;
    QString query;
    if (config->getAdmins().contains(config->appUser()))
    	query = config->replacePrjSettings("SELECT project_id FROM projects where active>0");
    else
    	query = config->replacePrjSettings("SELECT project_id FROM projects where active=1");
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
	Q_UNUSED(session);
//	QString query = "SELECT distinct cam FROM sync_utm32"
	//STUB TODO: Get cams
	// right now only 2 cams
	QStringList cams;
	cams.append("1");
	cams.append("2");
	return cams;
}

QStringList Db::getTrcList(QString session) {
	QStringList trc_list;
	QString query =
			QString("SELECT distinct gps_trc FROM sync_utm32 where session='%1' ORDER BY gps_trc").arg(session);
    qDebug() << query;
    QSqlQuery req(db);
    if ( ! req.exec(query) ) {
    	qDebug() << req.lastError().text();
        return trc_list;
    }
    while(req.next())
    	trc_list.append(req.value(0).toString());
    return trc_list;
}
