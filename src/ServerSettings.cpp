/*
 * ServerSettings.cpp
 *
 *  Created on: Sep 7, 2015
 *      Author: awg
 */

#include "ServerSettings.h"
#include "QMessageBox"

ServerSettings::ServerSettings(ConfigHandler * config) : ui(new Ui::ServerSettings), config(config) {
	// TODO Auto-generated constructor stub
	ui->setupUi(this);
}

ServerSettings::~ServerSettings() {
	// TODO Auto-generated destructor stub
}

void ServerSettings::handleAccept() {
	if (ui->database_password->text() != ui->database_password_check->text()) {
		QMessageBox message;
		message.setText(QString::fromUtf8("Passwörter stimmen nicht überein!"));
		return;
	}
	config->AddDatabase(ui->host_name->text(), ui->host_ip->text(), ui->host_port->text().toInt(), ui->database_name->text(),
			ui->datanase_user->text(), ui->database_password->text());
}

void ServerSettings::handleReject() {
	close();
}
