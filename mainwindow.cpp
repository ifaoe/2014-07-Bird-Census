#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "db.h"

MainWindow::MainWindow(const AppConfig *aConfig, Db * aDb)
    : QMainWindow(0), config(aConfig), ui(new Ui::MainWindow), db(aDb)
{
	qDebug() << "Main Window Construct";
    // Aufsetzen der GUI Grundumgebung + Logger
    ui->setupUi(this);
    this->resize(QSize(config->guiWinWidth(),config->guiWinHeight()));
    this->move(config->guiWinLeft(),config->guiWinTop());
    this->setWindowTitle(config->appTitle()+" - "+config->appVersion());
    logger = new TextLogger(this, ui->txtLogger, statusBar());

    // Der QGIS Umgebung
    qgsPvrRegistry = QgsProviderRegistry::instance();
    qgsLyrRegistry = QgsMapLayerRegistry::instance();
    qgsCheckProviders();

    db->initLogger(logger);
    guiInitAdditionals();

    addEdtTbx(KEY_BIRD_SWIM, 0, ui->rbVS, ui->tbvEdtVS);
    addEdtTbx(KEY_BIRD_FLY, 1, ui->rbVF, ui->tbvEdtVF);
    addEdtTbx(KEY_MAMMAL, 2, ui->rbMM, ui->tbvEdtMM);
    addEdtTbx(KEY_UFO, 3, ui->rbUFO, ui->tbvEdtUFO);
    addEdtTbx(KEY_SUN, 4, ui->rbSN, ui->tbvEdtSN);
    addEdtTbx(KEY_WAVE, 5, ui->rbWV, ui->tbvEdtWV);

    db->getImages(ui->tbvImages, config->prjType());
    imgSelector = ui->tbvImages->selectionModel();

    vsSelector  = ui->tbvEdtVS->selectionModel();
    vfSelector  = ui->tbvEdtVF->selectionModel();
    mmSelector  = ui->tbvEdtMM->selectionModel();
    uoSelector  = ui->tbvEdtUFO->selectionModel();
    snSelector  = ui->tbvEdtSN->selectionModel();
    wvSelector  = ui->tbvEdtWV->selectionModel();

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

    connect( vsSelector,
             SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             this,
             SLOT(vsUpdateSelection(QItemSelection,QItemSelection)));

    connect( vfSelector,
             SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             this,
             SLOT(vfUpdateSelection(QItemSelection,QItemSelection)));

    connect( mmSelector,
             SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             this,
             SLOT(mmUpdateSelection(QItemSelection,QItemSelection)));

    connect( uoSelector,
             SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             this,
             SLOT(uoUpdateSelection(QItemSelection,QItemSelection)));

    connect( wvSelector,
             SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             this,
             SLOT(wvUpdateSelection(QItemSelection,QItemSelection)));

    connect( snSelector,
             SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             this,
             SLOT(snUpdateSelection(QItemSelection,QItemSelection)));

    connect( ui->chbHideMarker, SIGNAL(clicked(bool)), this, SLOT(hideMarker(bool)));
    connect( ui->btgLayers, SIGNAL(buttonClicked(int)), this, SLOT(rbToggledType()));

    connect( ui ->tbxLayers,SIGNAL(currentChanged(int)),
             this, SLOT(changeEdit(int)));



}

bool MainWindow::addEdtTbx(QString tbName, int tbIndex, QRadioButton * tbButton, QListView * tbListView) {
	edtKeys[tbIndex] = tbName;
	edtViews[tbIndex] = tbListView;
	edtButtons[tbIndex] = tbButton;
    edtViews[tbIndex]->setModel(new QStringListModel);
    ui->btgLayers->setId(tbButton, tbIndex);
    mapCanvas->addEdtLyr(tbName, tbIndex, tbListView);
	return true;
}

// ------------------------------------------------------------------------
void MainWindow::saveData() {
   mapCanvas->doSaveData(ui->lblCurCam->text(), ui->lblCurImage->text());
   ovrCanvas->saveRawTile(false);
}

// ------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete logger;
    delete ui;
    delete db;
}

