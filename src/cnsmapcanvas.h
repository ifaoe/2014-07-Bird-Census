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
#include "db.h"
#include "ui_mainwindow.h"
#include "QgsMapMarker.h"

class OvrMapCanvas;

const int MAP_MODE_INSPECT  =  0;
const int MAP_MODE_SELECT   = 10;
const int MAP_MODE_DIGITIZE = 20;

class CnsMapCanvas : public QgsMapCanvas
{
    Q_OBJECT
public:
    explicit CnsMapCanvas(QWidget* parent,
                          Ui::MainWindow* aUI,
                    ConfigHandler* aConfig,
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
    int map_mode() { return map_mode_; };
    double getScaleFactor();
    bool DeselectObjects();
    QgsVectorLayer * qgis_edit_layer() {return qgis_edit_layer_;}
    QgsVectorLayer * qgis_poly_layer() {return qgis_poly_layer_;}
    QgsRasterLayer * qgis_image_layer() {return qgis_image_layer_;}
    void UpdateObjectMarkers();
    bool SelectObjectByLocation(const QgsPoint & point);
    bool SelectObjectById(int rcns_id);
    void HideMarkers(bool hide);
    int current_object_selection() {return current_object_selection_;}
signals:
    
public slots:
    void doZoomExtent();
    void doZoom1by1();
    bool doOpenRasterLayer(QString cam, QString file);
    void doModePan();
    void doModeDigitize();
    void doModeSelect();
    void doCanvasClicked(const QgsPoint &point, Qt::MouseButton button);
    bool doHandleCoords(const  QgsPoint &point );
    void doHandleKeyPressed(QKeyEvent* keyEvent);
    void doHandleKeyReleased(QKeyEvent* keyEvent);
    void doUpdateStatus();


private:
    Ui::MainWindow* ui = 0;
    ConfigHandler* config;
    Db* db;
    QgsMapLayerRegistry* qgsLyrRegistry = 0;
    OvrMapCanvas* ovrCanvas;

    QString curCam = "";
    QString curImg = "";

    int rawImgID = -1;
    QString rawImgTmWhen = "";
    QString rawImgTmSeen = "";
    QDateTime rawImgTm = QDateTime::currentDateTimeUtc();

    /** Kartencursor UTM -X */
    double dblMapCursorUtmX = 0.0;

    /** Kartencursor UTM - Y */
    double dblMapCursorUtmY = 0.0;

    /** Kartencursor Latitude */
    double dblMapCursorLatitude = 0;

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

    int map_mode_ = MAP_MODE_INSPECT;

    double scaleFactor = 1.0;

    /** Tastaturhandler */
    bool keyCtrl  = false;
    bool keyAlt   = false;
    bool keyShift = false;

    int   cvImageBirdStatus = 0;
    QgsVectorLayer* qgis_edit_layer_ = 0;
    QgsVectorLayer* qgis_poly_layer_ = 0;
    QgsRasterLayer* qgis_image_layer_ = 0 ;
    QgsRasterDataProvider* qgsImgProvider = 0;
    QgsMapLayerRegistry * qgsLayerRegistry;

    QMap<int,QgsMapMarker *> object_markers_;
    QMap<int,QgsPoint*> object_locations_;
    int current_object_selection_ = -1;

    QgsCoordinateReferenceSystem crs4326;
    QgsCoordinateReferenceSystem crsUTM;
    QgsCoordinateTransform qgsTrfm2LonLat;



    QgsMapToolPan       *qgsToolPan  = 0;
    QgsMapToolEmitPoint *qgsToolPoint = 0;
    QgsMapToolIdentify  *qgsToolIdentify = 0;

    QMap<QString, int> type_marker_icon_map_;
    QMap<QString, QColor> type_marker_color_map_;

    bool openRasterLayer(const QString imagePath, const QString strCam, const QString strFile);

    bool openPolyLayer(QString strCam, QString strFile);

    bool openEditLayer(const QString strCam, const QString strFile);

    bool saveData(QString cam, QString file);

    bool refreshLayerSet();

    void UpdateObjectSelection();
};

#endif // CNSMAPCANVAS_H
