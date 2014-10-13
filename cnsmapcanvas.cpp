#include "cnsmapcanvas.h"
#include "ovrmapcanvas.h"
#include "geometrycalc.h"
#include <qgsgeometry.h>


CnsMapCanvas::CnsMapCanvas(QWidget *parent,
                           Ui::MainWindow *aUI,
                           const AppConfig *aConfig,
                           Db* aDB,
                           QgsMapLayerRegistry *lyrRegistry) :
    QgsMapCanvas(parent),
    ui(aUI), config(aConfig), db(aDB), qgsLyrRegistry(lyrRegistry)
{

    out = new TextLogger(this, ui->txtLogger, ui->statusBar);

    // Initialize MapCanvas
    enableAntiAliasing(true);
    setParallelRenderingEnabled( true );
    setCanvasColor(QColor(0, 0, 0));
    freeze(false);

    // Ausdehnung des Kartenfensters in Pixeln
    intMapCursorPixelCol = 0;
    intMapCursorPixelRow = 0;

    // Definition der Aktionen mit der Karte
    // qtActZoomIn   = new QAction("ZOOM+", this);
    // qtActZoomOut  = new QAction("ZOOM-", this);
    // qgsToolZoomIn  = new QgsMapToolZoom(this, false);
    // qgsToolZoomOut = new QgsMapToolZoom(this, true);

    qgsToolPan     = new QgsMapToolPan(this);
    qgsToolPoint   = new QgsMapToolEmitPoint(this);
    qgsToolIdentify   = new QgsMapToolIdentify(this);

    // Verbindung der Maptools Pan, ZoomIn, ZoomOut mit dem Aktionen
    // qgsToolZoomIn->setAction(qtActZoomIn);
    // qgsToolZoomOut->setAction(qtActZoomOut);

    // Verbindung der Aktionen mit den Routinen
    // connect(qtActZoomOut, SIGNAL(triggered()), this, SLOT(doZoomOut()));
    // connect(qtActZoomIn,  SIGNAL(triggered()), this, SLOT(doZoomIn()));
    connect(ui->btnMapZoomExtent,SIGNAL(clicked()), this, SLOT(doZoomExtent()));
    connect(ui->btnMapZoom1by1, SIGNAL(clicked()), this, SLOT(doZoom1by1()));
    connect(ui->btnMapPan, SIGNAL(clicked()), this, SLOT(doModePan()));
    connect(ui->btnMapDigitize, SIGNAL(clicked()), this, SLOT(doModeDigitize()));
    connect(ui->btnMapSelect, SIGNAL(clicked()), this, SLOT(doModeSelect()));


    // Verbindung des qgsMapToolPoint mit der Rechenroutine
    connect(// Quelle der Nachricht qgsMapToolPoint->canvasClicked(..
            qgsToolPoint,
            SIGNAL( canvasClicked(const QgsPoint &, Qt::MouseButton) ),
            // Ziel der der Nachricht this->doMapCanvas...
            this,
            SLOT( doCanvasClicked(const QgsPoint &, Qt::MouseButton)));

    connect(this, SIGNAL(keyPressed(QKeyEvent *)),
            this, SLOT(doHandleKeyPressed(QKeyEvent *)));

    connect(this, SIGNAL(keyReleased(QKeyEvent *)),
             this, SLOT(doHandleKeyReleased(QKeyEvent *)));

    connect(this, SIGNAL(xyCoordinates(const QgsPoint &)),
            this, SLOT( doHandleCoords(const QgsPoint &)));

    connect(this, SIGNAL(scaleChanged(double)),
            this, SLOT( doUpdateStatus()));

    connect(this, SIGNAL(extentsChanged()),
            this, SLOT( doUpdateStatus()));




    // Aktionen Map Canvas an Toolbar binden
    //ui->btnMapZoomIn->addAction(qtActZoomIn);
    //ui->btnMapZoomIn->addAction(qtActZoomOut);

    // Initialisierung des ImageLayer der Karte
    qgsImgLayer    = 0;
    qgsImgProvider = 0;
    qgsTrfm2LonLat = 0;

    crs4326 = QgsCoordinateReferenceSystem(4326);
    setMapUnits(QGis::Meters);

    mapLayerStack = new LayerStack(this, qgsLyrRegistry);
}

