
#include <QStyle>
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

	ui->tbwObjects->setColumnWidth(0, 75);
	ui->tbwObjects->setColumnWidth(1, 50);
	ui->tbwObjects->setColumnWidth(2, 50);
	ui->tbwObjects->setColumnWidth(3, 50);
	ui->tbwObjects->setColumnWidth(4, 50);
	ui->tbwObjects->setColumnWidth(5, 50);
	ui->tbwObjects->setColumnWidth(6, 50);
	ui->tbwObjects->setColumnWidth(7, 50);
	ui->tbwObjects->setColumnWidth(8, 50);
	ui->tbwObjects->hideColumn(5);
	ui->tbwObjects->hideColumn(6);
	ui->tbwObjects->hideColumn(7);
	ui->tbwObjects->hideColumn(8);
	ui->tbwObjects->horizontalHeader()->setStretchLastSection(true);

	initFilters();
	initSessionFrame();

    object_query_model_ = new QSqlQueryModel;
    ui->tbwObjects->setModel(object_query_model_);

    imgSelector = ui->tbvImages->selectionModel();
    ui->tbvImages->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tbvImages->setSelectionBehavior(QAbstractItemView::SelectRows);

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
             SLOT(imgUpdateSelection()));

    connect( objSelector,
             SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             this,
             SLOT(objUpdateSelection()));

    connect( ui->cmbSession, SIGNAL(currentIndexChanged(int)),
    		this, SLOT(handleSessionSelection()));

    connect( ui->chbHideMarker, SIGNAL(clicked(bool)), this, SLOT(hideMarker(bool)));
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
}

// ----------------------------------------------------------------------
void MainWindow::deleteSelection() {
	QModelIndex index = objSelector->selectedRows(0).at(0);
	int rcns_id = ui->tbwObjects->model()->data(index).toInt();
	db->deleteRawCensus(rcns_id, selected_cam_, selected_file_, config->appUser());
	mapCanvas->UpdateObjectMarkers();
}

// ----------------------------------------------------------------------
void MainWindow::clearSelection() {
	mapCanvas->DeselectObjects();
	objSelector->reset();
}

void MainWindow::hideMarker(bool checked) {
	mapCanvas->HideMarkers(checked);
}

// ----------------------------------------------------------------------
void MainWindow::objUpdateSelection() {
	if (objSelector->selectedRows().isEmpty()) return;
    QModelIndex index = objSelector->selectedRows(0).at(0);
    ui->tbwObjects->scrollTo(index);
    ui->btnMapRmObj->setEnabled(false);

    mapCanvas->DeselectObjects();

    int rcns_id =  object_query_model_->data(objSelector->selectedRows(0).at(0)).toInt();
    QString user = object_query_model_->data(objSelector->selectedRows(1).at(0)).toString();
    QString type = object_query_model_->data(objSelector->selectedRows(2).at(0)).toString();
	double utm_x = object_query_model_->data(objSelector->selectedRows(3).at(0)).toDouble();
	double utm_y = object_query_model_->data(objSelector->selectedRows(4).at(0)).toDouble();

	mapCanvas->doCenter1by1(utm_x,utm_y);
	if (mapCanvas->map_mode() == MAP_MODE_SELECT) {
		mapCanvas->SelectObjectById(rcns_id);
		if (user == config->appUser() || config->getAdmins().contains(config->appUser()))
			ui->btnMapRmObj->setEnabled(true);
	}
}

// ----------------------------------------------------------------------
void MainWindow::imgUpdateSelection()
 {
	if (imgSelector->selectedIndexes().isEmpty())
		return;

	objSelector->clearSelection();
	int currentRow = imgSelector->selectedRows().at(0).row();
	selected_file_ = QString(ui->tbvImages->item(currentRow, 2)->text());
	selected_cam_  = QString(ui->tbvImages->item(currentRow, 1)->text());

	  ui->chbHideMarker->setChecked(false);
	  if (!mapCanvas->doSaveData(config->curCam, config->curImg)) {
		  QMessageBox::critical(this,"Fehler","Fehler beim Sichern der Daten fuer"
								"Metadaten fuer "+config->curCam+" Kamera "+config->curImg+
								"\n konnte nicht gesichert werden!",
							   "OK");
		  imgSelector->clearSelection();
		  return;
	  }
	  if (!mapCanvas->doOpenRasterLayer(selected_cam_, selected_file_)) {
		  QMessageBox::critical(this,"Fehler beim Laden Bilddatei",
		  "Bild "+selected_file_+" Kamera "+selected_cam_+
		  "\n konnte nicht geoeffnet werden!","OK");
		  imgSelector->clearSelection();
		  return;
	  }
	  if (!ovrCanvas->openImageEnvelope(selected_cam_, selected_file_, mapCanvas->extent())) {
		  QMessageBox::critical(this,"Fehler beim Laden der Envelope",
		  "Envelope fuer Bild "+selected_file_+" Kamera "+selected_cam_+
		  "\n konnte nicht geoeffnet werden!","OK");
		  imgSelector->clearSelection();
		  return;
	  }
	  if (!ovrCanvas->openImageTiles(selected_cam_, selected_file_)) {
		  QMessageBox::critical(this,"Fehler beim Laden der Tiles",
		  "Tiles fuer Bild "+selected_file_+" Kamera "+selected_cam_+
		  "\n konnte nicht geoeffnet werden!","OK");
		  imgSelector->clearSelection();
		  return;
	  }
	  this->setWindowTitle(config->appTitle()+" - "+config->appVersion()+" - Kamera "
			  + selected_cam_ +" - "+selected_file_);
	  ovrCanvas->doSelectFirstTile();

	  config->curCam = selected_cam_;
	  config->curImg = selected_file_;

	  ui->tbxTasks->setCurrentIndex(1);
	  mapCanvas->setFocus();
	  mapCanvas->doSetupEditModus();

	  db->UpdateObjectQuery(selected_cam_, selected_file_, object_query_model_);
	  mapCanvas->UpdateObjectMarkers();

 }
