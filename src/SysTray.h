/*
 * SysTray.h
 *
 *  Created on: Dec 9, 2009
 *      Author: jalil
 */

#ifndef SYSTRAY_H_
#define SYSTRAY_H_

#include <QtGui/QSystemTrayIcon>

class CSysTray: public QSystemTrayIcon {
	Q_OBJECT

public:
	CSysTray(QObject *parent = 0);

private:
	enum {
		TICK_INTERVAL = 2
	};

	int m_icon;

	QTimer *m_timer;

private slots:
	void timerTicked();

public slots:
	void updateMSG();
	void startAnimation();
	void stopAnimation();
};

#endif /* SYSTRAY_H_ */
