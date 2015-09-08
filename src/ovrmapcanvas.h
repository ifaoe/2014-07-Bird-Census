#ifndef OVRMAPCANVAS_H
#define OVRMAPCANVAS_H

#include <QMessageBox>
#include <QPushButton>
#include <qgsmapcanvas.h>
#include <qgsmaplayer.h>
#include <qgsmaplayerregistry.h>
#include <qgsgeometry.h>
#include <qgslabel.h>
#include <qgslabelattributes.h>
#include <qgsvectorlayer.h>
#include <qgsvectordataprovider.h>
#include <qgsmaptool.h>
#include <qgsmaptoolemitpoint.h>
#include "db.h"
#include "ui_mainwindow.h"

class CnsMapCanvas;

class OvrMapCanvas : public QgsMapCanvas
{
        Q_OBJECT
    public:
        explicit OvrMapCanvas(QWidget *parent,
                              Ui::MainWindow* aUI,
                              ConfigHandler* aConfig,
                              Db* aDB, CnsMapCanvas *aImgCanvas,
                              QgsMapLayerRegistry* lyrRegistry);

        void refreshLayerPaintList();
        bool openImageTiles(QString strCam, QString strFile);
        bool openImageEnvelope(QString strCam, QString strFile, QgsRectangle imgExt);
        bool saveRawTile(bool insert);
        bool readRawTile();

    signals:
        
    public slots:
        void doCanvasClicked(const QgsPoint &point, Qt::MouseButton button);
        void doSelectFirstTile();
        void doSelectNextTile();
        void doSelectPrevTile();
        void doSelectTile(int num);

    private:
        Ui::MainWindow* ui = 0;
        ConfigHandler* config;
        Db* db = 0;
        CnsMapCanvas* imgCanvas;

        int rawImgTileID = -1;
        QString rawImgTileTmWhen = "";
        QString rawImgTileTmSeen = "";
        QString rawImgTileW  = "";
        QString rawImgTileH  = "";
        QString rawImgTileUX = "";
        QString rawImgTileUY = "";

        bool isCurTile  = false;
        double  curTileW  = 0;
        double  curTileH  = 0;
        double  curTileUX  = 0;
        double  curTileUY  = 0;
        QDateTime rawImgTileTm = QDateTime::currentDateTimeUtc();


        QgsMapLayerRegistry* qgsLyrRegistry = 0;
        QgsVectorLayer* qgs_image_envelope_ = 0 ;
        QgsVectorLayer* qgs_image_tiles_ = 0 ;

        QgsMapToolEmitPoint *qgsToolPoint = 0;
        int selTileId = -1;
        int curTile = -1;
        QList<int> tileFeatureIds;

};

#endif // OVRMAPCANVAS_H