// ----------------------------------------------------------------------

bool MainWindow::checkButtonByKey(QString tp) {
	QList<QAbstractButton*> btnList = ui->button_group_types->buttons();
	for (int i=0; i<btnList.size(); i++) {
		if (btnList[i]->property("dbvalue").toString() == tp) {
			btnList[i]->setChecked(true);
			return true;
		}
	}
	QAbstractButton * cbtn = ui->button_group_types->checkedButton();
	if ( cbtn != 0 )
		cbtn->setChecked(false);
	return false;
}

void MainWindow::initSessionFrame() {
	combobox_cam_filter_->setEnabled(false);
	combobox_trac_filter_->setEnabled(false);
	ui->cmbSession->addItem("");
	ui->cmbSession->addItems(db->getSessionList());
}

void MainWindow::handleSessionSelection() {
	config->setPrjSession(ui->cmbSession->currentText());
	if (config->prjSession().isEmpty())
		return;
    project * prj = db->getSessionParameters(config->prjSession());
    //TODO: set Paramters from project struct or use references
    config->setPrjType(prj->session_type);
    config->setPrjFlight(prj->flight_id);
    config->setPrjFilter(prj->filter);
    config->setPrjUtmSector(prj->utmSector);
    config->setPrjPath(prj->path);
    delete prj;
    combobox_cam_filter_->clear();
    combobox_trac_filter_->clear();
    combobox_cam_filter_->setEnabled(true);
    combobox_trac_filter_->setEnabled(true);
	combobox_cam_filter_->addItem("");
	combobox_cam_filter_->addItems(db->getCamList(config->prjFlight()));
	combobox_cam_filter_->setCurrentIndex(0);
	combobox_trac_filter_->addItem("");
	combobox_trac_filter_->addItems(db->getTrcList(config->prjFlight()));
	combobox_trac_filter_->setCurrentIndex(0);
	db->getImages(ui->tbvImages, config->prjType(), "TRUE", ui->chbNotReady->isChecked());
}

void MainWindow::initFilters() {
	ui->tbwFilters->setColumnCount(3);
	ui->tbwFilters->setColumnWidth(0,50);
	ui->tbwFilters->setColumnWidth(1,50);
	ui->tbwFilters->setHorizontalHeaderLabels(QStringList() << "TRC" << "CAM" << "IMG");
	ui->tbwFilters->horizontalHeader()->setStretchLastSection(true);

	combobox_cam_filter_ = new QComboBox();
	combobox_trac_filter_ = new QComboBox();
	lineedit_image_filter_ = new QLineEdit();

	ui->tbwFilters->setCellWidget(0,0,combobox_trac_filter_);
	ui->tbwFilters->setCellWidget(0,1,combobox_cam_filter_);
	ui->tbwFilters->setCellWidget(0,2,lineedit_image_filter_);

    connect( combobox_cam_filter_, SIGNAL(currentIndexChanged(int)), this, SLOT(handleCamFilter()));
    connect( combobox_trac_filter_, SIGNAL(currentIndexChanged(int)), this, SLOT(handleTrcFilter()));
    connect( lineedit_image_filter_, SIGNAL(returnPressed()), this, SLOT(handleImgFilter()));
    connect( ui->chbNotReady, SIGNAL(stateChanged(int)), this, SLOT(handleMissingCheckBox()));
}

void MainWindow::handleCamFilter() {
	// if last value (all cameras) is selected show all columns
	// else only show selected column
	QString cam = combobox_cam_filter_->currentText();
	if(!cam.isEmpty()) {
		cam_filter_ = " AND cam LIKE '" + cam + "'";
	} else {
		cam_filter_ = "";
	}
	db->getImages(ui->tbvImages, config->prjType(), getFilterString(), ui->chbNotReady->isChecked());
}

void MainWindow::handleTrcFilter() {
	QString trc = combobox_trac_filter_->currentText();
	if (!trc.isEmpty()) {
		trac_filter_ = " AND trc=" + trc;
	} else {
		trac_filter_ = "";
	}
	db->getImages(ui->tbvImages, config->prjType(), getFilterString(), ui->chbNotReady->isChecked());
}

void MainWindow::handleImgFilter() {
	QString img = lineedit_image_filter_->text();
	if (!img.isEmpty()) {
		if (img.startsWith("hd",Qt::CaseInsensitive))
			image_filter_ = " AND img LIKE 'HD" + img.remove(0,2) + "'";
		else
			image_filter_ = " AND img LIKE 'HD" + img + "'";
	} else {
		image_filter_ = "";
	}

	db->getImages(ui->tbvImages, config->prjType(), getFilterString(), ui->chbNotReady->isChecked());
}

QString MainWindow::getFilterString() {
	return "TRUE" + cam_filter_ + trac_filter_ + image_filter_;
}

void MainWindow::handleMissingCheckBox() {
	db->getImages(ui->tbvImages, config->prjType(), getFilterString(), ui->chbNotReady->isChecked());
}

QAbstractButton * MainWindow::GetButtonByKey(QButtonGroup * button_group, QString key, QString value) {
	QList<QAbstractButton*> button_list = button_group->buttons();
	for (int i=0; i<button_list.size(); i++) {
		if (button_list[i]->property(key.toStdString().c_str()).toString() == value) {
			return button_list[i];
		}
	}
	return NULL;
}

void MainWindow::RefreshObjectList() {
	object_query_model_->query().exec();
}
