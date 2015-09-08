/*
 * ServerSelection.h
 *
 *  Created on: Sep 7, 2015
 *      Author: awg
 */

#ifndef SRC_SERVERSELECTION_H_
#define SRC_SERVERSELECTION_H_

#include <QDialog>
#include "ConfigHandler.h"
#include "ui_server_selection.h"

class ServerSelection : public QDialog {
public:
	ServerSelection(ConfigHandler * config);
	virtual ~ServerSelection();
private:
	Ui::ServerSelection * ui=0;
	ConfigHandler * config = 0;
	void refreshServerList();
public slots:
	void accept();
	void reject();
private slots:
	void handleAddServer();
	void handleRemoveServer();
};

#endif /* SRC_SERVERSELECTION_H_ */
