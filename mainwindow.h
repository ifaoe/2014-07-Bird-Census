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
#include "textlogger.h"
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
    explicit MainWindow(const AppConfig *aConfig, Db * aDb);
    ~MainWindow();

    
private:
    const AppConfig *config;
    Ui::MainWindow *ui = 0;
    TextLogger *logger = 0;
    TextLogger *dbText = 0;
    Db *db = 0;
    QgsProviderRegistry *qgsPvrRegistry = 0;
    QgsMapLayerRegistry *qgsLyrRegistry = 0;

    QItemSelectionModel *imgSelector= 0;
    QItemSelectionModel *vsSelector= 0;
    QItemSelectionModel *vfSelector= 0;
    QItemSelectionModel *uoSelector= 0;
    QItemSelectionModel *snSelector= 0;
    QItemSelectionModel *wvSelector= 0;
    QItemSelectionModel *mmSelector= 0;

    QStringList edtCurItems;
    QString edtCurPkStr = TK_QSTR_NONE;
    QString edtCurKey  = TK_QSTR_NONE;
    int edtCurPk = -1;
    QString edtCurView = TK_QSTR_NONE;
//    QListView* edtViews[6];
    QString edtKey[6];
    QMap<int, QListView*> edtViews;
    QMap<int, QString> edtKeys;
    QMap<int, QRadioButton*> edtButtons;

    bool keyMarkerHide = false;

    CnsMapCanvas *mapCanvas = 0;
    OvrMapCanvas *ovrCanvas = 0;
    void qgsCheckProviders();
    void guiInitAdditionals();

signals:

public slots:
//    void checkRdyImages();
    void saveData();
    void clearSelection();
    void deleteSelection();
    void rbToggledType();
    void changeEdit(int index);
    void imgUpdateSelection(QItemSelection selected, QItemSelection deselected);
    void vsUpdateSelection(QItemSelection selected, QItemSelection deselected);
    void vfUpdateSelection(QItemSelection selected, QItemSelection deselected);
    void uoUpdateSelection(QItemSelection selected, QItemSelection deselected);
    void mmUpdateSelection(QItemSelection selected, QItemSelection deselected);
    void wvUpdateSelection(QItemSelection selected, QItemSelection deselected);
    void snUpdateSelection(QItemSelection selected, QItemSelection deselected);
    void hideMarker(bool checked);
private:
    QSqlQueryModel* sqlImgModel = 0;
    void edtUpdateSelection(QListView* lst,
                            QItemSelection selected, QItemSelection deselected);
    bool addEdtTbx(QString tbName, int tbIndex, QRadioButton * tbButton, QListView * tbListView);
};

#endif // MAINWINDOW_H
