/*
 * ConfigDlg.cpp
 *
 *  Created on: Dec 16, 2009
 *      Author: jalil
 */

#include <QtGui/QtGui>

#include "Config.h"
#include "ConfigDlg.h"

CConfigDlg::CConfigDlg(QWidget *parent) :
	QDialog(parent) {

	setupUi(this);
	setConfig();

	lineEditUser->selectAll();
}

void CConfigDlg::setConfig() {
	lineEditUser->setText(g_config.m_user);
	lineEditPassword->setText(g_config.m_password);
	checkBoxAccount->setChecked(g_config.m_save);

	for (int i = 0; i < g_config.m_ips.count(); i++) {
		QListWidgetItem *ipItem = new QListWidgetItem(listWidgetBSC);
		ipItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
		ipItem->setText(g_config.m_ips[i]);
	}
	lineEditGIS->setText(g_config.m_file);

	dialInterval->setValue(g_config.m_interval);
	spinBoxLog->setValue(g_config.m_limit);
}

void CConfigDlg::getConfig() {
	g_config.m_user = lineEditUser->text();
	g_config.m_password = lineEditPassword->text();
	g_config.m_save = checkBoxAccount->isChecked();

	g_config.m_ips.clear();
	for (int i = 0; i < listWidgetBSC->count(); i++)
		if (!listWidgetBSC->item(i)->text().isEmpty())
			g_config.m_ips << listWidgetBSC->item(i)->text();

	g_config.m_ips.removeDuplicates();
	g_config.m_file = lineEditGIS->text();

	g_config.m_interval = dialInterval->value();
	g_config.m_limit = spinBoxLog->value();
}

void CConfigDlg::done(int reslt) {
	if (reslt) {
		if (lineEditUser->text().isEmpty()) {
			listWidget->setCurrentRow(0);
			lineEditUser->setFocus();
			QMessageBox::warning(this, tr("Warning"), tr("User Name must not be empty!"));
			return;
		}

		if (lineEditPassword->text().isEmpty()) {
			listWidget->setCurrentRow(0);
			lineEditPassword->setFocus();
			QMessageBox::warning(this, tr("Warning"), tr("Password must not be empty!"));
			return;
		}

		if (!(listWidgetBSC->count() > listWidgetBSC->findItems("", Qt::MatchFixedString).count())) {
			listWidget->setCurrentRow(1);
			toolButtonAdd->setFocus();
			QMessageBox::warning(this, tr("Warning"), tr("IP List must not be empty!"));
			return;
		}

		if (!lineEditGIS->text().isEmpty()) {
			QFile gisFile(lineEditGIS->text());

			if (!gisFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
				listWidget->setCurrentRow(1);
				toolButtonBrowse->setFocus();
				QMessageBox::warning(this, tr("Warning"), tr("GIS file cann't be opened!"));
				return;
			}
		}

		getConfig();

		if (!g_config.store())
			QMessageBox::warning(this, tr("Warning"), tr("Configuration cann't be saved!"));
	}

	QDialog::done(reslt);
}

void CConfigDlg::addIP() {
	QListWidgetItem *newItem;
	QList<QListWidgetItem *> ipItems = listWidgetBSC->findItems("", Qt::MatchFixedString);

	if (ipItems.count())
		newItem = ipItems.first();
	else {
		newItem = new QListWidgetItem(listWidgetBSC);
		newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
		newItem->setText("");
	}

	newItem->setSelected(true);
	listWidgetBSC->editItem(newItem);
}

void CConfigDlg::deleteIP() {
	foreach (QListWidgetItem *ipItem, listWidgetBSC->selectedItems())
		delete ipItem;
}

void CConfigDlg::browseFile() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open GIS File"), lineEditGIS->text(), tr("Excel files (*.xls *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"));

	if (!fileName.isEmpty())
		lineEditGIS->setText(fileName);
}
