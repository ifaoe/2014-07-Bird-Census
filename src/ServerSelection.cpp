/*
 * ServerSelection.cpp
 *
 *  Created on: Sep 7, 2015
 *      Author: awg
 */

#include "ServerSelection.h"
#include "ServerSettings.h"
#include <QMessageBox>

ServerSelection::ServerSelection(ConfigHandler * config) : ui(new Ui::ServerSelection), config(config) {
	ui->setupUi(this);

	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	refreshServerList();
}

ServerSelection::~ServerSelection() {
	delete ui;
}

void ServerSelection::refreshServerList() {
	ui->server_box->clear();
	ui->server_box->addItems(config->getDatabaseList());
}

void ServerSelection::accept() {
	if (ui->server_box->currentText().isEmpty()) {
		QMessageBox message;
		message.setText(QString::fromUtf8("Kein Server ausgewählt."));
		return;
	}
	config->setPreferredDatabase(ui->server_box->currentText());
	QDialog::accept();
}

void ServerSelection::reject() {
	QDialog::reject();
}

void ServerSelection::handleAddServer() {
	ServerSettings server_settings(config);
	refreshServerList();
}

void ServerSelection::handleRemoveServer() {
	QMessageBox message;
	message.setText(QString::fromUtf8("Server %1 wird gelöscht.").arg(ui->server_box->currentText()));
	message.setInformativeText(QString::fromUtf8("Sind sie sich sicher?"));
	message.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
	if (message.exec() == QMessageBox::Cancel)
		return;
	config->removeDatabase(ui->server_box->currentText());
	refreshServerList();
}
