#ifndef CNSMAPCANVAS_H
#define CNSMAPCANVAS_H

#include <QDebug>
#include <QObject>
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <QLabel>
#include <QToolBar>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStringListModel>
#include <qgis.h>
#include <qgsmaptoolzoom.h>
#include <qgsmaptoolpan.h>
#include <qgsmaptoolidentify.h>
#include <qgsmaptoolemitpoint.h>
#include <qgsmapcanvas.h>
#include <qgsmaplayer.h>
#include <qgsmaplayerregistry.h>
#include <qgsmultibandcolorrenderer.h>
#include <qgspoint.h>
#include <qgsgeometry.h>
#include <qgsfield.h>
#include <qgsfeature.h>
#include <qgsfeatureiterator.h>
#include <qgsrectangle.h>
#include <qgsrasterblock.h>
#include <qgsrasterdataprovider.h>
#include <qgsrasterlayer.h>
#include <qgsfillsymbollayerv2.h>
#include <qgssymbolv2.h>
#include <qgssinglesymbolrendererv2.h>
#include <qgsmarkersymbollayerv2.h>
#include <qgsvectorlayer.h>
#include <qgsvectordataprovider.h>
#include <opencv2/core/core.hpp>
#include "textlogger.h"
#include "appconfig.h"
#include "db.h"
#include "ui_mainwindow.h"
#include "layerstack.h"

class OvrMapCanvas;


#ifdef OPENCV
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
typedef cv::Vec<quint16, 3> Vec3u16;
typedef cv::Mat_<Vec3u16>  Mat3u16;
#endif /* OPENCV */

const int MAP_MODE_INSPECT  =  0;
const int MAP_MODE_SELECT   = 10;
const int MAP_MODE_DIGITIZE = 20;

class CnsMapCanvas : public QgsMapCanvas
{
    Q_OBJECT
public:
    explicit CnsMapCanvas(QWidget* parent,
                          Ui::MainWindow* aUI,
                    const AppConfig* aConfig,
                    Db* aDB,
                    QgsMapLayerRegistry* lyrRegistry
             );
    bool doCalcPixPos(const QgsPoint& point , int& pixX, int& pixY);
    bool doCalcLonLat(const QgsPoint& point , double& lon, double& lat);
    bool doCalcWorldPos(const int pixX ,const int pixY,
                        double& wldX, double& wldY);
    void doCenter1by1(double x, double y);
    void doCenter1by1(QgsPoint point);
    void doSetupEditModus();
    bool doSaveData(QString cam, QString file);
    void setOvrCanvas(OvrMapCanvas* ovrCvs);
    QgsVectorLayer* rbCheckedVectorLayer();
    QListView* rbCheckedListView();
    int getMapMode();
    double getScaleFactor();
    bool addEdtLyr(QString tbName, int tbIndex, QListView *tbListView);
signals:
    
public slots:
    void doZoomExtent();
    // void doZoomIn();
    // void doZoomOut();
    void doZoom1by1();
    bool doOpenRasterLayer(QString cam, QString file);
    void doModePan();
    void doModeDigitize();
    void doModeSelect();
    bool doSaveData();
    void doCanvasClicked(const QgsPoint &point, Qt::MouseButton button);
    bool doHandleCoords(const  QgsPoint &point );
    void doHandleKeyPressed(QKeyEvent* keyEvent);
    void doHandleKeyReleased(QKeyEvent* keyEvent);
    void doUpdateStatus();

    QgsMapLayer* layerByKey(QString key);

#ifdef OPENCV
    int mapImg2CV(const QgsPoint &point, double radius, int width, int height);
    QImage mat2QImage8Bit(const cv::Mat3b &src);
#endif /* OPENCV */


private:
    Ui::MainWindow* ui = 0;
    const AppConfig* config;
    Db* db;
    QgsMapLayerRegistry* qgsLyrRegistry = 0;
    OvrMapCanvas* ovrCanvas;

    LayerStack* mapLayerStack;
    QMap<int, QgsVectorLayer*> edtLayers;
    QMap<int, QListView*> edtViews;
    QMap<int, QString> edtKeys;

    int rawImgID = -1;
    QString rawImgTmWhen = "";
    QString rawImgTmSeen = "";
    QDateTime rawImgTm = QDateTime::currentDateTimeUtc();

