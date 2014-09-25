#include "ovrmapcanvas.h"
#include "cnsmapcanvas.h"
#include "geometrycalc.h"
// ----------------------------------------------------------------------
OvrMapCanvas::OvrMapCanvas(QWidget *parent,
                           Ui::MainWindow* aUI,
                           const AppConfig* aConfig,
                           Db* aDB,
                           CnsMapCanvas* aImgCanvas,
                           QgsMapLayerRegistry* lyrRegistry) :
  QgsMapCanvas(parent),
  ui(aUI), config(aConfig), db(aDB), imgCanvas(aImgCanvas),
  qgsLyrRegistry(lyrRegistry) {

    out = new TextLogger(this, ui->txtLogger, ui->statusBar);

    useImageToRender(true);
    enableAntiAliasing(true);
    setCanvasColor(QColor(0, 0, 0));
    freeze(false);

    qgsToolPoint   = new QgsMapToolEmitPoint(this);
    connect(// Quelle der Nachricht qgsMapToolPoint->canvasClicked(..
            qgsToolPoint,
            SIGNAL( canvasClicked(const QgsPoint &, Qt::MouseButton) ),
            // Ziel der der Nachricht this->doMapCanvas...
            this,
            SLOT( doCanvasClicked(const QgsPoint &, Qt::MouseButton)));
    imgCanvas->setOvrCanvas(this);
}

// ----------------------------------------------------------------------
void OvrMapCanvas::doSelectFirstTile() {
    doSelectTile(1);
}
// ----------------------------------------------------------------------
void OvrMapCanvas::doSelectNextTile() {
    if (curTile<tileFeatureIds.size()) {
    	curTile++;
    	doSelectTile(curTile);
    }
}
// ----------------------------------------------------------------------
void OvrMapCanvas::doSelectPrevTile() {
    curTile--;
    if (curTile<1) curTile = tileFeatureIds.size();
    doSelectTile(curTile);
}

// ----------------------------------------------------------------------
bool OvrMapCanvas:: saveRawTile(bool insert) {
    if (isCurTile && !insert) {
      QDateTime tmNow  = QDateTime::currentDateTimeUtc();
      QString   tmSeen = QString::number(rawImgTileTm.secsTo(tmNow));
      QString   tmWhen = rawImgTileTm.toString(Qt::ISODate);
      QString   tmUX   = QString::number(curTileUX,'f',2);
      QString   tmUY   = QString::number(curTileUY,'f',2);
      QString   tmW    = QString::number(curTileW,'f',2);
      QString   tmH    = QString::number(curTileH,'f',2);
      return db->writeRawImageTile(false,
                        rawImgTileID,
                        config->prjUtmSector(),
                        ui->lblCurCam->text(),
                        ui->lblCurImage->text(),
                        config->appUser(),
                        config->prjSession(),
                        rawImgTileUX+" "+tmUX,
                        rawImgTileUY+" "+tmUY,
                        rawImgTileW+" "+tmW,
                        rawImgTileH+" "+tmH,
                        rawImgTileTmWhen+" "+tmWhen,
                        rawImgTileTmSeen+" "+tmSeen);
    } else {
      return  db->writeRawImageTile(true,rawImgTileID, config->prjUtmSector(),
                          ui->lblCurCam->text(),
                          ui->lblCurImage->text(),
                          config->appUser(),
                          config->prjSession(),
                          "",
                          "",
                          "",
                          "",
                          "",
                          "");
    }
    return true;
}
// ----------------------------------------------------------------------
bool OvrMapCanvas:: readRawTile() {
    // Datensatz verfuegbar?
    if (!db->readRawImageTile(config->prjUtmSector(),
                          ui->lblCurCam->text(),
                          ui->lblCurImage->text(),
                          config->appUser(),
                          config->prjSession(),
                          rawImgTileID, rawImgTileUX, rawImgTileUY,
                          rawImgTileW, rawImgTileH,
                          rawImgTileTmWhen, rawImgTileTmSeen) ) {
        return false;
    }
    if (rawImgTileID<0) {
        bool done = false;

        done = saveRawTile(true)
               &&
               db->readRawImageTile(config->prjUtmSector(),
                                    ui->lblCurCam->text(),
                                    ui->lblCurImage->text(),

                                         config->appUser(), config->prjSession(),
                                         rawImgTileID, rawImgTileUX, rawImgTileUY,
                                         rawImgTileW, rawImgTileH,
                                         rawImgTileTmWhen, rawImgTileTmSeen) ;
        if (! done ) return false;
    }
    rawImgTileTm = QDateTime::currentDateTimeUtc();
}