bool CnsMapCanvas::addEdtTbx(QString tbName, int tbIndex, QListView *tbListView) {
	edtKeys[tbIndex] = tbName;
	edtViews[tbIndex] = tbListView;
	edtLayers[tbIndex] = NULL;
	return true;
}

// --------------------------------------------------------------------------
void CnsMapCanvas::setOvrCanvas(OvrMapCanvas* ovrCvs) {
   ovrCanvas =  ovrCvs;
}


// ===========================================================================
// Slots fuer den MapCanvas
// ===========================================================================
/*  doMapCanvasZoom In/Out Zoom-Modus wechseln
void CnsMapCanvas::doZoomOut() {
    out->log(tr("set MapMode ZOOM-"));
    setMapTool(qgsToolZoomOut);
}

// --------------------------------------------------------------------------
void CnsMapCanvas::doZoomIn() {
    out->log(tr("set MapMode ZOOM-"));
    setMapTool(qgsToolZoomOut);
}
*/
// --------------------------------------------------------------------------
/** @brief CnsMapCanvas::doZoomExtent() auf gesamten Kartenausschnitt zoomen */
void CnsMapCanvas::doZoomExtent(){
    if (!qgsImgLayer) return;
    out->log(tr("ZOOM GANZE KARTE"));
    this->setExtent(qgsImgLayer->extent());
    this->refresh();
}

// --------------------------------------------------------------------------
/** @brief CnsMapCanvas::doZoom1by1 auf 1:1 Maszstab zoomen */
void CnsMapCanvas::doZoom1by1() {
    if (out) out->log(tr("ZOOM 1:1"));
    doCenter1by1(dblCurCenterUtmX, dblCurCenterUtmY);
}

// --------------------------------------------------------------------------
/**
 * @brief MainWindow::doModePan In den Pan Modus wechseln
 */
void CnsMapCanvas::doModePan() {
    out->log(tr("set MapMode PAN"));
    mapMode = MAP_MODE_INSPECT;
    setMapTool(qgsToolPan);
}

// --------------------------------------------------------------------------
/**
 * @brief CnsMapCanvas::doModeDigitize() In den Digitalisierungsmodus wechseln
 */
void CnsMapCanvas::doModeDigitize() {
    out->log(tr("MODUS ERFASSUNG"));
    mapMode = MAP_MODE_DIGITIZE;
    setMapTool(qgsToolPoint);

}
// --------------------------------------------------------------------------
/**
 * @brief CnsMapCanvas::doModeSelect Punkte selektieren und inzpizieren
 */
void CnsMapCanvas::doModeSelect() {
      out->log(tr("MODUS AUSWAHL"));
      mapMode = MAP_MODE_SELECT;
      setMapTool(qgsToolPoint);
      this->setCursor(Qt::ArrowCursor);
}

// --------------------------------------------------------------------------
void CnsMapCanvas::doSetupEditModus() {
    // 1:1 Zoomen
    doZoom1by1();
    ui->btnMapPan->setChecked(true);
    doModePan();
    setFocus();
}

// --------------------------------------------------------------------------
void CnsMapCanvas::doCenter1by1(QgsPoint point) {
     doCenter1by1(point.x(), point.y());
}

// --------------------------------------------------------------------------
void CnsMapCanvas::doCenter1by1(double x, double y) {
     int px, py;
     doCalcPixPos(QgsPoint(x,y),px,py);
     int w2 = this->width()/2;
     int h2 = this->height()/2;
     doCalcWorldPos(px-w2,py+h2,dblCurMinUtmX,dblCurMinUtmY);
     doCalcWorldPos(px+w2,py-h2,dblCurMaxUtmX,dblCurMaxUtmY);
     QgsRectangle rect(dblCurMinUtmX,
                       dblCurMinUtmY,
                       dblCurMaxUtmX,
                       dblCurMaxUtmY);
     this->setExtent(rect);
     this->refresh();
}