    /** Kartencursor UTM -X */
    double dblMapCursorUtmX = 0.0;

    /** Kartencursor UTM - Y */
    double dblMapCursorUtmY = 0.0;

    /** Kartencursor Latitude */
    double dbMapCursorLatitude = 0;

    /** Kartencursor Longitude */
    double dblMapCursorLongitude = 0;

    /** Kartencursor in Bild Pixeln Spalte */
    int intMapCursorPixelCol = 0;

    /** Kartencursor in Bild Pixeln Zeile */
    int intMapCursorPixelRow = 0;

    /** Map Scale */
    double dblScale = 1;

    /** Einheiten je Pixel in X */
    double dblUnitPixX = 0;

    /** Einheiten je Pixel in Y */
    double dblUnitPixY = 0;

    /** UTM-X Minimum des Bildes */
    double dblExtMinUtmX = 0;

    /** UTM-X Maximum des Bildes */
    double dblExtMaxUtmX = 0;

    /** UTM-Y Minimum des Bildes */
    double dblExtMinUtmY = 0;

    /** UTM-Y Maximum des Bildes */
    double dblExtMaxUtmY = 0;

    /** UTM-X Minimum des aktuellen Kartenausschnitts */
    double dblCurMinUtmX = 0;

    /** UTM-X Maximum des aktuellen Kartenausschnitts */
    double dblCurMaxUtmX = 0;

    /** UTM-Y Minimum des aktuellen Kartenausschnitts */
    double dblCurMinUtmY = 0;

    /** UTM-Y Maximum des aktuellen Kartenausschnitts */
    double dblCurMaxUtmY= 0;

    /** UTM-X des aktuellen Kartenzentrums */
    double dblCurCenterUtmX = 0;

    /** UTM-Y des aktuellen Kartenzentrums */
    double dblCurCenterUtmY = 0;

    int mapMode = MAP_MODE_INSPECT;

    double scaleFactor = 1.0;

    /** Tastaturhandler */
    bool keyCtrl  = false;
    bool keyAlt   = false;
    bool keyShift = false;
    TextLogger* out = 0;

    /** Karten Objekte */
#ifdef OPENCV
    cv::Mat *cvImageBird = 0;
#endif /* OPENCV */
    int   cvImageBirdStatus = 0;
    QgsFields qgsEditFields;
    QgsVectorLayer* qgsVsLayer = 0 ;
    QgsVectorLayer* qgsVfLayer = 0 ;
    QgsVectorLayer* qgsMmLayer = 0 ;
    QgsVectorLayer* qgsSnLayer = 0 ;
    QgsVectorLayer* qgsUfoLayer = 0 ;
    QgsVectorLayer* qgsWvLayer = 0 ;
    QgsVectorLayer* qgsEdtLayer = 0;
    QgsVectorLayer* qgsPolyLayer = 0;
    QgsRasterLayer* qgsImgLayer = 0 ;
    QgsRasterDataProvider* qgsImgProvider = 0;

    //QList<QgsMapCanvasLayer> qgsLayerList;
    QgsCoordinateReferenceSystem crs4326;
    QgsCoordinateReferenceSystem crsUTM;
    QgsCoordinateTransform* qgsTrfm2LonLat = 0;


    /** Zeichenflaeche Kartefenster  */
    //    QgsMapToolZoom      *qgsToolZoomIn = 0;
    //    QgsMapToolZoom      *qgsToolZoomOut = 0;

    QgsMapToolPan       *qgsToolPan  = 0;
    QgsMapToolEmitPoint *qgsToolPoint = 0;
    QgsMapToolIdentify  *qgsToolIdentify = 0;

    /** Aktionen fuer die Kartenflaeche definieren  */
    // QAction *qtActZoomOut = 0;
    // QAction *qtActZoomIn  = 0;

    /** Aktionen */
    bool openRasterLayer(const QString imagePath,
                         const QString strCam,
                         const QString strFile);

    bool openPolyLayer(QString strCam, QString strFile);

    QgsVectorLayer* openEditLayer(const QString imagePath,
                       const QString strCam,
                       const QString strFile,
                       const QString lyrKey,
                       const int lyrId,
                       QgsVectorLayer *layer);

    bool saveData(QString cam, QString file);
};

#endif // CNSMAPCANVAS_H
