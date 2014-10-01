#include "sessionselector.h"
#include "ui_sessionselector.h"

SessionSelector::SessionSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SessionSelector)
{
	ui->setupUi(this);
	QDir prjdir(PRJ_DIR_BIRD_CENSUS);
    prjfiles = prjdir.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries);
	for (int i=0; i < prjfiles.size(); i++) {
        ui->cmbSession->insertItem(i, prjfiles.at(i).fileName());
	}
    //@TODO: Projektverzeichnis duchgehen und Projektkonfigurationen auslesen
}

SessionSelector::~SessionSelector()
{
    delete ui;
}

void SessionSelector::on_buttonBox_accepted()
{
    Session = prjfiles.at(ui->cmbSession->currentIndex());
    // TODO Session zurückgeben
    // TODO Session in Applikationstitel einblenden
}

void SessionSelector::on_buttonBox_rejected()
{
    exit(0);
}

QFileInfo SessionSelector::getSession() {
	return this->Session;
}