// --------------------------------------------------------------------------
/** Aktualisieren allerstatuslabel und Variablen */
void CnsMapCanvas::doUpdateStatus() {
    if (!qgsImgLayer) return;
    QString strMode = TK_QSTR_NONE;
    switch (mapMode) {
    case MAP_MODE_DIGITIZE:
        strMode = "ERFASSUNG";
        break;
    case MAP_MODE_SELECT:
        strMode = "AUSWAHL";
        break;
    default:
        strMode = "ANSICHT";
        break;
    }
    QString str;
    ui->lblUTM->setText(str.sprintf("UTM%d: %010.2f %010.2f m", config->prjUtmSector(),
                                    dblMapCursorUtmX, dblMapCursorUtmY));
    ui->lblMode->setText(str.sprintf("Mode: %s ",qPrintable(strMode)));
    ui->lblScale->setText(str.sprintf("Skale 1: %f", this->scale()/72.0));
    dblCurMinUtmX = extent().xMinimum();
    dblCurMinUtmY = extent().yMinimum();
    dblCurMaxUtmX = extent().xMaximum();
    dblCurMaxUtmY = extent().yMaximum();
    dblCurCenterUtmX = extent().center().x();
    dblCurCenterUtmY = extent().center().y();
    /*
    int px,py;
    if (doCalcPixPos(QgsPoint(cx,cy),px,py)) {
      ui->lblPos->setText(str.sprintf("Kachel: %d %d", px, py));
    } else {
      ui->lblPos->setText("Kachel: -----");
    }
   */
    if ( (intMapCursorPixelCol < 0) || (intMapCursorPixelRow < 0) ) {
        ui->lblPixel->setText("Position ------ -----");
    } else {
        ui->lblPixel->setText(str.sprintf("Position %05d %05d",
                                          intMapCursorPixelCol,
                                          intMapCursorPixelRow));
    }
#ifdef OPENCV
    cvImageBirdStatus = mapImg2CV(QgsPoint(dblWorldX, dblWorldY),2.0,200,200);
    if (cvImageBirdStatus == 1) {
        QImage img = mat2QImage8Bit(*cvImageBird);
        ui->grxView->scene()->clear();
        ui->grxView->scene()->addPixmap(QPixmap::fromImage(img));
        ui->grxView->scene()->update();
    }
#endif /* OPENCV */
}

// --------------------------------------------------------------------------
void CnsMapCanvas::doHandleKeyPressed(QKeyEvent *keyEvent) {
    if (keyEvent->key() == Qt::Key_Control) {
        keyCtrl = true;
        out->log("MCVS KEY COMMAND MODE ON");
    }
    if (keyEvent->key() == Qt::Key_Shift) {
        keyShift = true;
        out->log("MCVS KEY SHIFT MODE ON");
    }

}

// --------------------------------------------------------------------------
void CnsMapCanvas::doHandleKeyReleased(QKeyEvent *keyEvent) {
    if (keyEvent->key() == Qt::Key_Control) {
        keyCtrl = false;
        out->log("MCVS KEY COMMAND MODE OFF");
    }
    if (keyEvent->key() == Qt::Key_Shift) {
        keyShift = false;
        out->log("MCVS KEY SHIFT MODE OF");
    }
    if (keyEvent->key() == Qt::Key_Space && !keyShift) {
        ovrCanvas->doSelectNextTile();
    }
    if (keyEvent->key() == Qt::Key_Space && keyShift) {
        ovrCanvas->doSelectPrevTile();
    }
}

// --------------------------------------------------------------------------
bool CnsMapCanvas::doCalcLonLat(const QgsPoint& point ,
                              double& lon, double& lat) {

    if (!qgsImgLayer ) return false;
    if (!qgsTrfm2LonLat) return false;
    QgsPoint pnt = qgsTrfm2LonLat->transform(point);
    lon = pnt.x(); lat = pnt.y();
   return true;
}

// --------------------------------------------------------------------------
bool CnsMapCanvas::doHandleCoords(const QgsPoint &point) {
    if (!qgsImgLayer) return false;
    if ( doCalcPixPos(point, intMapCursorPixelCol, intMapCursorPixelRow) ) {
      dblMapCursorUtmX = point.x(); dblMapCursorUtmY = point.y();
      doCalcLonLat(point,dblMapCursorLongitude,dbMapCursorLatitude);
      doUpdateStatus();
    }
    return true;
}

