#ifndef SESSIONSELECTOR_H
#define SESSIONSELECTOR_H

#include <QDialog>
#include <QDir>
#include <QFileInfo>
#include <QAbstractButton>
#include <QDebug>
#include "defs.h"

namespace Ui {
class SessionSelector;
}

class SessionSelector : public QDialog
{
    Q_OBJECT
    
public:
    QFileInfo getSession();
    explicit SessionSelector(QWidget *parent = 0, QString sprjDir = PRJ_DIR_BIRD_CENSUS);
    ~SessionSelector();
    
private slots:

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::SessionSelector *ui;
	QFileInfo Session;
	QFileInfoList prjfiles;
};

#endif // SESSIONSELECTOR_H
