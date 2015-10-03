/*
 * Config.h
 *
 *  Created on: Jan 29, 2010
 *      Author: jalil
 */

#ifndef CONFIG_H_
#define CONFIG_H_

class CConfig {
public:
	CConfig();

private:
	enum {
		UPDATE_INTERVAL = 15, LOG_LIMIT = 100,
	};

public:
	QString m_user;
	QString m_password;
	bool m_save;

	QStringList m_ips;
	QString m_file;

	quint8 m_interval;
	quint16 m_limit;

public:
	bool load();
	bool store();
};

extern CConfig g_config;

#endif /* CONFIG_H_ */