// --------------------------------------------------------------------------
bool CnsMapCanvas::doCalcPixPos(const QgsPoint& point ,
                              int& pixX, int& pixY) {

    if (! qgsImgLayer ) return false;

    double x = point.x();
    double y = point.y();

    pixX = floor(( x - dblExtMinUtmX ) / dblUnitPixX ) ;
    pixY = floor(( dblExtMaxUtmY - y ) / dblUnitPixY ) ;

    return true;
}

// --------------------------------------------------------------------------
bool CnsMapCanvas::doCalcWorldPos(const int pixX ,const int pixY,
                                  double& wldX, double& wldY) {

    if (! qgsImgLayer ) return false;

    double x = pixX;
    double y = pixY;

    wldX = dblExtMinUtmX + x * dblUnitPixX;
    wldY = dblExtMaxUtmY - y * dblUnitPixY;

    return true;
}

// --------------------------------------------------------------------------
bool CnsMapCanvas::doOpenRasterLayer(QString cam, QString file) {
	QMap<int, QgsVectorLayer*>::iterator i;
	for(i=edtLayers.begin(); i!=edtLayers.end(); ++i) {
		i.value() = openEditLayer(config->prjPath(), cam, file, edtKeys[i.key()], i.key(), i.value());
	}
    bool enod = openPolyLayer(cam, file);
    bool done = openRasterLayer(config->prjPath(), cam, file);

    return done && enod;
}
// --------------------------------------------------------------------------
bool CnsMapCanvas::doSaveData() {
    doSaveData(ui->lblCurCam->text(),ui->lblCurImage->text());
    return true;
}
// --------------------------------------------------------------------------
bool CnsMapCanvas::doSaveData(QString cam, QString file) {
    return saveData(cam, file);
}

// --------------------------------------------------------------------------
bool CnsMapCanvas::saveData(const QString cam, const QString file) {

    if (cam.compare(TK_QSTR_NONE) == 0 ) return true;

    QDateTime tmNow = QDateTime::currentDateTimeUtc();
    QString tmSeen = QString::number(rawImgTm.secsTo(tmNow));
    QString tmWhen = rawImgTm.toString(Qt::ISODate);
    db->writeRawImage(false,rawImgID, config->prjUtmSector(), cam, file,
                      config->appUser(), config->prjSession(),
                      rawImgTmWhen+" "+tmWhen, rawImgTmSeen+" "+tmSeen);

    QMap<int, QString>::iterator i;
    for (i = edtKeys.begin(); i!=edtKeys.end(); ++i) {
        QStringListModel* model = qobject_cast<QStringListModel*>(edtViews[i.key()]->model());
        bool done = db->writeRawCensus(model,i.value(),config->prjUtmSector(), cam, file,
                           config->appUser(), config->prjSession());
    }
    return true;
}

// --------------------------------------------------------------------------
QgsMapLayer* CnsMapCanvas::layerByKey(QString key) {
    return mapLayerStack->getMapLayer(key);
}
// --------------------------------------------------------------------------
QgsVectorLayer* CnsMapCanvas::rbCheckedVectorLayer() {
	return edtLayers[ui->btgLayers->checkedId()];
}
// --------------------------------------------------------------------------
QListView* CnsMapCanvas::rbCheckedListView() {
	return edtViews[ui->btgLayers->checkedId()];
}

// --------------------------------------------------------------------------
int CnsMapCanvas::getMapMode() { return mapMode; }
// --------------------------------------------------------------------------
QgsFeatureId nextFeatureId(QgsVectorLayer* lyr) {
    QgsFeatureIterator iter=lyr->getFeatures();
    QgsFeature fet; QgsFeatureId id =0;
    while (iter.nextFeature(fet) ) {
        if (fet.id()>id) id = fet.id();
    }
    return id+1;
}
// --------------------------------------------------------------------------
/**
 * @brief MainWindow::doMapCanvasClicked Einen Punkt markieren
 * @param point Geokoordinaten auf die geklickt wurden
 * @param button Mausknpf der geklickt wurde
 */
