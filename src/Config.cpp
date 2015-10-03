/*
 * Config.cpp
 *
 *  Created on: Jan 29, 2010
 *      Author: jalil
 */

#include <QtCore/QtCore>

#include "Config.h"

CConfig g_config;

CConfig::CConfig() {
	m_user = "PERSIAL";
	m_password = "SITEMON";
	m_save = false;

	m_ips.clear();
	m_ips << "10.2.26.32";

	m_file.clear();

	m_interval = UPDATE_INTERVAL;
	m_limit = LOG_LIMIT;
}

bool CConfig::load() {
	QFile file("data/config");

	if (!file.open(QIODevice::ReadOnly))
		return false;

	QString user = "PERSIAL";
	QString password = "SITEMON";

	QDataStream cfgIn(&file);
	cfgIn >> user >> password >> m_save >> m_ips >> m_file >> m_interval >> m_limit;

	if (m_save) {
		m_user = user;
		m_password = password;
	}

	return true;
}

bool CConfig::store() {
	QFile file("data/config");

	if (!file.open(QIODevice::WriteOnly))
		return false;

	QString user = "PERSIAL";
	QString password = "SITEMON";

	if (m_save) {
		user = m_user;
		password = m_password;
	}

	QDataStream cfgOut(&file);
	cfgOut << user << password << m_save << m_ips << m_file << m_interval << m_limit;

	return true;
}
