#ifndef SESSIONSELECTOR_H
#define SESSIONSELECTOR_H

#include <QDialog>
#include <QDir>
#include <QFileInfo>
#include <QAbstractButton>
#include <QDebug>
#include "defs.h"
#include "db.h"
#include "appconfig.h"

namespace Ui {
class SessionSelector;
}

class SessionSelector : public QDialog
{
    Q_OBJECT
    
public:
    explicit SessionSelector(Db * aDb, AppConfig * aCfg);
    ~SessionSelector();
    
private slots:

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Db * db = 0;
    AppConfig * cfg = 0;
    Ui::SessionSelector *ui;
};

#endif // SESSIONSELECTOR_H