void CnsMapCanvas::doCanvasClicked(const QgsPoint &point,
                                  Qt::MouseButton button) {
    if (!(qgsImgLayer && qgsImgProvider) ) return;

    QgsVectorLayer* layer = rbCheckedVectorLayer();
    QListView* list = rbCheckedListView();
    if ( !layer) return;
    if ( !list ) return;
    QStringListModel* model = qobject_cast<QStringListModel*>(list->model());


    if ( !(doHandleCoords(point) && button == Qt::LeftButton) ) return;

    /*
    QString str; str.sprintf("COORD %f %f PIX %d %d",
                  point.x(), point.y(),
                  intMapCursorPixelCol, intMapCursorPixelRow
                  );
    if (out) out->log(str);
    */

    if (layer && mapMode ==  MAP_MODE_DIGITIZE ) {
        bool done = false;
        QString lyrName = layer->name();
        QString usrName = config->appUser();
        QgsFeature fet = QgsFeature(layer->dataProvider()->fields());
        fet.setGeometry( QgsGeometry::fromPoint(QgsPoint(point)));
        int ix = nextFeatureId(layer);
        fet.setFeatureId(ix);
        done = fet.setAttribute("SID",-1);
        done = fet.setAttribute("TP",lyrName);
        done = fet.setAttribute("PX",intMapCursorPixelCol);
        done = fet.setAttribute("PY",intMapCursorPixelRow);
        done = fet.setAttribute("UX",dblMapCursorUtmX);
        done = fet.setAttribute("UY",dblMapCursorUtmY);
        done = fet.setAttribute("LX",dblMapCursorLongitude);
        done = fet.setAttribute("LY",dbMapCursorLatitude);
        layer->startEditing();
        layer->addFeature(fet,true);
        layer->commitChanges();

        QString lstr;
        lstr.sprintf(FMT_CNS_LIST,
                      qPrintable(lyrName),
                      intMapCursorPixelCol, intMapCursorPixelRow,
                      dblMapCursorUtmX, dblMapCursorUtmY,
                      dblMapCursorLongitude, dbMapCursorLatitude, -1, ix);

        model->insertRow(model->rowCount());
        QModelIndex index = model->index(model->rowCount()-1);
        model->setData(index, lstr);

        if (done) out->log("..Feature "
                           +layer->name()
                           +QString::number(layer->featureCount()));
        else out->log("..Feature not done "+layer->name());
//        this->refresh();

    } else if (layer && mapMode ==  MAP_MODE_SELECT )  {
         QgsFeatureIterator iter=layer->getFeatures();
         QgsFeature fet;
         double min = 1E6; double dist =0;
         QgsFeatureId ix =-1;
         while (iter.nextFeature(fet)) {
             QgsPoint fPoint = fet.geometry()->asPoint();
             dist = sqrt(fPoint.sqrDist(point));
             if (min>dist && dist<=SELECT_DIST) {
                 min = dist;
                 ix = fet.id();
             }
         }
         if (ix>=0) layer->select(ix);
         QModelIndex index = model->index( ix-1);
         list->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    }
    doUpdateStatus();

}

// --------------------------------------------------------------------------
QgsVectorLayer *CnsMapCanvas::openEditLayer(
                                   const QString imagePath,
                                   const QString strCam,
                                   const QString strFile,
                                   const QString lyrKey,
                                   const int lyrId,
                                   QgsVectorLayer *layer) {
    if ( layer ) {
         QString id = layer->id();
         mapLayerStack->removeMapLayer(lyrKey);
    }


    QString props = QString("Point?")+
                    QString("crs=epsg:326")+
                    QString::number(config->prjUtmSector());

    layer = new QgsVectorLayer(props, lyrKey, "memory");
    if ( layer->isValid() ) {
        out->log(tr("editLayer OPEN ")+" "+lyrKey+" "+strFile);
    } else {
        out->error(tr("editLayer OPEN FAILED ")+" "+lyrKey+" "+strFile);
        return 0;
    }

    QList<QgsField> fields;
    fields.append(QgsField("SID", QVariant::Int,  "rcns_id"));
    fields.append(QgsField("TP",  QVariant::String, "typ", 3));
    fields.append(QgsField("PX",  QVariant::Int,    "px"));
    fields.append(QgsField("PY",  QVariant::Int,    "py"));
    fields.append(QgsField("UX",  QVariant::Double, "ux"));
    fields.append(QgsField("UY", QVariant::Double,  "uy"));
    fields.append(QgsField("LX",  QVariant::Double, "lx"));
    fields.append(QgsField("LY", QVariant::Double,  "ly"));

    layer->dataProvider()->addAttributes(fields);
    out->log("editLayer PROVIDER.NAME "+layer->dataProvider()->name());
    out->log("editLayer PROVIDER.CAPS "+layer->dataProvider()->capabilitiesString());

    QListView* listView = edtViews[lyrId];
    QStringListModel* model = qobject_cast<QStringListModel*>(listView->model());
    QString usr = config->appUser(); int dummy;
    db->readRawCensus(model, layer, strCam, strFile, usr, dummy);

    bool done = false;
    layer->loadSldStyle(config->symbolFileSld(lyrKey),done);
    if (!done) {
        out->error("NO STYLE");
    }
    mapLayerStack->addMapLayer(lyrKey, layer);
    return layer;
 }

