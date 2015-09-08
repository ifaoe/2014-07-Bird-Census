#include "cnsmapcanvas.h"
#include "ovrmapcanvas.h"
#include "mainwindow.h"
#include <qgsgeometry.h>

void qDebug(QString text) {
	qDebug() << text;
}

void qWarning(QString text) {
	qWarning(text.toStdString().c_str());
}

void qCritical(QString text) {
	qCritical(text.toStdString().c_str());
}

CnsMapCanvas::CnsMapCanvas(QWidget *parent,
                           Ui::MainWindow *aUI,
                           ConfigHandler *aConfig,
                           Db* aDB,
                           QgsMapLayerRegistry *lyrRegistry) :
    QgsMapCanvas(parent),
    ui(aUI), config(aConfig), db(aDB), qgsLyrRegistry(lyrRegistry)
{
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

    // Initialisierung des ImageLayer der Karte
    qgis_image_layer_ = 0;
    qgsImgProvider = 0;
    qgsTrfm2LonLat.initialise();

    crs4326 = QgsCoordinateReferenceSystem(4326);
    setMapUnits(QGis::Meters);

    qgsLayerRegistry = QgsMapLayerRegistry::instance();

    type_marker_icon_map_["VS"] = QgsMapMarker::ICON_CROSS_BOX;
	type_marker_icon_map_["VF"] = QgsMapMarker::ICON_X_BOX;
	type_marker_icon_map_["UO"] = QgsMapMarker::ICON_X_CIRCLE;
	type_marker_icon_map_["TR"] = QgsMapMarker::ICON_CROSS_CIRCLE;
	type_marker_icon_map_["AN"] = QgsMapMarker::ICON_CROSS_DIAMOND;
	type_marker_icon_map_["MM"] = QgsMapMarker::ICON_X_DIAMOND;

    type_marker_color_map_["VS"] = Qt::green;
	type_marker_color_map_["VF"] = Qt::green;
	type_marker_color_map_["UO"] = Qt::gray;
	type_marker_color_map_["TR"] = Qt::gray;
	type_marker_color_map_["AN"] = Qt::red;
	type_marker_color_map_["MM"] = Qt::cyan;
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
    zoomToFullExtent();
}

// --------------------------------------------------------------------------
/** @brief CnsMapCanvas::doZoom1by1 auf 1:1 Maszstab zoomen */
void CnsMapCanvas::doZoom1by1() {
	qDebug() << tr("ZOOM 1:1");
    doCenter1by1(dblCurCenterUtmX, dblCurCenterUtmY);
}

// --------------------------------------------------------------------------
/**
 * @brief MainWindow::doModePan In den Pan Modus wechseln
 */
void CnsMapCanvas::doModePan() {
	qDebug() << tr("set MapMode PAN");
	map_mode_ = MAP_MODE_INSPECT;
    setMapTool(qgsToolPan);
}

// --------------------------------------------------------------------------
/**
 * @brief CnsMapCanvas::doModeDigitize() In den Digitalisierungsmodus wechseln
 */
void CnsMapCanvas::doModeDigitize() {
	ui->tbwObjects->selectionModel()->clearSelection();
	qDebug() << tr("MODUS ERFASSUNG");
    map_mode_ = MAP_MODE_DIGITIZE;
    setMapTool(qgsToolPoint);

}
// --------------------------------------------------------------------------
/**
 * @brief CnsMapCanvas::doModeSelect Punkte selektieren und inzpizieren
 */
