/*
 * ServerSettings.h
 *
 *  Created on: Sep 7, 2015
 *      Author: awg
 */

#ifndef SRC_SERVERSETTINGS_H_
#define SRC_SERVERSETTINGS_H_

#include <QDialog>
#include "ui_server_settings.h"
#include "ConfigHandler.h"

class ServerSettings:public QDialog {
public:
	ServerSettings(ConfigHandler * config);
	virtual ~ServerSettings();
private:
	Ui::ServerSettings * ui = 0;
	ConfigHandler * config = 0;
private slots:
	void handleAccept();
	void handleReject();
};

#endif /* SRC_SERVERSETTINGS_H_ */