// --------------------------------------------------------------------------
bool CnsMapCanvas::openRasterLayer(const QString imagePath,
                                   const QString strCam,
                                   const QString strFile) {
    if ( qgsImgLayer ) {
         QString id = qgsImgLayer->id();
         mapLayerStack->removeMapLayer("raster");
         qgsImgLayer = 0;
//         refreshLayerPaintList();
     }


    QString file =imagePath+"cam"+strCam+"/geo/"+strFile+".tif";
    QFileInfo info (file);
    if ( !info.isFile() || !info.isReadable() ) {
        out->error(tr("rasterLayer invalid file  ")+file);
        return false;
    }
    QString basePath = info.filePath();
    QString baseName = info.fileName();
    out->log(tr("rasterLayer BASE.PATH ") + basePath);
    out->log(tr("rasterLayer BASE.NAME ") + baseName);

    qgsImgLayer = new QgsRasterLayer(basePath,baseName);

    if ( qgsImgLayer->isValid() ) {
        out->log(tr("rasterLayer OPEN ")+file);
    } else {
        out->error(tr("rasterLayer OPEN FAILED ")+file);
        return false;
    }

    qgsImgProvider = qgsImgLayer->dataProvider();
    /** PROOF OF CONCEPT
    QString props = QString("Point?")+
                    QString("crs=epsg:326")+
                    QString::number(config->prjUtmSector());
    qgsImgEnvelope = db->readImageEnvelope(strCam, strFile, qgsImgEnvelope);
    QgsRectangle extR = qgsImgEnvelope->boundingBox();
    QgsRectangle extI = qgsImgProvider->extent();
    if (extR.intersects(extI)) {
      out->log("OK Envelope passt!");
    } else {
      out->log("OK Envelope passt nicht!");
    }
    */
    out->log("rasterLayer PROVIDER.NAME "+qgsImgProvider->name());
    out->log("rasterLayer PROVIDER.CAPS "+qgsImgProvider->capabilitiesString());
    dblExtMinUtmX = qgsImgLayer->extent().xMinimum();
    dblExtMaxUtmX = qgsImgLayer->extent().xMaximum();
    dblExtMinUtmY = qgsImgLayer->extent().yMinimum();
    dblExtMaxUtmY = qgsImgLayer->extent().yMaximum();
    dblUnitPixX = qgsImgLayer->rasterUnitsPerPixelX();
    dblUnitPixY = qgsImgLayer->rasterUnitsPerPixelY();

    out->log("rasterLayer West  "+QString::number(dblExtMinUtmX));
    out->log("rasterLayer East  "+QString::number(dblExtMaxUtmX));
    out->log("rasterLayer North "+QString::number(dblExtMaxUtmY));
    out->log("rasterLayer South "+QString::number(dblExtMinUtmY));
    out->log("rasterLayer UnitX "+QString::number(dblUnitPixX));
    out->log("rasterLayer UnitY "+QString::number(dblUnitPixY));


    QgsContrastEnhancement* qgsContrastEnhRed = new QgsContrastEnhancement(QGis::UInt16);
    qgsContrastEnhRed->setMinimumValue(config->imgMinRed());
    qgsContrastEnhRed->setMaximumValue(config->imgMaxRed());
    qgsContrastEnhRed->setContrastEnhancementAlgorithm ( QgsContrastEnhancement::StretchToMinimumMaximum);

    QgsContrastEnhancement* qgsContrastEnhGreen = new QgsContrastEnhancement(QGis::UInt16);
    qgsContrastEnhGreen->setMinimumValue(config->imgMinGreen());
    qgsContrastEnhGreen->setMaximumValue(config->imgMaxGreen());
    qgsContrastEnhGreen->setContrastEnhancementAlgorithm ( QgsContrastEnhancement::StretchToMinimumMaximum);

    QgsContrastEnhancement* qgsContrastEnhBlue = new QgsContrastEnhancement(QGis::UInt16);
    qgsContrastEnhBlue->setMinimumValue(config->imgMinBlue());
    qgsContrastEnhBlue->setMaximumValue(config->imgMaxBlue());
    qgsContrastEnhBlue->setContrastEnhancementAlgorithm ( QgsContrastEnhancement::StretchToMinimumMaximum);

    QgsMultiBandColorRenderer* renderer = new QgsMultiBandColorRenderer(
                qgsImgProvider,
                config->imgBandRed(),
                config->imgBandGreen(),
                config->imgBandBlue() ,
                qgsContrastEnhRed,
                qgsContrastEnhGreen,
                qgsContrastEnhBlue);

    qgsImgLayer->setRenderer( renderer );
    setExtent(qgsImgLayer->extent());
    out->debug("layerRegistry NUM.LAYER "+QString::number(qgsLyrRegistry->count()));
    crsUTM = qgsImgLayer->crs();
    if (qgsTrfm2LonLat) delete qgsTrfm2LonLat;
    qgsTrfm2LonLat = new QgsCoordinateTransform(crsUTM, crs4326);

    if (!db->readRawImage(config->prjUtmSector(),strCam, strFile,
                          config->appUser(), config->prjSession(),
                          rawImgID, rawImgTmWhen, rawImgTmSeen) ) {
        return false;
    }
    if (rawImgID<0) {
        bool done = false;
        done = db->writeRawImage(true, rawImgID,
                          config->prjUtmSector(), strCam,
                          strFile, config->appUser(), config->prjSession(),
                          rawImgTmWhen, rawImgTmSeen)
               &&

               db->readRawImage(config->prjUtmSector(), strCam, strFile,
                         config->appUser(), config->prjSession(),
                         rawImgID, rawImgTmWhen, rawImgTmSeen);
        if (! done ) return false;
    }
    rawImgTm = QDateTime::currentDateTimeUtc();
    mapLayerStack->addMapLayer("raster",qgsImgLayer,1000);
    return true;

}

