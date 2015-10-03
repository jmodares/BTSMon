/*
 * ConfigDlg.h
 *
 *  Created on: Dec 16, 2009
 *      Author: jalil
 */

#ifndef CONFIGDLG_H_
#define CONFIGDLG_H_

#include <QtGui/QDialog>

#include "ui_Config.h"

class CConfigDlg: public QDialog, private Ui_config {
	Q_OBJECT

public:
	CConfigDlg(QWidget *parent = 0);

private:
	void setConfig();
	void getConfig();

private slots:
	void done(int reslt);

	void addIP();
	void deleteIP();
	void browseFile();
};

#endif /* CONFIGDLG_H_ */