// ----------------------------------------------------------------------
void OvrMapCanvas::doSelectTile(int num) {
    qgsImgTiles->removeSelection();
    if (!qgsImgTiles ) return;
    saveRawTile(false);
    QgsFeatureIterator iter = qgsImgTiles->dataProvider()->getFeatures();
    QgsFeature tileFeature;
    isCurTile = false;
    while (iter.nextFeature(tileFeature) && !isCurTile) {
       if ( tileFeature.attribute("IX").toInt() == num) {
           qgsImgTiles->select(tileFeature.id());
           QgsGeometry* tileGeom = tileFeature.geometry();
           isCurTile = true;
           curTileW = tileGeom->boundingBox().width();
           curTileH = tileGeom->boundingBox().height();
           curTileUX = tileGeom->boundingBox().center().x();
           curTileUY = tileGeom->boundingBox().center().y();
           curTile = num;
           isCurTile = true;
           readRawTile();
           imgCanvas->doCenter1by1(tileGeom->boundingBox().center());
           imgCanvas->setFocus();
       }
    }
}
// ----------------------------------------------------------------------
void OvrMapCanvas::doCanvasClicked(const QgsPoint &point,
                                  Qt::MouseButton button) {
    selTileId = -1;

    if (!(qgsImgTiles) ) return;
    saveRawTile(false);
    qgsImgTiles->removeSelection();
    if ( button != Qt::LeftButton ) return;
    QgsFeatureIterator iter = qgsImgTiles->dataProvider()->getFeatures();
    isCurTile = false;
    QgsFeature tileFeature;
    QgsGeometry* pntGeom = QgsGeometry::fromPoint(point);
    while (iter.nextFeature(tileFeature) && !isCurTile) {
       QgsGeometry* tileGeom = tileFeature.geometry();
       if ( tileGeom->contains(pntGeom)) {
           selTileId = tileFeature.id();
           qgsImgTiles->select(selTileId);
           curTile=tileFeature.attribute("IX").toInt();
           curTileW = tileGeom->boundingBox().width();
           curTileH = tileGeom->boundingBox().height();
           curTileUX = tileGeom->boundingBox().center().x();
           curTileUY = tileGeom->boundingBox().center().y();
           isCurTile = true;
           readRawTile();
           imgCanvas->doCenter1by1(tileGeom->boundingBox().center());
           imgCanvas->setFocus();
       }
    }
}

// ----------------------------------------------------------------------
void OvrMapCanvas::refreshLayerPaintList() {
      QList<QgsMapCanvasLayer> list;
      if (qgsImgTiles)    list.append(QgsMapCanvasLayer(qgsImgTiles));
      if (qgsImgEnvelope) list.append(QgsMapCanvasLayer(qgsImgEnvelope));
      this->setLayerSet(list);
}

