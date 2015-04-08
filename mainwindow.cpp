#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "db.h"

MainWindow::MainWindow(AppConfig *aConfig, Db * aDb)
    : QMainWindow(0), config(aConfig), ui(new Ui::MainWindow), db(aDb)
{
	qDebug() << "Main Window Construct";
    // Aufsetzen der GUI Grundumgebung + Logger
    ui->setupUi(this);
    this->resize(QSize(config->guiWinWidth(),config->guiWinHeight()));
    this->move(config->guiWinLeft(),config->guiWinTop());
    this->setWindowTitle(config->appTitle()+" - "+config->appVersion());

    // Der QGIS Umgebung
    qgsPvrRegistry = QgsProviderRegistry::instance();
    qgsLyrRegistry = QgsMapLayerRegistry::instance();
    qgsCheckProviders();

    guiInitAdditionals();

    ui->tbwObjects->setColumnCount(9);
	ui->tbwObjects->setHorizontalHeaderLabels(
			QStringList() << "ID" << "TP" << "UX" << "UY" << "LX" << "LY" << "PX" << "PY" << "USER");
	ui->tbwObjects->setColumnWidth(0, 75);
	ui->tbwObjects->setColumnWidth(1, 50);
	ui->tbwObjects->setColumnWidth(2, 50);
	ui->tbwObjects->setColumnWidth(3, 50);
	ui->tbwObjects->setColumnWidth(4, 50);
	ui->tbwObjects->setColumnWidth(5, 50);
	ui->tbwObjects->setColumnWidth(6, 50);
	ui->tbwObjects->setColumnWidth(7, 50);
	ui->tbwObjects->setColumnWidth(8, 50);
	ui->tbwObjects->hideColumn(2);
	ui->tbwObjects->hideColumn(4);
	ui->tbwObjects->hideColumn(5);
	ui->tbwObjects->hideColumn(6);
	ui->tbwObjects->hideColumn(7);
	ui->tbwObjects->hideColumn(3);
	ui->tbwObjects->horizontalHeader()->setStretchLastSection(true);


    db->getImages(ui->tbvImages, config->prjType());
    imgSelector = ui->tbvImages->selectionModel();
    ui->tbvImages->setSelectionMode(QAbstractItemView::SingleSelection);

    objSelector = ui->tbwObjects->selectionModel();
    ui->tbwObjects->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tbwObjects->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect( ui->btnMapSelect,
             SIGNAL(clicked()),
             this,
             SLOT(clearSelection()));

    connect( ui->btnMapRmObj,
             SIGNAL(clicked()),
             this,
             SLOT(deleteSelection()));

    connect( imgSelector,
             SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             this,
             SLOT(imgUpdateSelection(QItemSelection,QItemSelection)));

    connect( objSelector,
             SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             this,
             SLOT(objUpdateSelection()));

    connect( ui->cmbSession, SIGNAL(currentIndexChanged(int)),
    		this, SLOT(handleSessionSelection()));

    connect( ui->cmbCam, SIGNAL(currentIndexChanged(int)), this, SLOT(handleCamSelection()));

    connect( ui->cmbTrc, SIGNAL(currentIndexChanged(int)), this, SLOT(handleTrcSelection()));

    connect( ui->chbHideMarker, SIGNAL(clicked(bool)), this, SLOT(hideMarker(bool)));
    connect( ui->btgLayers, SIGNAL(buttonClicked(int)), this, SLOT(rbToggledType()));

//	TODO
//    connect( ui ->tbxLayers,SIGNAL(currentChanged(int)),
//             this, SLOT(changeEdit(int)));



}

// ------------------------------------------------------------------------
void MainWindow::saveData() {
   mapCanvas->doSaveData(config->curCam, config->curImg);
   ovrCanvas->saveRawTile(false);
}

// ------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
    delete db;
}