bool CnsMapCanvas::openPolyLayer(QString strCam, QString strFile) {
    if ( qgsPolyLayer ) {
         QString id = qgsPolyLayer->id();
         mapLayerStack->removeMapLayer("polygon");
         qgsPolyLayer = 0;
    }
    QString uri = QString("Polygon?crs=epsg:326")+QString::number(config->prjUtmSector());
    qgsPolyLayer = new QgsVectorLayer(uri, "Polygon Layer", "memory");

    QgsGeometry *validPoly = validPolyGeometry(db, strCam, strFile);
    QgsGeometry *imgEnv	= db->readImageEnvelope(strCam, strFile);
    QgsGeometry *invPoly= imgEnv->difference(validPoly);

    QgsFeature fet = QgsFeature( qgsPolyLayer->dataProvider()->fields() );
    fet.setGeometry( invPoly );
    qgsPolyLayer->startEditing();
    qgsPolyLayer->addFeature(fet,true);
    qgsPolyLayer->commitChanges();
    bool done = false;
    qgsPolyLayer->loadNamedStyle(config->symbolFileQml("BOR"),done);
    mapLayerStack->addMapLayer("polygon",qgsVsLayer,2000);

    QgsRectangle rect = qgsPolyLayer->extent();
    rect.setXMinimum(rect.xMinimum()-10);
    rect.setYMinimum(rect.yMinimum()-10);
    rect.setXMaximum(rect.xMaximum()+10);
    rect.setYMaximum(rect.yMaximum()+10);
    setExtent(rect);

    return true;
}

#ifdef OPENCV