// ----------------------------------------------------------------------
bool OvrMapCanvas::openImageTiles(QString strCam, QString strFile) {

    if ( qgsImgTiles ) {
         QString id = qgsImgTiles->id();
         qgsLyrRegistry->removeMapLayer(id);
         qgsImgTiles = 0;
     }
    if (! qgsImgEnvelope ) return false;

    QList<QgsField> fields;
    fields.append(QgsField("CAM",     QVariant::Int,    "Kamera"));
    fields.append(QgsField("FILE",    QVariant::String, "Image"));
    fields.append(QgsField("SESSION", QVariant::String, "Session"));
    fields.append(QgsField("SECTOR",  QVariant::Int,    "UTM-Sector"));
    fields.append(QgsField("USER",    QVariant::String, "User"));
    fields.append(QgsField("TWIDTH",  QVariant::Int,    "Breite des Tiles"));
    fields.append(QgsField("THEIGHT", QVariant::Int,    "Hoehe des Tiles"));
    fields.append(QgsField("TCOL",    QVariant::Int,    "Splatennummer des Tiles"));
    fields.append(QgsField("TROW",    QVariant::Int,    "Zeilennummer des Tiles"));
    fields.append(QgsField("IX",      QVariant::Int,    "Index"));

    QString props = QString("Polygon?")+
                    QString("crs=epsg:326")+
                    QString::number(config->prjUtmSector());

    qgsImgTiles = new QgsVectorLayer(props, "TILES", "memory");
    qgsImgTiles->dataProvider()->addAttributes(fields);
    double x0, x1, y0, y1 = 0 ;
    imgCanvas->doCalcWorldPos(0, 0, x0, y0);
    imgCanvas->doCalcWorldPos(imgCanvas->width()-100,
                              imgCanvas->height()-100,
                              x1, y1);

    // imgCanvas->doCalcWorldPos(config->imgTileWidth(), config->imgTileHeight(),
    //                           x1, y1);
    QgsRectangle r0(x0, y0, x1, y1);
    double utmTileWidth  = r0.width();
    double utmTileHeight = r0.height();
    double ttlWidth = qgsImgEnvelope->extent().width();
    double ttlHeight = qgsImgEnvelope->extent().height();
    int numX = floor(ttlWidth/utmTileWidth);
    int numY = floor(ttlHeight/utmTileHeight);
    double offsX = qgsImgEnvelope->extent().xMinimum() + (ttlWidth - utmTileWidth*(numX+1) )/2;
    double offsY = qgsImgEnvelope->extent().yMinimum() + (ttlHeight - utmTileHeight*(numY+1) )/2;
    bool done = true;
    qgsImgTiles->startEditing();
    // Auslesen der geometry
    QgsFeatureIterator iter = qgsImgEnvelope->dataProvider()->getFeatures();
    QgsFeature envFeature; QgsGeometry* envGeom;
    if (iter.nextFeature(envFeature)) {
         envGeom = envFeature.geometry();
    } else {
        return false;
    }
    tileFeatureIds.clear();int fcnt = 1;
     for(int y = numY; y >=0 ; y-- ) {
            for(int x = 0; x < (numX+1); x++ ) {
            x0 = offsX + x*utmTileWidth;
            y0 = offsY + y*utmTileHeight;
            x1 = x0 + utmTileWidth;
            y1 = y0 + utmTileHeight;
            QgsGeometry* geom = QgsGeometry::fromRect(QgsRectangle(x0, y0, x1, y1));
            QgsFeature fet = QgsFeature(qgsImgTiles->dataProvider()->fields());
            fet.setGeometry( geom );
            done = done && fet.setAttribute("CAM",strCam.toInt());
            done = done && fet.setAttribute("FILE",strFile);
            done = done && fet.setAttribute("SESSION",config->prjSession());
            done = done && fet.setAttribute("SECTOR",config->prjUtmSector());
            done = done && fet.setAttribute("USER",config->appUser());
            done = done && fet.setAttribute("TWIDTH",config->imgTileWidth());
            done = done && fet.setAttribute("THEIGHT",config->imgTileHeight());
            done = done && fet.setAttribute("TCOL",x);
            done = done && fet.setAttribute("TROW",y);
            done = done && fet.setAttribute("IX",fcnt);
            if (!done) return false;
            if (geom->intersects(envGeom)) {
                int fid = ++fcnt;
                fet.setFeatureId(fid);
                tileFeatureIds.append(fet.id());
                qgsImgTiles->addFeature(fet,false);
            }
        }
    }
    qgsImgTiles->commitChanges();
    qgsImgTiles->loadNamedStyle(config->symbolFileQml("TILE"),done);
    QgsLabel* lab = qgsImgTiles->label();
    QgsLabelAttributes* labAttr = lab->labelAttributes();
    lab->setLabelField(QgsLabel::Text, 9);
    labAttr->setColor(Qt::yellow);
    labAttr->setSize(10,0);
    qgsImgTiles->enableLabels(true);
    qgsLyrRegistry->addMapLayer(qgsImgTiles);
    refreshLayerPaintList();
    refresh();
    setMapTool(qgsToolPoint);
    return true;
}

