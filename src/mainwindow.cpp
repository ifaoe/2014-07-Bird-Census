
#include <QStyle>
#include <QInputDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "db.h"
#include "ServerSelection.h"

MainWindow::MainWindow(ConfigHandler *aConfig, Db * aDb)
    : QMainWindow(0), config(aConfig), ui(new Ui::MainWindow), db(aDb)
{
	qDebug() << "Main Window Construct";
    // Aufsetzen der GUI Grundumgebung + Logger
    ui->setupUi(this);
    resize(config->getAppSize());
    move(config->getAppPosition());


    this->setWindowTitle("BirdCensus");

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
	ui->tbwObjects->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	ui->tbwObjects->horizontalHeader()->setStretchLastSection(true);
	ui->tbwObjects->setBackgroundRole(QPalette::Base);
	ui->tbwObjects->setAutoFillBackground( true );

//	QPalette dark_palette = ui->tbwObjects->palette();
//	ui->tbwObjects->viewport()->setBackgroundRole(QPalette::AlternateBase);
////	dark_palette.setColor(QPalette::Window, Qt::lightGray);
//	dark_palette.setColor(QPalette::AlternateBase, Qt::gray);
//	ui->tbwObjects->setPalette(dark_palette);

	initFilters();
	initSessionFrame();

	filter_map["std"] = "TRUE";

    object_query_model = new QSqlQueryModel;
    ui->tbwObjects->setModel(object_query_model);

    imgSelector = ui->image_table->selectionModel();
    ui->image_table->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->image_table->setSelectionBehavior(QAbstractItemView::SelectRows);

    objSelector = ui->tbwObjects->selectionModel();
    ui->tbwObjects->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tbwObjects->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect( ui->btnMapSelect, SIGNAL(clicked()), this, SLOT(clearSelection()));

    connect( ui->btnMapRmObj, SIGNAL(clicked()), this, SLOT(deleteSelection()));

    connect( imgSelector, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(imgUpdateSelection()));

    connect( objSelector, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(objUpdateSelection()));

    connect( ui->cmbSession, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSessionSelection()));

    connect( ui->chbHideMarker, SIGNAL(clicked(bool)), this, SLOT(hideMarker(bool)));

    connect(ui->option_admin, SIGNAL(clicked()), this, SLOT(handleAdminPass()));

    connect(ui->option_server, SIGNAL(clicked()),this, SLOT(handleServerSelection()));
}

// ------------------------------------------------------------------------
void MainWindow::saveData() {
   mapCanvas->doSaveData(config->current_cam, config->current_image);
   ovrCanvas->saveRawTile(false);
	config->setAppPosition(pos());
	config->setAppSize(size());
	config->setAppMaximized(isMaximized());
	config->sync();
}

// ------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

