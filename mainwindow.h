#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QItemSelectionModel>
#include <QMessageBox>
#include <QModelIndex>
#include <QMainWindow>
#include <QLabel>
#include <QStringList>
#include <qgsproviderregistry.h>
#include <qgsmaplayerregistry.h>
#include "appconfig.h"
#include "ovrmapcanvas.h"
#include "cnsmapcanvas.h"
#include "db.h"


const char MNWD_ERR_MISSING_PROVIDER[] = "Der Provider %s fehlt!\n%s";
const char MNDW_ERR_MISSING_HINT_PFX[] = "Vermutlich ist der QGIS Pfad in der "
      "Umgebungsvariable $qgis.prefixPath falsch gesetzt!";

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(AppConfig *aConfig, Db * aDb);
    ~MainWindow();
    
private:
    AppConfig *config;
    Ui::MainWindow *ui = 0;
    Db *db = 0;
    QgsProviderRegistry *qgsPvrRegistry = 0;
    QgsMapLayerRegistry *qgsLyrRegistry = 0;

    QItemSelectionModel *imgSelector= 0;
    QItemSelectionModel *objSelector= 0;

    QString selFile = "";
    QString selCam = "";
    QString curCam = "";
    QString curFile = "";

    QString edtCurKey  = TK_QSTR_NONE;
    QString edtCurView = TK_QSTR_NONE;
    QString edtKey[6];
    QMap<int, QString> edtKeys;

    bool keyMarkerHide = false;

    CnsMapCanvas *mapCanvas = 0;
    OvrMapCanvas *ovrCanvas = 0;
    void qgsCheckProviders();
    void guiInitAdditionals();
    bool checkButtonByKey(QString tp);
    void initSessionFrame();

signals:

public slots:
//    void checkRdyImages();
    void saveData();
    void clearSelection();
    void deleteSelection();
    void rbToggledType();
    void imgUpdateSelection(QItemSelection selected, QItemSelection deselected);
    void objUpdateSelection();
    void hideMarker(bool checked);
    void handleSessionSelection();
    void handleCamSelection();
    void handleTrcSelection();
private:
    QSqlQueryModel* sqlImgModel = 0;
};

#endif // MAINWINDOW_H