void CnsMapCanvas::doModeSelect() {
	ui->tbwObjects->selectionModel()->clearSelection();
	qDebug() << tr("MODUS AUSWAHL");
	map_mode_ = MAP_MODE_SELECT;
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
     int w2 = this->width()*scaleFactor/2;
     int h2 = this->height()*scaleFactor/2;
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
    if (!qgis_image_layer_) return;
    QString strMode;
    switch (map_mode_) {
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
    ui->lblUTM->setText(str.sprintf("UTM%d: %010.2f %010.2f m", config->getUtmSector(),
                                    dblMapCursorUtmX, dblMapCursorUtmY));
    ui->lblMode->setText(str.sprintf("Mode: %s ",qPrintable(strMode)));
    ui->lblScale->setText(str.sprintf("Skale 1: %f", this->scale()/72.0));
    dblCurMinUtmX = extent().xMinimum();
    dblCurMinUtmY = extent().yMinimum();
    dblCurMaxUtmX = extent().xMaximum();
    dblCurMaxUtmY = extent().yMaximum();
    dblCurCenterUtmX = extent().center().x();
    dblCurCenterUtmY = extent().center().y();
    if ( (intMapCursorPixelCol < 0) || (intMapCursorPixelRow < 0) ) {
        ui->lblPixel->setText("Position ------ -----");
    } else {
        ui->lblPixel->setText(str.sprintf("Position %05d %05d",
                                          intMapCursorPixelCol,
                                          intMapCursorPixelRow));
    }
}
// --------------------------------------------------------------------------
void CnsMapCanvas::doHandleKeyPressed(QKeyEvent *keyEvent) {
    if (keyEvent->key() == Qt::Key_Control) {
        keyCtrl = true;
        qDebug() << "MCVS KEY COMMAND MODE ON";
    }
    if (keyEvent->key() == Qt::Key_Shift) {
        keyShift = true;
        qDebug() << "MCVS KEY SHIFT MODE ON";
    }
}

// --------------------------------------------------------------------------
void CnsMapCanvas::doHandleKeyReleased(QKeyEvent *keyEvent) {
    if (keyEvent->key() == Qt::Key_Control) {
        keyCtrl = false;
        qDebug() << "MCVS KEY COMMAND MODE OFF";
    }
    if (keyEvent->key() == Qt::Key_Shift) {
        keyShift = false;
        qDebug() << "MCVS KEY SHIFT MODE OF";
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

    if (!qgis_image_layer_ ) return false;
    if (!qgsTrfm2LonLat.isInitialised()) return false;
    QgsPoint pnt = qgsTrfm2LonLat.transform(point);
    lon = pnt.x(); lat = pnt.y();
   return true;
}

// --------------------------------------------------------------------------
bool CnsMapCanvas::doHandleCoords(const QgsPoint &point) {
    if (!qgis_image_layer_) return false;
    if ( doCalcPixPos(point, intMapCursorPixelCol, intMapCursorPixelRow) ) {
      dblMapCursorUtmX = point.x(); dblMapCursorUtmY = point.y();
      doCalcLonLat(point,dblMapCursorLongitude,dblMapCursorLatitude);
      doUpdateStatus();
    }
    return true;
}

// --------------------------------------------------------------------------
bool CnsMapCanvas::doCalcPixPos(const QgsPoint& point ,
                              int& pixX, int& pixY) {

    if (! qgis_image_layer_ ) return false;

    double x = point.x();
    double y = point.y();

    pixX = floor(( x - dblExtMinUtmX ) / dblUnitPixX ) ;
    pixY = floor(( dblExtMaxUtmY - y ) / dblUnitPixY ) ;

    return true;
}

// --------------------------------------------------------------------------
bool CnsMapCanvas::doCalcWorldPos(const int pixX ,const int pixY,
                                  double& wldX, double& wldY) {

    if (!qgis_image_layer_ ) return false;

    double x = pixX;
    double y = pixY;

    wldX = dblExtMinUtmX + x * dblUnitPixX;
    wldY = dblExtMaxUtmY - y * dblUnitPixY;

    return true;
}

// --------------------------------------------------------------------------
bool CnsMapCanvas::doOpenRasterLayer(QString cam, QString file) {
	bool done = true;
	done = done &&	openEditLayer(cam, file);
    done = done && openPolyLayer(cam, file);
    done = done && openRasterLayer(config->getProjectPath(), cam, file);

    refreshLayerSet();
    return done;
}

// --------------------------------------------------------------------------
bool CnsMapCanvas::doSaveData(QString cam, QString file) {
    return saveData(cam, file);
}

// --------------------------------------------------------------------------
bool CnsMapCanvas::saveData(const QString cam, const QString file) {

	// TODO: CLEANUP
    if (cam.isEmpty()) return true;

    QDateTime tmNow = QDateTime::currentDateTimeUtc();
    QString tmSeen = QString::number(rawImgTm.secsTo(tmNow));
    QString tmWhen = rawImgTm.toString(Qt::ISODate);
    db->writeRawImage(false,rawImgID, config->getUtmSector(), cam, file,
                      config->getUser(), config->getProjectId(),
                      rawImgTmWhen+" "+tmWhen, rawImgTmSeen+" "+tmSeen);
    return true;
}

// --------------------------------------------------------------------------
/**
 * @brief MainWindow::doMapCanvasClicked Einen Punkt markieren
 * @param point Geokoordinaten auf die geklickt wurden
 * @param button Mausknpf der geklickt wurde
 */
void CnsMapCanvas::doCanvasClicked(const QgsPoint &point,
                                  Qt::MouseButton button) {
    if (!(qgis_image_layer_ && qgsImgProvider) ) return;

    if ( !(doHandleCoords(point) && button == Qt::LeftButton) ) return;

    if (map_mode_ ==  MAP_MODE_DIGITIZE ) {

        int px, py;
        doCalcPixPos(point, px, py);
        double lon, lat;
        doCalcLonLat(point, lon, lat);
        double ux, uy;
        ux = point.x();
        uy = point.y();

        QString type = ui->button_group_types->checkedButton()->property("dbvalue").toString();
        db->writeRawCensus(type,config->getUtmSector(), curCam , curImg, config->getUser(),
        		config->getProjectId(),
				QString::number(py), QString::number(px),
				QString::number(ux, 'g', 15), QString::number(uy, 'g', 15),
				QString::number(lon, 'g', 15), QString::number(lat, 'g', 15));

        UpdateObjectMarkers();

    } else if (map_mode_ ==  MAP_MODE_SELECT )  {
    	SelectObjectByLocation(point);
    }
    doUpdateStatus();

}

// --------------------------------------------------------------------------
bool CnsMapCanvas::openEditLayer(const QString strCam, const QString strFile) {

	if (qgis_edit_layer_ != 0) {
		qgsLyrRegistry->removeMapLayer("EDIT");
//		delete layer;
		qgis_edit_layer_ = 0;
	}


    curCam = strCam;
    curImg = strFile;

    QString props = QString("Point?")+
                    QString("crs=epsg:326")+
                    QString::number(config->getUtmSector());

    qgis_edit_layer_ = new QgsVectorLayer(props, "EDIT", "memory");


    if ( qgis_edit_layer_->isValid() ) {
    	qDebug() << tr("editLayer OPEN ")+strFile;
    } else {
        qWarning(tr("editLayer OPEN FAILED ")+strFile);
        return 0;
    }

    return true;
 }

// --------------------------------------------------------------------------
bool CnsMapCanvas::openRasterLayer(const QString imagePath,
                                   const QString strCam,
                                   const QString strFile) {
    if ( qgis_image_layer_!=0 ) {
    	qgsLayerRegistry->removeMapLayer(qgis_image_layer_->id());
    	qgis_image_layer_ = 0;
     }


    QString file =imagePath+"cam"+strCam+"/geo/"+strFile+".tif";
    QFileInfo info (file);
    if ( !info.isFile() || !info.isReadable() ) {
        qCritical("rasterLayer invalid file  "+file);
        return false;
    }
    QString basePath = info.filePath();
    QString baseName = info.fileName();
    qDebug() << tr("rasterLayer BASE.PATH ") + basePath;
    qDebug() << tr("rasterLayer BASE.NAME ") + baseName;

    qgis_image_layer_ = new QgsRasterLayer(basePath,baseName);

    if ( qgis_image_layer_->isValid() ) {
        qDebug() << tr("rasterLayer OPEN ")+file;
    } else {
        qCritical("rasterLayer OPEN FAILED "+file);
        return false;
    }

    qgsImgProvider = qgis_image_layer_->dataProvider();

    qDebug() << "rasterLayer PROVIDER.NAME "+qgsImgProvider->name();
    qDebug() << "rasterLayer PROVIDER.CAPS "+qgsImgProvider->capabilitiesString();
    dblExtMinUtmX = qgis_image_layer_->extent().xMinimum();
    dblExtMaxUtmX = qgis_image_layer_->extent().xMaximum();
    dblExtMinUtmY = qgis_image_layer_->extent().yMinimum();
    dblExtMaxUtmY = qgis_image_layer_->extent().yMaximum();
    dblUnitPixX = qgis_image_layer_->rasterUnitsPerPixelX();
    dblUnitPixY = qgis_image_layer_->rasterUnitsPerPixelY();

    qDebug() << "rasterLayer West  "+QString::number(dblExtMinUtmX);
    qDebug() <<"rasterLayer East  "+QString::number(dblExtMaxUtmX);
    qDebug() <<"rasterLayer North "+QString::number(dblExtMaxUtmY);
    qDebug() <<"rasterLayer South "+QString::number(dblExtMinUtmY);
    qDebug() <<"rasterLayer UnitX "+QString::number(dblUnitPixX);
    qDebug() <<"rasterLayer UnitY "+QString::number(dblUnitPixY);


    QgsContrastEnhancement* qgsContrastEnhRed = new QgsContrastEnhancement(QGis::UInt16);
    qgsContrastEnhRed->setMinimumValue(config->getRedMinValue());
    qgsContrastEnhRed->setMaximumValue(config->getRedMaxValue());
    qgsContrastEnhRed->setContrastEnhancementAlgorithm (
    		QgsContrastEnhancement::StretchToMinimumMaximum);

    QgsContrastEnhancement* qgsContrastEnhGreen = new QgsContrastEnhancement(QGis::UInt16);
    qgsContrastEnhGreen->setMinimumValue(config->getGreenMinValue());
    qgsContrastEnhGreen->setMaximumValue(config->getGreenMaxValue());
    qgsContrastEnhGreen->setContrastEnhancementAlgorithm (
    		QgsContrastEnhancement::StretchToMinimumMaximum);

    QgsContrastEnhancement* qgsContrastEnhBlue = new QgsContrastEnhancement(QGis::UInt16);
    qgsContrastEnhBlue->setMinimumValue(config->getBlueMinValue());
    qgsContrastEnhBlue->setMaximumValue(config->getBlueMaxValue());
    qgsContrastEnhBlue->setContrastEnhancementAlgorithm (
    		QgsContrastEnhancement::StretchToMinimumMaximum);

    QgsMultiBandColorRenderer* renderer = new QgsMultiBandColorRenderer(
                qgsImgProvider,
                config->getRedChannel(),
                config->getGreenChannel(),
                config->getBlueChannel(),
                qgsContrastEnhRed,
                qgsContrastEnhGreen,
                qgsContrastEnhBlue);

    qgis_image_layer_->setRenderer( renderer );
    setExtent(qgis_image_layer_->extent());
    qDebug() << "layerRegistry NUM.LAYER "+QString::number(qgsLyrRegistry->count());
    crsUTM = qgis_image_layer_->crs();
    qgsTrfm2LonLat.setSourceCrs(crsUTM);
    qgsTrfm2LonLat.setDestCRS(crs4326);

    if (!db->readRawImage(strCam, strFile, config->getUser(), rawImgID, rawImgTmWhen, rawImgTmSeen) ) {
        return false;
    }
    if (rawImgID<0) {
        bool done = false;
        done = db->writeRawImage(true, rawImgID,
                          config->getUtmSector(), strCam,
                          strFile, config->getUser(), config->getProjectId(),
                          rawImgTmWhen, rawImgTmSeen)
               &&

               db->readRawImage(strCam, strFile, config->getUser(), rawImgID, rawImgTmWhen, rawImgTmSeen);
        if (! done ) return false;
    }
    rawImgTm = QDateTime::currentDateTimeUtc();

    return true;

}

bool CnsMapCanvas::openPolyLayer(QString strCam, QString strFile) {
    if ( qgis_poly_layer_!=0 ) {
    	qgsLayerRegistry->removeMapLayer(qgis_poly_layer_->id());
         qgis_poly_layer_ = 0;
    }
    QString uri = QString("Polygon?crs=epsg:326")+QString::number(config->getUtmSector());
    qgis_poly_layer_ = new QgsVectorLayer(uri, "Polygon Layer", "memory");

    QgsGeometry *validPoly = db->readValidPolygon(strCam, strFile)->buffer(1e-8,0);
    QgsGeometry *imgEnv	= db->readImageEnvelope(strCam, strFile);
    QgsGeometry *invPoly= imgEnv->difference(validPoly);

    QgsFillSymbolV2 * symbol = new QgsFillSymbolV2;
    if (config->getAdmin())
    	symbol->setAlpha(0.3);
    else
    	symbol->setAlpha(1.0);
    symbol->setColor(Qt::black);

    qgis_poly_layer_->setRendererV2(new QgsSingleSymbolRendererV2(symbol));
    qDebug() << validPoly->area();
    if (validPoly->area() > 1e3) {
		QgsFeature fet = QgsFeature( qgis_poly_layer_->dataProvider()->fields() );
		fet.setGeometry( invPoly );
		qgis_poly_layer_->startEditing();
		qgis_poly_layer_->addFeature(fet,true);
		qgis_poly_layer_->commitChanges();
    }

    QgsRectangle rect = qgis_poly_layer_->extent();
    rect.setXMinimum(rect.xMinimum()-10);
    rect.setYMinimum(rect.yMinimum()-10);
    rect.setXMaximum(rect.xMaximum()+10);
    rect.setYMaximum(rect.yMaximum()+10);
    setExtent(rect);

    return true;
}

double CnsMapCanvas::getScaleFactor() {
	return scaleFactor;
}

bool CnsMapCanvas::refreshLayerSet() {

	QList<QgsMapCanvasLayer> layerSet;


	qgsLayerRegistry->addMapLayer(qgis_edit_layer_,false,true);
	qgsLayerRegistry->addMapLayer(qgis_poly_layer_,false,true);
	qgsLayerRegistry->addMapLayer(qgis_image_layer_,false,true);

	layerSet.append(qgis_edit_layer_);
	layerSet.append(qgis_poly_layer_);
	layerSet.append(qgis_image_layer_);

	qgsLyrRegistry->reloadAllLayers();
	setLayerSet(layerSet);
	return true;
}

bool CnsMapCanvas::DeselectObjects() {
	if (current_object_selection_ > 0) {
		object_markers_[current_object_selection_]->setFill(false);
		current_object_selection_ = -1;
		return true;
	}
	 return false;
}

void CnsMapCanvas::UpdateObjectMarkers() {
	DeselectObjects();
	QSqlQueryModel * query_model = static_cast<QSqlQueryModel *>(ui->tbwObjects->model());
//	query_model->clear();
	db->UpdateObjectQuery(curCam,curImg,query_model);
//	query_model->query().exec();
	QgsMapMarker * temp_marker;
	for (int i=0; i<object_markers_.size(); i++) {
		delete object_markers_.values().at(i);
		delete object_locations_.values().at(i);
	}
	object_locations_.clear();
	object_markers_.clear();

	for (int i=0; i<query_model->rowCount(); i++) {
		int rcns_id = query_model->record(i).value("rcns_id").toInt();
		QString tp = query_model->record(i).value("tp").toString();
		double ux = query_model->record(i).value("ux").toDouble();
		double uy =query_model->record(i).value("uy").toDouble();

		temp_marker =  new QgsMapMarker(this);
		temp_marker->setCenter(QgsPoint(ux,uy));
		temp_marker->setIconType(type_marker_icon_map_[tp]);
		temp_marker->setIconColor(type_marker_color_map_[tp]);
		temp_marker->setIconSize(8);
		temp_marker->setDrawWidth(1);
//		temp_marker->setText(QString::number(rcns_id));
		object_markers_[rcns_id] = temp_marker;
		object_locations_[rcns_id] = new QgsPoint(ux,uy);
	}
	qgis_edit_layer_->reload();
	this->refresh();
	ui->tbwObjects->resizeColumnsToContents();
	ui->tbwObjects->horizontalHeader()->setStretchLastSection(true);
}

void CnsMapCanvas::UpdateObjectSelection() {
	QModelIndexList selection = ui->tbwObjects->selectionModel()->selectedRows(0);
	if (selection.size() > 1) {
		qDebug() << "CnsMapCanvas::UpdateObjectSelection: Too many selected items.";
		ui->tbwObjects->selectionModel()->clearSelection();
		return;
	}
	if (selection.size() == 0) return;
	QModelIndex index = selection.at(0);
	int rcns_id = ui->tbwObjects->model()->data(index).toInt();
	qDebug() << "CnsMapCanvas::UpdateObjectSelection: Update object selection. ID: " << rcns_id;
	object_markers_[rcns_id]->setFill(true);
}

bool CnsMapCanvas::SelectObjectByLocation(const QgsPoint & point) {
	int min_id = -1;
	double min_dist = 1.0;
	for (QMap<int, QgsPoint*>::Iterator it = object_locations_.begin(); it!=object_locations_.end(); ++it) {
		double cur_dist = it.value()->sqrDist(point);
		if (cur_dist < config->getSelectDistance() && cur_dist < min_dist) {
			min_dist = cur_dist;
			min_id = it.key();
		}
	}
	if (min_id > 0) {
		QSqlQueryModel * model = static_cast<QSqlQueryModel *>(ui->tbwObjects->model());
		for (int i=0; i<model->rowCount(); i++) {
			QModelIndex index = model->index(i,0);
			if (model->data(index).toInt() == min_id) {
				ui->tbwObjects->selectRow(index.row());
				object_markers_[min_id]->setFill(true);
				current_object_selection_ = min_id;
				return true;
			}
		}
		current_object_selection_ = -1;
	}
	return false;
}

bool CnsMapCanvas::SelectObjectById(int rcns_id) {
	QSqlQueryModel * model = static_cast<QSqlQueryModel *>(ui->tbwObjects->model());
	for (int i=0; i<model->rowCount(); i++) {
		QModelIndex index = model->index(i,0);
		if (model->data(index).toInt() == rcns_id) {
			object_markers_[rcns_id]->setFill(true);
			current_object_selection_ = rcns_id;
			return true;
		}
	}
	current_object_selection_ = -1;
	return false;
}

void CnsMapCanvas::HideMarkers(bool hide) {
	for (int i=0; i<object_markers_.size();i++) {
		if (hide)
			object_markers_.values().at(i)->hide();
		else
			object_markers_.values().at(i)->show();
	}
}
