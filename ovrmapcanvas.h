#ifndef OVRMAPCANVAS_H
#define OVRMAPCANVAS_H

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
#include <opencv2/core/core.hpp>
#include "textlogger.h"
#include "appconfig.h"
#include "db.h"
#include "ui_mainwindow.h"

class CnsMapCanvas;

class OvrMapCanvas : public QgsMapCanvas
{
        Q_OBJECT
    public:
        explicit OvrMapCanvas(QWidget *parent,
                              Ui::MainWindow* aUI,
                              const AppConfig* aConfig,
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
        const AppConfig* config;
        Db* db;
        CnsMapCanvas* imgCanvas;
        TextLogger* out = 0;

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
        QgsVectorLayer* qgsImgEnvelope = 0 ;
        QgsVectorLayer* qgsImgTiles = 0 ;
        QgsVectorLayer* qgsCurTile = 0 ;

        QgsMapToolEmitPoint *qgsToolPoint = 0;
        int selTileId = -1;
        int curTile = -1;
        QList<int> tileFeatureIds;

};

#endif // OVRMAPCANVAS_H