// ------------------------------------------------------------------------
void MainWindow::qgsCheckProviders() {
    qDebug() << "QGIS PREFIX PATH IS "+config->qgsPrefixPath();
    QString msg;
    qDebug() << "OPEN PROVIDER LIST";
    QStringList providers = qgsPvrRegistry->providerList();
    qDebug() << QString("PROVIDERS %1 FOUND").arg(providers.size());
    if (providers.size()==0) {
        qFatal("%s\n%s\n qgis : {\n  prefixPath=%s; \n  ... \n}",
               "Die Providerliste ist leer.",
               MNDW_ERR_MISSING_HINT_PFX,
               qPrintable(config->qgsPrefixPath()) );
    }
    bool doneGdal = false;
    bool doneOgr = false;
    bool donePGIS = false;
    for (int i=0; i<providers.size(); ++i) {
        if (!doneGdal) { doneGdal = (providers.at(i) == QString("gdal")); }
        if (!doneOgr)  { doneOgr  = (providers.at(i) == QString("ogr")); }
        if (!donePGIS) { donePGIS = (providers.at(i) == QString("postgres")); }
        qDebug() << msg.sprintf("PROVIDER %0d: %s", (i+1),
                                providers.at(i).toUtf8().data());
    }
    if (!doneGdal) {
        qFatal(MNWD_ERR_MISSING_PROVIDER, "GDAL",
               MNDW_ERR_MISSING_HINT_PFX);
    } else { 	qDebug() << "TEST PROVIDER GDAL OK";  }

    if (!doneOgr) {
        qFatal(MNWD_ERR_MISSING_PROVIDER, "OGR",
               MNDW_ERR_MISSING_HINT_PFX);
    } else {  qDebug() << "TEST PROVIDER OGR OK"; }

    if (!donePGIS) {
        qFatal(MNWD_ERR_MISSING_PROVIDER, "Postgresql/PostGIS",
               MNDW_ERR_MISSING_HINT_PFX);
     } else {  qDebug() << "TEST PROVIDER POSTGRESQL/POSTGIS OK"; }

}


// ----------------------------------------------------------------------
void MainWindow::guiInitAdditionals() {

    QVBoxLayout *lytFrmImg = new QVBoxLayout;
    lytFrmImg->setMargin(0);
    mapCanvas = new CnsMapCanvas(this, ui, config, db, qgsLyrRegistry);
    lytFrmImg->addWidget(mapCanvas);
    ui->wdgMap->setLayout(lytFrmImg);

    QVBoxLayout *lytFrmOvr = new QVBoxLayout;
    lytFrmOvr->setMargin(0);
    ovrCanvas = new OvrMapCanvas(this, ui, config, db, mapCanvas, qgsLyrRegistry);
    lytFrmOvr->addWidget(ovrCanvas);
    ui->wdgOverview->setLayout(lytFrmOvr);

    connect(ui->btnPrevTile, SIGNAL(clicked()),
            ovrCanvas, SLOT( doSelectPrevTile()));

    connect(ui->btnNextTile, SIGNAL(clicked()),
            ovrCanvas, SLOT( doSelectNextTile()));

    connect( ui->btnDoSave,SIGNAL(clicked()),
             this, SLOT(saveData()));


}

// ----------------------------------------------------------------------
void MainWindow::deleteSelection() {
	int currentRow = objSelector->selectedRows().at(0).row();
	int rcns_id = ui->tbwObjects->item(currentRow,0)->text().toInt();
	db->deleteRawCensus(rcns_id, selCam, selFile);
	db->readRawCensus(ui->tbwObjects, selCam, selFile, config->appUser());
}

// ----------------------------------------------------------------------
void MainWindow::clearSelection() {
	mapCanvas->removeSelection();
	objSelector->reset();
}

void MainWindow::rbToggledType() {
	clearSelection();
	edtCurKey = ui->btgLayers->checkedButton()->property("dbvalue").toString();
}

void MainWindow::hideMarker(bool checked) {
	int trans;
	if (checked)
		trans = 100;
	else
		trans = 0;
	for (auto it = config->edtLayers->begin(); it!=config->edtLayers->end(); ++it) {
		it.value()->setLayerTransparency(trans);
	}
	mapCanvas->refresh();
}