// ----------------------------------------------------------------------
bool OvrMapCanvas::openImageEnvelope(QString strCam,
                                     QString strFile, QgsRectangle imgExt) {

    if ( qgsImgEnvelope ) {
         QString id = qgsImgEnvelope->id();
         qgsLyrRegistry->removeMapLayer(id);
         qgsImgEnvelope = 0;
     }

    QList<QgsField> fields;
    fields.append(QgsField("CAM",     QVariant::Int,    "Kamera"));
    fields.append(QgsField("FILE",    QVariant::String, "Image"));
    fields.append(QgsField("SESSION", QVariant::String, "Session"));
    fields.append(QgsField("SECTOR",  QVariant::Int,    "UTM-Sector"));
    fields.append(QgsField("USER",    QVariant::String, "User"));
    fields.append(QgsField("TWIDTH",  QVariant::Int,    "Breite der Tiles"));
    fields.append(QgsField("THEIGHT", QVariant::Int,    "Hoehe der Tiles"));

    QString props = QString("Polygon?")+
                    QString("crs=epsg:326")+
                    QString::number(config->prjUtmSector());

    qgsImgEnvelope = new QgsVectorLayer(props, "ENVELOPE", "memory");
    qgsImgEnvelope->dataProvider()->addAttributes(fields);
    QgsGeometry* qgsImgEnvGeom = validPolyGeometry(db, strCam, strFile);
    if (!qgsImgEnvGeom) {
    	QgsGeometry* qgsImgEnvGeom = db->readImageEnvelope(strCam, strFile);
    }
    QgsFeature fet = QgsFeature(qgsImgEnvelope->dataProvider()->fields());
    fet.setGeometry( qgsImgEnvGeom );
    bool done = true;
    done = done && fet.setAttribute("CAM",strCam.toInt());
    done = done && fet.setAttribute("FILE",strFile);
    done = done && fet.setAttribute("SESSION",config->prjSession());
    done = done && fet.setAttribute("SECTOR",config->prjUtmSector());
    done = done && fet.setAttribute("USER",config->appUser());
    done = done && fet.setAttribute("TWIDTH",config->imgTileWidth());
    done = done && fet.setAttribute("THEIGHT",config->imgTileHeight());
    if (!done) return false;
    qgsImgEnvelope->startEditing();
    qgsImgEnvelope->addFeature(fet,true);
    qgsImgEnvelope->commitChanges();
    done = false;
    qgsImgEnvelope->loadNamedStyle(config->symbolFileQml("ENV"),done);
    //@TODO WORK ARROUND
    QgsLabel* lab = qgsImgEnvelope->label();
    QgsLabelAttributes* labAttr = lab->labelAttributes();
    lab->setLabelField(QgsLabel::Text, 1);
    labAttr->setColor(Qt::yellow);
    qgsImgEnvelope->enableLabels(false);
    if (!done) {
        out->error("NO STYLE ENVELOPE");
    } else {
        out->log("STYLE ENVELOPE");
    }
    qgsLyrRegistry->addMapLayer(qgsImgEnvelope);
    QgsRectangle rect = qgsImgEnvelope->extent();
    rect.setXMinimum(rect.xMinimum()-10);
    rect.setYMinimum(rect.yMinimum()-10);
    rect.setXMaximum(rect.xMaximum()+10);
    rect.setYMaximum(rect.yMaximum()+10);
    setExtent(rect);
    refreshLayerPaintList();
    refresh();

    /* PROOF OF CONCEPT
    QgsRectangle extR = qgsImgEnvelope->extent();
    if (extR.intersects(imgExt)) {
      out->log("OK Envelope passt!");
    } else {
      out->log("OK Envelope passt nicht!");
    }
    */
    return true;
}

