/*
 * SiteSpy.h
 *
 *  Created on: Dec 7, 2009
 *      Author: jalil
 */

#ifndef SITESPY_H_
#define SITESPY_H_

#include <QtCore/QObject>

class QTimer;
class QtTelnet;

class CSiteSpy: public QObject {
	Q_OBJECT

public:
	CSiteSpy(QObject *parent = 0);

private:
	enum {
		SESSION_TIMEOUT = 30,
	};

	enum EState {
		STATE_USERNAME, STATE_PASSWORD, STATE_READY, STATE_COMMAND,
	};

	enum ECommand {
		COMMAND_ZERO, COMMAND_ZEEI, COMMAND_ZEOL,
	};

	int m_bsc;
	int m_site;

	EState m_state;
	ECommand m_command;

	QString m_msg;

	QTimer *m_timer;
	QtTelnet *m_telnet;

signals:
	void zeeiChecked(const QString &ip);
	void zeolChecked(const QString &ip);
	void zeroChecked(const QList<quint32> &id);
	void bscChecked(const QString &ip);

	void siteLogUpdated(const QList<quint32> &id);
	void checkFinished();

private slots:
	void bscMessage(const QString &msg);
	void timerTicked();

public slots:
	void startCheck();
	void stopCheck();

private:
	void checkNextBSC();
	void checkNextSite();

	void handleZEEI();
	void handleZEOL();
	void handleZERO();
};

#endif /* SITESPY_H_ */