// ----------------------------------------------------------------------
void MainWindow::objUpdateSelection() {
	if (objSelector->selectedRows().isEmpty()) return;
	int currentRow = objSelector->selectedRows().at(0).row();
    QModelIndex index = objSelector->model()->index(currentRow, 0);
    ui->tbwObjects->scrollTo(index);
    ui->btnMapRmObj->setEnabled(false);
    QString tp = ui->tbwObjects->item(currentRow, 1)->text();
    checkButtonByKey(tp);
    QgsVectorLayer * lyr = config->edtLayers->value(tp);
    mapCanvas->removeSelection();

    int id = ui->tbwObjects->item(currentRow, 0)->text().toInt();
	double mx = ui->tbwObjects->item(currentRow, 2)->text().toDouble();
	double my = ui->tbwObjects->item(currentRow, 3)->text().toDouble();
	mapCanvas->doCenter1by1(mx,my);
	if (mapCanvas->getMapMode() == MAP_MODE_SELECT) {
		QgsFeatureIterator it = lyr->dataProvider()->getFeatures();
		QgsFeature fet;
		while (it.nextFeature(fet))
			if(fet.attribute("SID").toInt() == id)
				lyr->select(fet.id());
		ui->btnMapRmObj->setEnabled(true);
	}
}

// ----------------------------------------------------------------------
void MainWindow::imgUpdateSelection(QItemSelection selected,
     QItemSelection deselected)
 {
	Q_UNUSED(deselected);
	objSelector->clearSelection();
	ui->tbwObjects->clearContents();

     QModelIndexList selItems = selected.indexes();
     if (selItems.size()<1 ) return;
     QModelIndex selIndex = selItems.at(0);
     selFile = QString(ui->tbvImages->model()->data(selIndex).toString());
     selCam  = QString::number(selIndex.column()-1);

	  ui->chbHideMarker->setChecked(false);
	  if (!mapCanvas->doSaveData(curCam, curFile)) {
		  QMessageBox::critical(this,"Fehler beim Sichern der Daten fuer"
								"Metadaten fuer "+curFile+" Kamera "+curCam+
								"\n konnte nicht gesichert werden!",
							   "OK");
		  imgSelector->setCurrentIndex(selIndex,
									   QItemSelectionModel::Deselect);
		  return;
	  }
	  if (!mapCanvas->doOpenRasterLayer(selCam, selFile)) {
		  QMessageBox::critical(this,"Fehler beim Laden Bilddatei",
		  "Bild "+selFile+" Kamera "+selCam+
		  "\n konnte nicht geoeffnet werden!","OK");
		  imgSelector->setCurrentIndex(selIndex,
									   QItemSelectionModel::Deselect);
		  return;
	  }
	  if (!ovrCanvas->openImageEnvelope(selCam, selFile, mapCanvas->extent())) {
		  QMessageBox::critical(this,"Fehler beim Laden der Envelope",
		  "Envelope fuer Bild "+selFile+" Kamera "+selCam+
		  "\n konnte nicht geoeffnet werden!","OK");
		  return;
	  }
	  if (!ovrCanvas->openImageTiles(selCam, selFile)) {
		  QMessageBox::critical(this,"Fehler beim Laden der Tiles",
		  "Tiles fuer Bild "+selFile+" Kamera "+selCam+
		  "\n konnte nicht geoeffnet werden!","OK");
		  return;
	  }
	  this->setWindowTitle(config->appTitle()+" - "+config->appVersion()+" - Kamera "
			  + selCam +" - "+selFile);
	  ovrCanvas->doSelectFirstTile();

	  ui->tbxTasks->setCurrentIndex(1);
	  mapCanvas->setFocus();
	  mapCanvas->doSetupEditModus();

	  db->readRawCensus(ui->tbwObjects, selCam, selFile, config->appUser());
 }
// ----------------------------------------------------------------------

bool MainWindow::checkButtonByKey(QString tp) {
	QList<QAbstractButton*> btnList = ui->btgLayers->buttons();
	for (int i=0; i<btnList.size(); i++) {
		if (btnList[i]->property("dbvalue").toString() == tp) {
			btnList[i]->setChecked(true);
			return true;
		}
	}
	QAbstractButton * cbtn = ui->btgLayers->checkedButton();
	if ( cbtn != 0 )
		cbtn->setChecked(false);
	return false;
}

void MainWindow::initSessionFrame() {
	ui->cmbCam->setEnabled(false);
	ui->cmbTrc->setEnabled(false);
	ui->cmbSession->addItems(db->getSessionList());
}

void MainWindow::handleSessionSelection() {
	QString session = ui->cmbSession->currentText();
    project * prj = db->getSessionParameters(session);
    config->setPrjSession(session);
    delete prj;
}

void MainWindow::handleCamSelection() {
	// if last value (all cameras) is selected show all columns
	// else only show selected column
}

void MainWindow::handleTrcSelection() {

}
