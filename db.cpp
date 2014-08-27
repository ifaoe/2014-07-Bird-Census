#include "db.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <spa/spa.h>

Db::Db(const AppConfig *aConfig, char *key, char *help) :
    config(aConfig), sectionKey(key), sectionHelp(help)
{

}

void Db::initConfig(TextLogger *aOut) {

    const Setting &root = config->root();
    const Setting &section = config->readGroup(root, sectionKey, sectionHelp);
    out = aOut;
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
        out->log("OPEN DATABASE "+uri);
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

    QgsGeometry* geom;
    SqlQuery *q = config->getSqlQuery(ACFG_SQL_QRY_READ_IMGENV);
    QString resolv =  q->query.arg(cam).arg(image);
    QSqlQuery req(db);
    if ( ! req.exec(resolv) ) {
        out->error(req.lastError().text());
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
    SqlQuery *q = config->getSqlQuery(ACFG_SQL_QRY_READ_FDATA);
    QString resolv = q->query.arg(cam).arg(image);
    QSqlQuery req(db);
    if ( ! req.exec(resolv) ) {
        out->error(req.lastError().text());
        return 0;
    }
    spa_data = sdata;
    string date;
    string time;
    while(req.next()) {
            sdata.longitude  = lexical_cast<double>req.value(0);
            sdata.latitude   = lexical_cast<double>req.value(1);
            date = lexical_cast<string>( req.value(2) );
            time = lexical_cast<string>( req.value(3) );
    }
    time_struct tdata;
    tdata = posix_time::to_tm( posix_time::time_from_string(date + " " + time));

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
    sdata.timezone = 2;
        
    // weather data
    // TODO: Get weatherdata from crawler
    sdata.pressure         = 1200;
    sdata.temperature      = 15;
    sdata.slope            = 0;
    sdata.azm_rotation     = 0;
        
    sdata.atmos_refract    = 0.5667;
    sdata.function         = SPA_ZA;
    spa_calculate( spa );

    return sdata.azimuth;
}

// --------------------------------------------------------
bool Db::readIdMapping(const int * sync_int, const QString * cam1_img, const QString * cam2_img) {
    QString SQL_QUERY_MATCH, arg_match, sync_id;
    sync_id = QString::number(sync_int);
    if ( *sync_id == 0 ) {
        if ( cam1_img->isEmpty() ) {
            SQL_QUERY_MATCH = ACFG_SQL_QRY_READ_ID_MAP_C1;
            arg_match = *cam1_img;
        } else {
            SQL_QUERY_MATCH = ACFG_SQL_QRY_READ_ID_MAP_C2;
            arg_match = *cam2_img;
        }
    } else {
        SQL_QUERY_MATCH = ACFG_SQL_QRY_READ_ID_MAP_SYNC;
        arg_match = sync_id;
    }
    SqlQuery *q = config->getSqlQuery( SQL_QUERY_MATCH );
    QString resolv =q->query.arg(arg_match);
    QSqlQuery req(db);
    if ( ! req.exec(resolv) ) {
        out->error(req.lastError().text());
        return false;
    }
    while (req.next()) {
        *sync_id = req.value(0);
        *cam1_img= req.value(1);
        *cam2_img= req.value(2);
    }
    return true;
}

// -------------------------------------------------------
bool Db::readRawImageTile(const quint8 epsg, const QString cam, const QString file,
                      const QString usr, const QString session,
                      int &id,
                      QString &ux, QString &uy, QString &w, QString &h,
                      QString &tmWhen, QString &tmSeen) {
    SqlQuery *q = config->getSqlQuery(ACFG_SQL_QRY_READ_RIMAGE_TILE);
    QString resolv =  q->query.arg(cam).arg(file).arg(usr);
    out->log(resolv);
    out->log(q->desc);
    QSqlQuery req(db);
    id =-1; tmWhen = ""; tmSeen = "";
    if ( ! req.exec(resolv) ) {
        out->error(req.lastError().text());
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
    SqlQuery *q = config->getSqlQuery(ACFG_SQL_QRY_READ_RIMAGE);
    QString resolv = q->query.arg(cam).arg(file).arg(usr);
    out->log(resolv);
    out->log(q->desc);
    QSqlQuery req(db);
    id =-1; tmWhen = ""; tmSeen = "";
    if ( ! req.exec(resolv) ) {
        out->error(req.lastError().text());
        return false;
    }
    if (! req.next()) return true;
    id = req.value(0).toInt();
    tmWhen = req.value(1).toString();
    tmSeen = req.value(2).toString();
    return true;
7
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
             "epsg = %1, cam = '%2', img = '%3', "
             "usr = '%4', session = '%5', "
             "tm_when = '%6', tm_seen = '%7', "
             "ux = '%8', uy = '%9', w = '%10', h = '%11' "
             "WHERE rtls_id = %12;";
      lstr = lstr.arg(epsg).arg(cam).arg(file).arg(usr).arg(session)
             .arg(tmWhen).arg(tmSeen).arg(x).arg(y).arg(w).arg(h).arg(id);
  }

  QSqlQuery write(db);
  if (!write.exec(lstr)) {
      out->error(write.lastError().text());
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
      out->error(write.lastError().text());
      return false;
  }
  return true;
}

// -------------------------------------------------------
bool Db::writeRawCensus(QStringListModel* model,
                      const QString type,
                      const int epsg,
                      const QString cam,
                      const QString img,
                      const QString user,
                      const QString session) {
    QStringList list = model->stringList();
    if (list.size()<1) return true;
    int cnt = 0;
    foreach (QString data, list) {
        QStringList items = data.split(' ');
        QString lstr;
        if ( items.at(7).startsWith('-') ) {
           cnt++;
           lstr = "INSERT INTO raw_census "
                  "( tp, px, py, ux, uy, lx, ly, epsg, cam, img, usr, session ) "
                  " values ('%1',%2, %3, %4, %5, %6, %7, %8, "
                  "'%9', '%10' , '%11', '%12' );";
           lstr = lstr.arg(items.at(0)).arg(items.at(1))
                        .arg(items.at(2)).arg(items.at(3)).arg(items.at(4))
                        .arg(items.at(5)).arg(items.at(6))
                  .arg(epsg).arg(cam).arg(img).arg(user).arg(session);
        } else {

            lstr =  "UPDATE raw_census "
                    "SET tp='%1', px=%2, py=%3, ux=%4, uy=%5, lx=%6,"
                    "ly=%7, epsg=%8, cam='%9', img='%10', usr='%11', session='%12' "
                    "WHERE rcns_id = %13;";
            lstr = lstr.arg(items.at(0)).arg(items.at(1))
                          .arg(items.at(2)).arg(items.at(3)).arg(items.at(4))
                          .arg(items.at(5)).arg(items.at(6)).arg(epsg)
                          .arg(cam).arg(img).arg(user).arg(session)
                          .arg(items.at(7));
        }
        QSqlQuery write(db);
        if (!write.exec(lstr)) {
            out->error(write.lastError().text());
            return false;
        }
    }

    return true;
}

// -------------------------------------------------------
bool Db::deleteRawCensus(int id) {
    SqlQuery *q = config->getSqlQuery(ACFG_SQL_QRY_DEL_RCENSUS);
    QString resolv =  q->query.arg(id);
    QSqlQuery req(db);
    if ( ! req.exec(resolv) ) {

        out->error(req.lastError().text());
        return false;
    }
    return true;

}

// -------------------------------------------------------
QStringListModel* Db::readRawCensus(QStringListModel* model,
                      QgsVectorLayer*   layer,
                      const QString cam,
                      const QString img,
                      const QString user, int &fCnt) {
    SqlQuery *q = config->getSqlQuery(ACFG_SQL_QRY_READ_RCENSUS);
    if (model->rowCount()>0) model->removeRows(0,model->rowCount());
    QString lyrName = layer->name();
    QString resolv =  q->query.arg(lyrName).arg(cam).arg(img).arg(user);
    out->log(resolv);
    out->log(q->desc);
    QSqlQuery req(db);
    if ( ! req.exec(resolv) ) out->error(req.lastError().text());
    QgsFeatureList flist = QgsFeatureList();
    QgsFeature fet = QgsFeature(layer->dataProvider()->fields());
    int cnt = 1;
    while (req.next()) {
            QString tp = req.value(0).toString();
            int px = req.value(1).toInt();
            int py = req.value(2).toInt();
            double ux = req.value(3).toDouble();
            double uy = req.value(4).toDouble();
            double lx = req.value(5).toDouble();
            double ly = req.value(6).toDouble();
            int    id = req.value(7).toInt();
            QString lstr;
            lstr.sprintf(FMT_CNS_LIST,
                          qPrintable(tp), px, py, ux, uy, lx, ly, id, cnt);
            model->insertRow(model->rowCount());
            QModelIndex index = model->index(model->rowCount()-1);
            model->setData(index, lstr);

            fet.setGeometry( QgsGeometry::fromPoint(QgsPoint(ux,uy)) );
            fet.setAttribute("SID",id);
            fet.setAttribute("TP",lyrName);
            fet.setAttribute("PX",px);
            fet.setAttribute("PY",py);
            fet.setAttribute("UX",ux);
            fet.setAttribute("UY",uy);
            fet.setAttribute("LX",lx);
            fet.setAttribute("LY",ly);
            flist.append(fet);
            cnt++;
            if (cnt > fCnt) fCnt = cnt;
    }
    layer->dataProvider()->addFeatures(flist);
    return model;
}

// -------------------------------------------------------
QSqlQueryModel* Db::getImages(QTableView* result , QSqlQueryModel *model){
    SqlQuery *q = config->getSqlQuery(ACFG_SQL_QRY_READ_IMAGES);
    if (model) delete model;
    model = new QSqlQueryModel;
    out->log(q->desc);
    model->setQuery(q->query);
    model->setHeaderData(0, Qt::Horizontal, "SID");
    model->setHeaderData(1, Qt::Horizontal, "GID");
    model->setHeaderData(2, Qt::Horizontal, "TID");
    model->setHeaderData(3, Qt::Horizontal, "CAM1");
    model->setHeaderData(4, Qt::Horizontal, "CAM2");
    result->setModel(model);
    result->resizeRowsToContents();
    result->resizeColumnsToContents();
    result->hideColumn(0);
    return model;

}


