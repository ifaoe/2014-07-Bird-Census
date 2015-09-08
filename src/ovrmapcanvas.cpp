#include "ovrmapcanvas.h"
#include "cnsmapcanvas.h"
// ----------------------------------------------------------------------
OvrMapCanvas::OvrMapCanvas(QWidget *parent,
                           Ui::MainWindow* aUI,
                           ConfigHandler* aConfig,
                           Db* aDB,
                           CnsMapCanvas* aImgCanvas,
                           QgsMapLayerRegistry* lyrRegistry) :
  QgsMapCanvas(parent),
  ui(aUI), config(aConfig), db(aDB), imgCanvas(aImgCanvas),
  qgsLyrRegistry(lyrRegistry) {


//    useImageToRender(true); //deprecated
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
    } else {
        QMessageBox msgBox;
        msgBox.setText("Das Ende des Bildes wurde erreicht.");
        msgBox.setInformativeText(trUtf8("Soll das nächste Bild geladen werden? (Aktuelles Bild wird als fertig markiert!)"));
        QAbstractButton *nextButton = msgBox.addButton(trUtf8("Ja"), QMessageBox::YesRole);
        QAbstractButton *stayButton = msgBox.addButton(trUtf8("Nein"), QMessageBox::NoRole);
        msgBox.exec();
        if(msgBox.clickedButton() == nextButton) {
            int rawImgID = -1;
            QString rawImgTmWhen = "";
            QString rawImgTmSeen = "";
            db->readRawImage(config->getCurrentCam(), config->getCurrentImage(), config->getUser(), rawImgID, rawImgTmWhen,
            		rawImgTmSeen);
            db->writeImageDone(1, rawImgID);
            int curRow = ui->image_table->selectionModel()->selectedRows().at(0).row();
            if (curRow < ui->image_table->rowCount()) {
            	QModelIndex newRow =ui->image_table->selectionModel()->model()->index(curRow+1,0);
                ui->image_table->selectionModel()->select(
                		newRow, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            }
        } else if (msgBox.clickedButton() == stayButton) {

        }
    }
}
// ----------------------------------------------------------------------
void OvrMapCanvas::doSelectPrevTile() {
    if (curTile>1) {
        curTile--;
        doSelectTile(curTile);
    }
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
                        config->getUtmSector(),
						config->getCurrentCam(), config->getCurrentImage(),
                        config->getUser(),
                        config->getProjectId(),
                        rawImgTileUX+" "+tmUX,
                        rawImgTileUY+" "+tmUY,
                        rawImgTileW+" "+tmW,
                        rawImgTileH+" "+tmH,
                        rawImgTileTmWhen+" "+tmWhen,
                        rawImgTileTmSeen+" "+tmSeen);
    } else {
      return  db->writeRawImageTile(true,rawImgTileID, config->getUtmSector(),
    		  	  	  	  config->getCurrentCam(), config->getCurrentImage(),
                          config->getUser(),
                          config->getProjectId(),
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
	// TODO: TODO: TODO: CLEANUP
    // Datensatz verfuegbar?
    if (!db->readRawImageTile( config->getCurrentCam(), config->getCurrentImage(), config->getUser(), rawImgTileID,
    		rawImgTileUX, rawImgTileUY, rawImgTileW, rawImgTileH, rawImgTileTmWhen, rawImgTileTmSeen) ) {
        return false;
    }
    if (rawImgTileID<0) {
        bool done = false;

        done = saveRawTile(true)
               &&
               db->readRawImageTile(config->getCurrentCam(), config->getCurrentImage(), config->getUser(), rawImgTileID,
            		   rawImgTileUX, rawImgTileUY, rawImgTileW, rawImgTileH, rawImgTileTmWhen, rawImgTileTmSeen) ;
        if (! done ) return false;
    }
    rawImgTileTm = QDateTime::currentDateTimeUtc();
    return true;
}

// ----------------------------------------------------------------------
void OvrMapCanvas::doSelectTile(int num) {
    qgs_image_tiles_->removeSelection();
    if (!qgs_image_tiles_ ) return;
    saveRawTile(false);
    QgsFeatureIterator iter = qgs_image_tiles_->dataProvider()->getFeatures();
    QgsFeature tileFeature;
    isCurTile = false;
    while (iter.nextFeature(tileFeature) && !isCurTile) {
       if ( tileFeature.attribute("IX").toInt() == num) {
           qgs_image_tiles_->select(tileFeature.id());
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

    if (!(qgs_image_tiles_) ) return;
    saveRawTile(false);
    qgs_image_tiles_->removeSelection();
    if ( button != Qt::LeftButton ) return;
    QgsFeatureIterator iter = qgs_image_tiles_->dataProvider()->getFeatures();
    isCurTile = false;
    QgsFeature tileFeature;
    QgsGeometry* pntGeom = QgsGeometry::fromPoint(point);
    while (iter.nextFeature(tileFeature) && !isCurTile) {
       QgsGeometry* tileGeom = tileFeature.geometry();
       if ( tileGeom->contains(pntGeom)) {
           selTileId = tileFeature.id();
           qgs_image_tiles_->select(selTileId);
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
      if (qgs_image_tiles_)    list.append(QgsMapCanvasLayer(qgs_image_tiles_));
      if (qgs_image_envelope_) list.append(QgsMapCanvasLayer(qgs_image_envelope_));
      this->setLayerSet(list);
}

// ----------------------------------------------------------------------
bool OvrMapCanvas::openImageTiles(QString strCam, QString strFile) {

    if ( qgs_image_tiles_ ) {
         QString id = qgs_image_tiles_->id();
         qgsLyrRegistry->removeMapLayer(id);
         qgs_image_tiles_ = 0;
     }
    if (! qgs_image_envelope_ ) return false;

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
                    QString::number(config->getUtmSector());

    qgs_image_tiles_ = new QgsVectorLayer(props, "TILES", "memory");
    qgs_image_tiles_->dataProvider()->addAttributes(fields);
    double x0, x1, y0, y1 = 0 ;
    imgCanvas->doCalcWorldPos(0, 0, x0, y0);
    imgCanvas->doCalcWorldPos(imgCanvas->width()-100,
                              imgCanvas->height()-100,
                              x1, y1);

    QgsRectangle r0(x0, y0, x1, y1);
    double utmTileWidth  = r0.width()*imgCanvas->getScaleFactor();
    double utmTileHeight = r0.height()*imgCanvas->getScaleFactor();
    double ttlWidth = qgs_image_envelope_->extent().width();
    double ttlHeight = qgs_image_envelope_->extent().height();
    int numX = floor(ttlWidth/utmTileWidth);
    int numY = floor(ttlHeight/utmTileHeight);
    double offsX = qgs_image_envelope_->extent().xMinimum() + (ttlWidth - utmTileWidth*(numX+1) )/2;
    double offsY = qgs_image_envelope_->extent().yMinimum() + (ttlHeight - utmTileHeight*(numY+1) )/2;
    bool done = true;
    qgs_image_tiles_->startEditing();
    // Auslesen der geometry
    QgsFeatureIterator iter = qgs_image_envelope_->dataProvider()->getFeatures();
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
            QgsFeature fet = QgsFeature(qgs_image_tiles_->dataProvider()->fields());
            fet.setGeometry( geom );
            done = done && fet.setAttribute("CAM",strCam.toInt());
            done = done && fet.setAttribute("FILE",strFile);
            done = done && fet.setAttribute("SESSION",config->getProjectId());
            done = done && fet.setAttribute("SECTOR",config->getUtmSector());
            done = done && fet.setAttribute("USER",config->getUser());
            done = done && fet.setAttribute("TWIDTH",config->getTileWidth());
            done = done && fet.setAttribute("THEIGHT",config->getTileHeight());
            done = done && fet.setAttribute("TCOL",x);
            done = done && fet.setAttribute("TROW",y);
            done = done && fet.setAttribute("IX",fcnt);
            if (!done) return false;
            if (geom->intersects(envGeom)) {
                int fid = ++fcnt;
                fet.setFeatureId(fid);
                tileFeatureIds.append(fet.id());
                qgs_image_tiles_->addFeature(fet,false);
            }
        }
    }
    qgs_image_tiles_->commitChanges();

    QgsStringMap properties;
    properties["color"]="255,170,0";
    properties["outline_color"]="170,85,0";
    properties["outline_width"]="0.26";

    QgsFillSymbolV2 * symbol = QgsFillSymbolV2::createSimple(properties);
//    symbol->setAlpha(0.4);
    qgs_image_tiles_->setLayerTransparency(50);
     qgs_image_tiles_->setRendererV2(new QgsSingleSymbolRendererV2(symbol));

    QgsLabel* lab = qgs_image_tiles_->label();
    QgsLabelAttributes* labAttr = lab->labelAttributes();
    lab->setLabelField(QgsLabel::Text, 9);
    labAttr->setColor(Qt::yellow);
    labAttr->setSize(10,0);
    qgs_image_tiles_->enableLabels(true);
    qgsLyrRegistry->addMapLayer(qgs_image_tiles_);
    refreshLayerPaintList();
    refresh();
    setMapTool(qgsToolPoint);
    return true;
}

// ----------------------------------------------------------------------
bool OvrMapCanvas::openImageEnvelope(QString strCam,
                                     QString strFile, QgsRectangle imgExt) {

	Q_UNUSED(imgExt);
    if ( qgs_image_envelope_ ) {
         QString id = qgs_image_envelope_->id();
         qgsLyrRegistry->removeMapLayer(id);
         qgs_image_envelope_ = 0;
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
                    QString::number(config->getUtmSector());

    qgs_image_envelope_ = new QgsVectorLayer(props, "ENVELOPE", "memory");

        QgsStringMap properties;
        properties["color"]="85,85,127";
        properties["outline_color"]="255,255,0";

    QgsFillSymbolV2 * symbol = QgsFillSymbolV2::createSimple(properties);;
//    symbol->setAlpha(1.0);
//    symbol->setColor(QColor(85,85,127,255));
     qgs_image_envelope_->setRendererV2(new QgsSingleSymbolRendererV2(symbol));

    qgs_image_envelope_->dataProvider()->addAttributes(fields);
    QgsGeometry* qgsImgEnvGeom = db->readValidPolygon(strCam, strFile);
    QgsFeature fet = QgsFeature(qgs_image_envelope_->dataProvider()->fields());
    fet.setGeometry( qgsImgEnvGeom );
    bool done = true;
    done = done && fet.setAttribute("CAM",strCam.toInt());
    done = done && fet.setAttribute("FILE",strFile);
    done = done && fet.setAttribute("SESSION",config->getProjectId());
    done = done && fet.setAttribute("SECTOR",config->getUtmSector());
    done = done && fet.setAttribute("USER",config->getUser());
    done = done && fet.setAttribute("TWIDTH",config->getTileWidth());
    done = done && fet.setAttribute("THEIGHT",config->getTileHeight());
    if (!done) return false;
    qgs_image_envelope_->startEditing();
    qgs_image_envelope_->addFeature(fet,true);
    qgs_image_envelope_->commitChanges();
    done = false;

    //@TODO WORK ARROUND
    QgsLabel* lab = qgs_image_envelope_->label();
    QgsLabelAttributes* labAttr = lab->labelAttributes();
    lab->setLabelField(QgsLabel::Text, 1);
    labAttr->setColor(Qt::yellow);
    qgs_image_envelope_->enableLabels(false);
    qgsLyrRegistry->addMapLayer(qgs_image_envelope_);
    QgsRectangle rect = qgs_image_envelope_->extent();
    rect.setXMinimum(rect.xMinimum()-10);
    rect.setYMinimum(rect.yMinimum()-10);
    rect.setXMaximum(rect.xMaximum()+10);
    rect.setYMaximum(rect.yMaximum()+10);
    setExtent(rect);
    refreshLayerPaintList();
    refresh();
    return true;
}