// -------------------------------------------------------------------------
//@TODO Korrektur 16 Bit Index 123 vs. 8 Bit Index 321
QImage CnsMapCanvas::mat2QImage8Bit(const cv::Mat3b &src) {
        QImage result(src.cols, src.rows, QImage::Format_ARGB32);
        for (int y = 0; y < src.rows; ++y) {
            const cv::Vec3b *srcrow = src[y];
                QRgb *destrow = (QRgb*)result.scanLine(y);
                for (int x = 0; x < src.cols; ++x) {
                        destrow[x] = qRgba(srcrow[x][0],
                                           srcrow[x][1],
                                           srcrow[x][2], 255);
                }
        }
        return result;
}


/** Konvertieung QgisOpenCV */

/** Conversation of a QGIS RasterLayer DataBlock and
 * store it in a OpenCV Matrix.
 *
 * Remarks:
 *   Only 3 channel byte ased raster data are accepted.
 *
 * @param point - Center position of the patch
 * @param radius - How many meters around the center position we interested
 * @param width  - Width of the CV::Mat
 * @param height  - Height of the CV::Mat
 *
 * @returns an OperationCode
 *   1 OK, the CV object is the member Variable cv::Mat *cvImageBird;
 * -10 raster data provider is empty
 * -20 less or more than 3 channels
 * -30 raster data are not byte based
 * -110 raster data are not byte based at the 1. band
 * -120 raster data are not byte based at the 2. band
 * -130 raster data are not byte based at the 3. band
 * -(2|3|4)10 raster data block invalid at the 1. band
 * -(2|3|4)20 raster data block invalid at the 2. band
 * -(2|3|4)30 raster data block invalid at the 3. band
 *     -2?0  block is valid?
 *     -3?0  block is empty?
 *     -4?0  data bits are empty?
 */
int CnsMapCanvas::mapImg2CV( const QgsPoint& point,
                             double radius, int width, int height) {

    // RasteDataProvider is valid?
    if ( !qgsImgProvider) return -10; //error code -10

    // Do we have a 3 channels?
    int numBand   = qgsImgProvider->bandCount();
    if (numBand != 3) return -20;  //error code -20

    // Do we have a byte based sampling?
    QGis::DataType dataType  = qgsImgProvider->dataType(1);
    if (! (dataType == QGis::UInt16 || dataType == QGis::Byte )) return -30; //error code -23

    // OK we have a RGB Image!
    // Create the region of interest
    QgsRectangle roi(point.x()-radius, point.y()-radius,
                     point.x()+radius, point.y()+radius);

    // OpenCV member Variable cv::Mat *cvImageBird;
    // free mem if the OPenCV image is assigned
    if (cvImageBird) delete cvImageBird;

    // Create a new on RGB
    cvImageBird = new cv::Mat3b(width, height, CV_8UC3);

    // Iterate over the bands
    for (int band = 0; band < numBand; ++band) {

        // Request data band homogenity?
        if (dataType != qgsImgProvider->dataType(band+1)) {
             return -100*(band+1)-10; // error codes -110, -120, -130
        }

        // Get the a proper raster block
        QgsRasterBlock *block = qgsImgProvider->block(band+1, roi, width, height);
        if (!block->isValid()) {
             return -200*(band+1)-10; // error codes -210, -220, -230
        }

        if (block->isEmpty())  {
             return -400*(band+1)-10; // error codes -310, -320, -330
        }

        // Get the data of the raster block
        char *bits = block->bits();
        if (! bits )  {
            return -400*(band+1)-10; // error codes -410, -420, -430
        }

        // Copy the data block into the CV Object
        for (int col=0; col<width; col++) {
            for (int row=0; row<height; row++) {
                if (dataType == QGis::UInt16) {
                  char lo = *bits; bits++; char hi = *bits;
                  quint16 word = hi | quint16(lo) << 8;
                  cvImageBird->at<cv::Vec3b>(col,row)[band] = static_cast<quint8>(word);
                } else {
                   cvImageBird->at<cv::Vec3b>(col,row)[band] = *bits;
                }
                bits++;
            }
        }

        // Free mem the qgsDataBlock
        delete block;
    } // eof for(int band...)

    // set a flag, that we have a proper result;
    return 1;
} // eof mapImg2CV(...)

#endif  /* OPENCV */

