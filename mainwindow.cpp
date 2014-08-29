#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "db.h"

MainWindow::MainWindow(const AppConfig *aConfig)
    : QMainWindow(0), config(aConfig), ui(new Ui::MainWindow)
{

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

    // Einlesen der Datenbankparameter
    db = new Db (config, (char*)"database",
                 (char*)"Datenbankkonfiguration");

    db->initConfig(logger);
    guiInitAdditionals();

    edtKey[0] = KEY_BIRD_SWIM; edtKey[1] = KEY_BIRD_FLY; edtKey[2] = KEY_MAMMAL;
    edtKey[3] = KEY_UFO; edtKey[4] = KEY_SUN; edtKey[5] = KEY_WAVE;

    rbEdit[0] = ui->rbVS; rbEdit[1] = ui->rbVF; rbEdit[2] = ui->rbMM;
    rbEdit[3] = ui->rbUFO; rbEdit[4] = ui->rbSN; rbEdit[5] = ui->rbWV;

    edtViews[0] = ui->tbvEdtVS; edtViews[1] = ui->tbvEdtVF;
    edtViews[2] = ui->tbvEdtMM; edtViews[3] = ui->tbvEdtUFO;
    edtViews[4] = ui->tbvEdtSN; edtViews[5] = ui->tbvEdtWV;

    for (int i = 0; i < 6; ++i) {
        edtViews[i]->setModel(new QStringListModel);
    }

    sqlImgModel = db->getImages(ui->tbvImages, sqlImgModel);
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

    connect( ui->rbVS, SIGNAL(toggled(bool)), this, SLOT(rbToggledVS(bool)));
    connect( ui->rbVF, SIGNAL(toggled(bool)), this, SLOT(rbToggledVF(bool)));
    connect( ui->rbMM, SIGNAL(toggled(bool)), this, SLOT(rbToggledMM(bool)));
    connect( ui->rbUFO, SIGNAL(toggled(bool)), this, SLOT(rbToggledUFO(bool)));
    connect( ui->rbSN, SIGNAL(toggled(bool)), this, SLOT(rbToggledSN(bool)));
    connect( ui->rbWV, SIGNAL(toggled(bool)), this, SLOT(rbToggledWV(bool)));

    connect( ui ->tbxLayers,SIGNAL(currentChanged(int)),
             this, SLOT(changeEdit(int)));



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
    switch (index) {
    case 0:
        ui->rbVS->setChecked(true);
        break;
    case 1:
        ui->rbVF->setChecked(true);
        break;
    case 2:
        ui->rbMM->setChecked(true);
        break;
    case 3:
        ui->rbUFO->setChecked(true);
        break;
    case 4:
        ui->rbSN->setChecked(true);
        break;
    case 5:
        ui->rbWV->setChecked(true);
        break;
    default:
        break;
    }
}
// ----------------------------------------------------------------------
void MainWindow::deleteSelection() {
    if (edtCurKey.compare(TK_QSTR_NONE) != 0) {
       QgsVectorLayer* lyr = mapCanvas->layerByKey(edtCurKey);
       QListView* lst = mapCanvas->keyListView(edtCurKey);
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
           mapCanvas->refresh();
       }
   }

}

// ----------------------------------------------------------------------
void MainWindow::clearSelection() {
    if (edtCurKey.compare(TK_QSTR_NONE) != 0) {
       QgsVectorLayer* lyr = mapCanvas->layerByKey(edtCurKey);
       if (lyr) { lyr->removeSelection();}
       QListView* lst = mapCanvas->keyListView(edtCurKey);
       if (lst) lst->selectionModel()->reset();
   }
    edtCurItems.clear();
    edtCurPkStr = TK_QSTR_NONE; edtCurPk = -1;
}

// ----------------------------------------------------------------------
void MainWindow::rbToggledVS(bool checked) {
    if (checked) {
        clearSelection();
        edtCurKey = edtKey[0];
        ui->tbxLayers->setCurrentIndex(0);
        mapCanvas->refresh();
     }
}
// ----------------------------------------------------------------------
void MainWindow::rbToggledVF(bool checked) {
    if (checked) {
        clearSelection();
        edtCurKey = edtKey[1];
        ui->tbxLayers->setCurrentIndex(1);
        mapCanvas->refresh();
     }
}
// ----------------------------------------------------------------------
void MainWindow::rbToggledMM(bool checked) {
    if (checked) {
       clearSelection();
       edtCurKey = edtKey[2];
        ui->tbxLayers->setCurrentIndex(2);
        mapCanvas->refresh();
     }
}
// ----------------------------------------------------------------------
void MainWindow::rbToggledUFO(bool checked) {
    if (checked) {
        clearSelection();
        edtCurKey = edtKey[3];
        ui->tbxLayers->setCurrentIndex(3);
        mapCanvas->refresh();
     }
}
// ----------------------------------------------------------------------
void MainWindow::rbToggledSN(bool checked) {
    if (checked) {
        clearSelection();
        edtCurKey = edtKey[4];
        ui->tbxLayers->setCurrentIndex(4);
        mapCanvas->refresh();
     }
}
// ----------------------------------------------------------------------
void MainWindow::rbToggledWV(bool checked) {
    if (checked) {
        clearSelection();
        edtCurKey = edtKey[5];
        ui->tbxLayers->setCurrentIndex(5);
        mapCanvas->refresh();
     }
}


// ----------------------------------------------------------------------
void MainWindow::edtUpdateSelection(QListView* lst, QItemSelection selected,
    QItemSelection deselected) {
    QModelIndexList selItems = selected.indexes();
    QModelIndexList deselItems = selected.indexes();
    QString line = "";
    if (deselItems.size()<1 ) return;
    QModelIndex deselIndex = deselItems.at(0);
    line = QString(lst->model()->data(deselIndex).toString());
    edtCurItems.clear();
    edtCurItems = line.split(' ');
    QgsVectorLayer * lyr = mapCanvas->layerByKey(edtCurItems.at(0));
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
       QgsVectorLayer * lyr = mapCanvas->layerByKey(edtCurKey);
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
     QString selCam  = QString::number(selIndex.column()-2);
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

