#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QItemSelectionModel>
#include <QMessageBox>
#include <QModelIndex>
#include <QMainWindow>
#include <QLineEdit>
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

    QString selected_file_;
    QString selected_cam_;

    QString filter = "WHERE TRUE";
    QString cam_filter_ = "";
    QString trac_filter_ = "";
    QString image_filter_ = "";

    QString edtCurKey;
    QString edtCurView;

    QComboBox * combobox_cam_filter_;
    QComboBox * combobox_trac_filter_;
    QLineEdit * lineedit_image_filter_;

    bool keyMarkerHide = false;

    QSqlQueryModel* object_query_model_ = 0;

    CnsMapCanvas *mapCanvas = 0;
    OvrMapCanvas *ovrCanvas = 0;
    void qgsCheckProviders();
    void guiInitAdditionals();
    bool checkButtonByKey(QString tp);
    void initSessionFrame();
    void initFilters();
    QString getFilterString();
    QAbstractButton * GetButtonByKey(QButtonGroup * button_group, QString key, QString value);
    void RefreshObjectList();
signals:

public slots:
//    void checkRdyImages();
    void saveData();
    void clearSelection();
    void deleteSelection();
    void imgUpdateSelection();
    void objUpdateSelection();
    void hideMarker(bool checked);
    void handleSessionSelection();
    void handleCamFilter();
    void handleTrcFilter();
    void handleImgFilter();
    void handleMissingCheckBox();
};

#endif // MAINWINDOW_H