// ------------------------------------------------------------------------
void MainWindow::qgsCheckProviders() {
    logger->log("QGIS PREFIX PATH IS "+config->qgsPrefixPath());
    QString msg;
    logger->log("OPEN PROVIDER LIST");
    QStringList providers = qgsPvrRegistry->providerList();
    logger->log(QString("PROVIDERS %1 FOUND").arg(providers.size()));
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
        logger->log(msg.sprintf("PROVIDER %0d: %s", (i+1),
                                providers.at(i).toUtf8().data()));
    }
    if (!doneGdal) {
        qFatal(MNWD_ERR_MISSING_PROVIDER, "GDAL",
               MNDW_ERR_MISSING_HINT_PFX);
    } else { logger->log("TEST PROVIDER GDAL OK");  }

    if (!doneOgr) {
        qFatal(MNWD_ERR_MISSING_PROVIDER, "OGR",
               MNDW_ERR_MISSING_HINT_PFX);
    } else {  logger->log("TEST PROVIDER OGR OK"); }

    if (!donePGIS) {
        qFatal(MNWD_ERR_MISSING_PROVIDER, "Postgresql/PostGIS",
               MNDW_ERR_MISSING_HINT_PFX);
     } else {  logger->log("TEST PROVIDER POSTGRESQL/POSTGIS OK"); }

}


