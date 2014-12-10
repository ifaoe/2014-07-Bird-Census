#include "sessionselector.h"
#include "ui_sessionselector.h"

SessionSelector::SessionSelector(Db * aDb, AppConfig * aCfg) :
    db(aDb), cfg(aCfg) ,ui(new Ui::SessionSelector)
{
	ui->setupUi(this);
	ui->cmbSession->insertItems(0, db->getSessionList());
//	QDir prjdir(sprjDir);
//    prjfiles = prjdir.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries);
//	for (int i=0; i < prjfiles.size(); i++) {
//        ui->cmbSession->insertItem(i, prjfiles.at(i).fileName());
//	}
    //@TODO: Projektverzeichnis duchgehen und Projektkonfigurationen auslesen
}

SessionSelector::~SessionSelector()
{
    delete ui;
}

void SessionSelector::on_buttonBox_accepted()
{
	QString session = ui->cmbSession->currentText();
    project * prj = db->getSessionParameters(session);
    cfg->setPrjSession(prj->project_id);
    cfg->setPrjFlight(prj->flight_id);
    cfg->setPrjFilter(prj->filter);
    cfg->setPrjType(prj->session_type);
    cfg->setPrjPath(prj->path);
    cfg->setPrjUtmSector(prj->utmSector);
    cfg->readQueries();
    delete prj;
}

void SessionSelector::on_buttonBox_rejected()
{
    exit(0);
}