// ------------------------------------------------------------------------
void MainWindow::qgsCheckProviders() {
    qDebug() << "QGIS PREFIX PATH IS "+config->getQGisPrefixPath();
    QString msg;
    qDebug() << "OPEN PROVIDER LIST";
    QStringList providers = qgsPvrRegistry->providerList();
    qDebug() << QString("PROVIDERS %1 FOUND").arg(providers.size());
    if (providers.size()==0) {
        qFatal("%s\n%s\n qgis : {\n  prefixPath=%s; \n  ... \n}",
               "Die Providerliste ist leer.",
               MNDW_ERR_MISSING_HINT_PFX,
               qPrintable(config->getQGisPrefixPath()) );
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
	db->deleteRawCensus(rcns_id, selected_cam, selected_file, config->getUser());
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

    int rcns_id =  object_query_model->data(objSelector->selectedRows(0).at(0)).toInt();
    QString user = object_query_model->data(objSelector->selectedRows(1).at(0)).toString();
    QString type = object_query_model->data(objSelector->selectedRows(2).at(0)).toString();
	double utm_x = object_query_model->data(objSelector->selectedRows(3).at(0)).toDouble();
	double utm_y = object_query_model->data(objSelector->selectedRows(4).at(0)).toDouble();

	mapCanvas->doCenter1by1(utm_x,utm_y);
	if (mapCanvas->map_mode() == MAP_MODE_SELECT) {
		mapCanvas->SelectObjectById(rcns_id);
		if (user == config->getUser() || config->getAdmin())
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
	selected_file = QString(ui->image_table->item(currentRow, 2)->text());
	selected_cam  = QString(ui->image_table->item(currentRow, 1)->text());

	  ui->chbHideMarker->setChecked(false);
	  if (!mapCanvas->doSaveData(config->current_cam, config->current_image)) {
		  QMessageBox::critical(this,"Fehler","Fehler beim Sichern der Daten fuer"
								"Metadaten fuer "+config->current_cam+" Kamera "+config->current_image+
								"\n konnte nicht gesichert werden!",
							   "OK");
		  imgSelector->clearSelection();
		  return;
	  }
	  if (!mapCanvas->doOpenRasterLayer(selected_cam, selected_file)) {
		  QMessageBox::critical(this,"Fehler beim Laden Bilddatei",
		  "Bild "+selected_file+" Kamera "+selected_cam+
		  "\n konnte nicht geoeffnet werden!","OK");
		  imgSelector->clearSelection();
		  return;
	  }
	  if (!ovrCanvas->openImageEnvelope(selected_cam, selected_file, mapCanvas->extent())) {
		  QMessageBox::critical(this,"Fehler beim Laden der Envelope",
		  "Envelope fuer Bild "+selected_file+" Kamera "+selected_cam+
		  "\n konnte nicht geoeffnet werden!","OK");
		  imgSelector->clearSelection();
		  return;
	  }
	  if (!ovrCanvas->openImageTiles(selected_cam, selected_file)) {
		  QMessageBox::critical(this,"Fehler beim Laden der Tiles",
		  "Tiles fuer Bild "+selected_file+" Kamera "+selected_cam+
		  "\n konnte nicht geoeffnet werden!","OK");
		  imgSelector->clearSelection();
		  return;
	  }
	  this->setWindowTitle("BirdCensus - Kamera "
			  + selected_cam +" - "+selected_file);
	  ovrCanvas->doSelectFirstTile();

	  config->current_cam = selected_cam;
	  config->current_image = selected_file;

	  ui->tbxTasks->setCurrentIndex(1);
	  mapCanvas->setFocus();
	  mapCanvas->doSetupEditModus();

	  db->UpdateObjectQuery(selected_cam, selected_file, object_query_model);
	  ui->tbwObjects->hideColumn(3);
	  ui->tbwObjects->hideColumn(4);
	  ui->tbwObjects->hideColumn(5);
	  ui->tbwObjects->hideColumn(6);
	  mapCanvas->UpdateObjectMarkers();
	  mapCanvas->setEnabled(true);
	  ovrCanvas->setEnabled(true);
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
	combobox_cam_filter->setEnabled(false);
	combobox_trac_filter->setEnabled(false);
	ui->cmbSession->clear();
	ui->cmbSession->addItem("");
	ui->cmbSession->addItems(db->getSessionList());
}

void MainWindow::handleSessionSelection() {
	config->setProjectId(ui->cmbSession->currentText());
	if (config->getProjectId().isEmpty())
		return;
    project * prj = db->getSessionParameters(config->getProjectId());
    config->setFlightId(prj->flight_id);
    config->setUtmSector(prj->utmSector);
    config->setProjectPath(prj->path);
    delete prj;
    combobox_cam_filter->clear();
    combobox_trac_filter->clear();
    combobox_cam_filter->setEnabled(true);
    combobox_trac_filter->setEnabled(true);
	combobox_cam_filter->addItem("");
	combobox_cam_filter->addItems(db->getCamList(config->getFlightId()));
//	combobox_cam_filter->setMinimumWidth(combobox_cam_filter->minimumSize().width());
	combobox_cam_filter->setCurrentIndex(0);
	combobox_trac_filter->addItem("");
	combobox_trac_filter->addItems(db->getTrcList(config->getFlightId()));
	combobox_trac_filter->setCurrentIndex(0);

	db->getImages(ui->image_table, getFilterString(), ui->chbNotReady->isChecked());
}

void MainWindow::initFilters() {
	ui->image_table->setColumnCount(3);
	ui->image_table->setColumnWidth(0,60);
	ui->image_table->setColumnWidth(1,60);
	ui->image_table->setHorizontalHeaderLabels(QStringList() << "TRC" << "CAM" << "IMG");
	ui->image_table->horizontalHeader()->setStretchLastSection(true);

	combobox_cam_filter = new QComboBox();
	combobox_trac_filter = new QComboBox();
	lineedit_image_filter = new QLineEdit();

	ui->image_table->setCellWidget(0,0,combobox_trac_filter);
	ui->image_table->setCellWidget(0,1,combobox_cam_filter);
	ui->image_table->setCellWidget(0,2,lineedit_image_filter);

    connect( combobox_cam_filter, SIGNAL(currentIndexChanged(int)), this, SLOT(handleCamFilter()));
    connect( combobox_trac_filter, SIGNAL(currentIndexChanged(int)), this, SLOT(handleTrcFilter()));
    connect( lineedit_image_filter, SIGNAL(returnPressed()), this, SLOT(handleImgFilter()));
    connect( ui->chbNotReady, SIGNAL(stateChanged(int)), this, SLOT(handleMissingCheckBox()));
}

/*
 * TODO: Handle filters in one procedure. Also use maps.
 */

void MainWindow::handleCamFilter() {
	// if last value (all cameras) is selected show all columns
	// else only show selected column
	QString cam = combobox_cam_filter->currentText();
	if(!cam.isEmpty()) {
		filter_map["cam"] = "cam='" + cam + "'";
	} else {
		filter_map.remove("cam");
	}

	db->getImages(ui->image_table, getFilterString(), ui->chbNotReady->isChecked());
}

void MainWindow::handleTrcFilter() {
	QString trc = combobox_trac_filter->currentText();
	if (!trc.isEmpty()) {
		filter_map["trc"] = "trc=" + trc;
	} else {
		filter_map.remove("trc");
	}
	db->getImages(ui->image_table, getFilterString(), ui->chbNotReady->isChecked());
}

void MainWindow::handleImgFilter() {
	QString img = lineedit_image_filter->text();
	if (!img.isEmpty()) {
		if (img.startsWith("hd",Qt::CaseInsensitive))
			filter_map["img"] = "img LIKE 'HD" + img.remove(0,2) + "'";
		else
			filter_map["img"] = "img LIKE 'HD" + img + "'";
	} else {
		filter_map.remove("img");
	}

	db->getImages(ui->image_table, getFilterString(), ui->chbNotReady->isChecked());
}

QString MainWindow::getFilterString() {
	QStringList filter_list =  filter_map.values();
	return filter_list.join(" AND ");
}

void MainWindow::handleMissingCheckBox() {
	db->getImages(ui->image_table, getFilterString(), ui->chbNotReady->isChecked());
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
	object_query_model->query().exec();
}

void MainWindow::handleAdminPass() {
	bool ok;
	QString password;
	password = QInputDialog::getText(this, tr("Admin Zugang freischalten"), tr("Passwort:"), QLineEdit::Password,QString(),&ok);
	/*
	 * MD5 sum of password
	 * 72ead00ca661b02c2dd8e677433ad390
	 */

	QCryptographicHash password_hash(QCryptographicHash::Md5);
	password_hash.addData(password.toStdString().c_str());
	if (ok ) {
		if(QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex())
				== QString("72ead00ca661b02c2dd8e677433ad390")) {
			QMessageBox message;
			message.setText(tr("Adminzugang erfolgreich freigeschaltet."));
			message.setStandardButtons(QMessageBox::Ok);
			message.exec();
			config->setAdmin(true);
		} else {
			QMessageBox message;
			message.setText("Admin Freischaltung: Falsches Passwort.");
			message.setStandardButtons(QMessageBox::Ok);
			message.exec();
		}
	}
}

void MainWindow::handleServerSelection() {
	ServerSelection server_selection(config);
	server_selection.exec();
	db->OpenDatabase();
	mapCanvas->setEnabled(false);
	ovrCanvas->setEnabled(false);
	ui->image_table->clearContents();
	object_query_model->clear();
	initSessionFrame();
}