// ----------------------------------------------------------------------
void MainWindow::guiInitAdditionals() {
    QGraphicsScene *scnImage = new QGraphicsScene(this);
    QGraphicsView  *vwpImage = ui->grxView;
    vwpImage->setScene(scnImage);

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
void MainWindow::changeEdit(int index) {
	edtButtons[index]->setChecked(true);
}
// ----------------------------------------------------------------------
void MainWindow::deleteSelection() {
    if (edtCurKey.compare(TK_QSTR_NONE) != 0) {
       QgsVectorLayer* lyr = mapCanvas->rbCheckedVectorLayer();
       QListView* lst = mapCanvas->rbCheckedListView();
       if (lyr && lst) {
           lyr->startEditing();
           QgsFeatureList flist= lyr->selectedFeatures();
           QStringListModel* model = qobject_cast<QStringListModel*>(lst->model());
           QStringList slist = model->stringList();
           foreach(QgsFeature fet, flist) {
               lyr->selectedFeatures();
               int ix = fet.id();
               int pk = fet.attribute("SID").toInt();
               QString pat; pat.sprintf(" IX%d",ix);
               int i=0;
               while ( i < slist.size() ) {
                  QString s = slist.at(i);
                  if (s.endsWith(pat)) slist.removeAt(i); else i++;
               }
               if (pk!=-1) {
                   db->deleteRawCensus(pk);
               }
           }
           lyr->dataProvider()->deleteFeatures(lyr->selectedFeaturesIds());
           lyr->commitChanges();
           model->setStringList(slist);
//           mapCanvas->refresh();
       }
   }

}

// ----------------------------------------------------------------------
void MainWindow::clearSelection() {
    if (edtCurKey.compare(TK_QSTR_NONE) != 0) {
       QgsVectorLayer* lyr = mapCanvas->rbCheckedVectorLayer();
       if (lyr) { lyr->removeSelection();}
       QListView* lst = mapCanvas->rbCheckedListView();
       if (lst) lst->selectionModel()->reset();
   }
    edtCurItems.clear();
    edtCurPkStr = TK_QSTR_NONE; edtCurPk = -1;
}

void MainWindow::rbToggledType() {
	clearSelection();
	edtCurKey = ui->btgLayers->checkedButton()->property("dbvalue").toString();
	ui->tbxLayers->setCurrentIndex(ui->btgLayers->checkedId());
}

void MainWindow::hideMarker(bool checked) {
	if (checked) {
		QList<QString> mlayers = edtKeys.values();
		for (int i=0; i<mlayers.size(); i++) {
			static_cast<QgsVectorLayer*>(mapCanvas->layerByKey(mlayers[i]))->setLayerTransparency(100);
		}
		mapCanvas->refresh();
	} else {
		QList<QString> mlayers = edtKeys.values();
		for (int i=0; i<mlayers.size(); i++) {
			static_cast<QgsVectorLayer*>(mapCanvas->layerByKey(mlayers[i]))->setLayerTransparency(0);
		}
		mapCanvas->refresh();
	}
}

// ----------------------------------------------------------------------
void MainWindow::edtUpdateSelection(QListView* lst, QItemSelection selected,
    QItemSelection deselected) {
	Q_UNUSED(deselected);
    QModelIndexList selItems = selected.indexes();
    QModelIndexList deselItems = selected.indexes();
    QString line = "";
    if (deselItems.size()<1 ) return;
    QModelIndex deselIndex = deselItems.at(0);
    line = QString(lst->model()->data(deselIndex).toString());
    edtCurItems.clear();
    edtCurItems = line.split(' ');
    QgsVectorLayer * lyr = mapCanvas->rbCheckedVectorLayer();
    if (lyr) lyr->removeSelection();
    ui->btnMapRmObj->setEnabled(false);

    if (selItems.size()<1 ) return;
    QModelIndex selIndex = selItems.at(0);
    line = QString(lst->model()->data(selIndex).toString());
    lst->scrollTo(selIndex,QAbstractItemView::EnsureVisible);
    edtCurItems.clear();
    edtCurItems = line.split(' ');
    edtCurPkStr = edtCurItems.at(7);
    try {
        edtCurPk = edtCurPkStr.toInt();
    } catch (...) {
        edtCurPk = -1;
    }
    for (int i = 0; i < 6; ++i) {
        QListView* cur =edtViews[i];
        if (lst!=cur) {
            cur->selectionModel()->reset();
        } else {
            edtCurKey = edtKey[i];
        }
    }
    try {
       double mx = edtCurItems.at(3).toDouble();
       double my = edtCurItems.at(4).toDouble();
       QString six = edtCurItems.at(8);
       int ix = six.replace("IX","").toInt();
       mapCanvas->doCenter1by1(mx,my);
       QgsVectorLayer * lyr = mapCanvas->rbCheckedVectorLayer();
       if (lyr && mapCanvas->getMapMode() == MAP_MODE_SELECT) {
             lyr->select(ix);
             ui->btnMapRmObj->setEnabled(true);
       }
    } catch (...) {
    }
    if (edtCurPk!=-1) {

    }
}

// ----------------------------------------------------------------------
void MainWindow::vsUpdateSelection(QItemSelection selected,
    QItemSelection deselected) {
    edtUpdateSelection(ui->tbvEdtVS, selected, deselected);
}

// ----------------------------------------------------------------------
void MainWindow::vfUpdateSelection(QItemSelection selected,
    QItemSelection deselected) {
    edtUpdateSelection(ui->tbvEdtVF, selected, deselected);
}

// ----------------------------------------------------------------------
void MainWindow::uoUpdateSelection(QItemSelection selected,
    QItemSelection deselected) {
    edtUpdateSelection(ui->tbvEdtUFO, selected, deselected);
}
// ----------------------------------------------------------------------
void MainWindow::mmUpdateSelection(QItemSelection selected,
    QItemSelection deselected) {
    edtUpdateSelection(ui->tbvEdtMM, selected, deselected);
}
// ----------------------------------------------------------------------
void MainWindow::wvUpdateSelection(QItemSelection selected,
    QItemSelection deselected) {
    edtUpdateSelection(ui->tbvEdtWV, selected, deselected);
}
// ----------------------------------------------------------------------
void MainWindow::snUpdateSelection(QItemSelection selected,
    QItemSelection deselected) {
    edtUpdateSelection(ui->tbvEdtSN, selected, deselected);
}


// ----------------------------------------------------------------------
void MainWindow::imgUpdateSelection(QItemSelection selected,
     QItemSelection deselected)
 {
     QModelIndexList deselItems = deselected.indexes();
     QModelIndexList selItems = selected.indexes();
     if (selItems.size()<1 ) return;
     QModelIndex selIndex = selItems.at(0);
     QString selFile = QString(ui->tbvImages->model()->data(selIndex).toString());
     QString selCam  = QString::number(selIndex.column()-1);
     QString curFile = ui->lblCurImage->text();
     QString curCam  = ui->lblCurCam->text();
     bool isDefault  = (ui->lblCurImage->text().compare(TK_QSTR_NONE) == 0);
     bool changed = (selCam.compare(curCam) !=0 || selFile.compare(curFile) !=0);
     if ( ! changed) return;
     if ( ! isDefault && ASK_MODIFY) {
          changed = ( QMessageBox::question(this,
                      "Luftbild aendern",
                      "Bildauswahl "+curFile+" Kamera "+curCam+" aendern?\n"+
                      "Bem: Metadaten werden automatisch gesichert!",
                       QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes);
     }
     if ( changed ) {
              ui->lblCurImage->setText(selFile);
              ui->lblCurCam->setText(selCam);
              ui->chbHideMarker->setChecked(false);
//              hideMarker(false);
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
              this->setWindowTitle(config->appTitle()+" - "+config->appVersion()+" - Kamera "+ selCam +" - "+selFile);
              ovrCanvas->doSelectFirstTile();
              ui->lblCurCam->setText(selCam);
              ui->lblCurImage->setText(selFile);
              ui->tbxTasks->setCurrentIndex(1);
              mapCanvas->setFocus();
              mapCanvas->doSetupEditModus();
     } else {
         imgSelector->setCurrentIndex(selIndex,
                                      QItemSelectionModel::Deselect);
     }
 }
// ----------------------------------------------------------------------

