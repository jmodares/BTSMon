/*
 * SysTray.cpp
 *
 *  Created on: Dec 9, 2009
 *      Author: jalil
 */

#include <QtGui/QtGui>

#include "Site.h"
#include "Icon.h"
#include "SysTray.h"

CSysTray::CSysTray(QObject *parent) :
	QSystemTrayIcon(parent) {

	m_timer = new QTimer(this);
	m_timer->setInterval(TICK_INTERVAL * 100);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(timerTicked()));

	m_icon = 0;
	setIcon(g_icons[(m_icon++ % 6) + ICON_PERSIAL_0]);
}

void CSysTray::updateMSG() {
	foreach (CBSC bsc, g_bscs)
		if (!bsc.m_state.contains("CHECKED")) {
			showMessage("Warning!", "SiteMon couldn't check fully some BSCs!", Warning);
			return;
		}
}

void CSysTray::timerTicked() {
	setIcon(g_icons[(m_icon++ % 6) + ICON_PERSIAL_0]);
}

void CSysTray::startAnimation() {
	m_icon = 0;
	m_timer->start();
	setIcon(g_icons[(m_icon++ % 6) + ICON_PERSIAL_0]);
	setToolTip(tr("SiteMon is checking sites!"));
}

void CSysTray::stopAnimation() {
	m_icon = 0;
	m_timer->stop();
	setIcon(g_icons[(m_icon++ % 6) + ICON_PERSIAL_0]);
	setToolTip(tr("Sites have been checked!"));
}
