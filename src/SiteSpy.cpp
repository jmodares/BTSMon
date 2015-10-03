/*
 * SiteSpy.cpp
 *
 *  Created on: Dec 7, 2009
 *      Author: jalil
 */

#include <QtCore/QtCore>

#include <QtTelnet>

#include "Site.h"
#include "Config.h"
#include "SiteSpy.h"

CSiteSpy::CSiteSpy(QObject *parent) :
	QObject(parent) {

	m_telnet = new QtTelnet(this);
	connect(m_telnet, SIGNAL(message(QString)), this, SLOT(bscMessage(QString)));

	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
	m_timer->setInterval(SESSION_TIMEOUT * 1000);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(timerTicked()));
}

void CSiteSpy::bscMessage(const QString &msg) {
	m_msg.append(msg);

	switch (m_state) {
	case STATE_USERNAME:
		if (m_msg.contains("PLEASE WAIT")) {
			m_timer->stop();

			g_bscs[m_bsc].m_state = "WAIT";
			checkNextBSC();
		} else if (m_msg.contains("USERNAME < ")) {
			m_timer->stop();

			m_msg.clear();
			m_state = STATE_PASSWORD;

			m_telnet->sendData(g_config.m_user);
			m_timer->start();
		}
		break;
	case STATE_PASSWORD:
		if (m_msg.contains("PASSWORD < ")) {
			m_timer->stop();

			m_msg.clear();
			m_state = STATE_READY;

			m_telnet->sendData(g_config.m_password);
			m_timer->start();
		}
		break;
	case STATE_READY:
		if (m_msg.contains("AUTHORIZATION FAILURE")) {
			m_timer->stop();

			g_bscs[m_bsc].m_state = "AUTHORIZATION FAILURE";
			checkNextBSC();
		} else if (m_msg.contains("\r\n< ")) {
			m_timer->stop();

			m_msg.clear();
			m_state = STATE_COMMAND;
			m_command = COMMAND_ZEEI;

			m_telnet->sendData("ZEEI;");
			m_timer->start();
		}
		break;
	case STATE_COMMAND:
		if (m_msg.contains("NOT AUTHORIZED")) {
			m_timer->stop();

			switch (m_command) {
			case COMMAND_ZEEI:
				g_bscs[m_bsc].m_state = "ZEEI NOT AUTHORIZED";
				break;
			case COMMAND_ZEOL:
				g_bscs[m_bsc].m_state = "ZEOL NOT AUTHORIZED";
				break;
			case COMMAND_ZERO:
				g_bscs[m_bsc].m_state = "ZERO NOT AUTHORIZED";
				break;
			}
			checkNextBSC();
		} else if (m_msg.contains("\r\n< ")) {
			m_timer->stop();

			switch (m_command) {
			case COMMAND_ZEEI:
				handleZEEI();

				m_msg.clear();
				m_command = COMMAND_ZEOL;

				m_telnet->sendData("ZEOL;");
				m_timer->start();
				break;
			case COMMAND_ZEOL:
				handleZEOL();

				m_command = COMMAND_ZERO;
				m_site = -1;
				checkNextSite();
				break;
			case COMMAND_ZERO:
				handleZERO();
				checkNextSite();
				break;
			}
		}
		break;
	}
}

void CSiteSpy::handleZEEI() {
	QStringList zeeiList = m_msg.split("\r\n");
	QStringList bcfList = zeeiList.filter(QRegExp("^BCF-\\d{4}"));

	QList<CSite> newSites;

	int begin = 0;
	int end = 0;

	for (int i = 0; i < bcfList.count(); i++) {
		begin = zeeiList.indexOf(bcfList[i], end + 1);
		end = zeeiList.indexOf("", begin + 1);
		QStringList siteZEEI = zeeiList.mid(begin, end - begin);

		CSite newSite;
		newSite.m_zeei = siteZEEI.join("\n");

		bool found = false;
		QList<quint32> id = newSite.getID();

		QMutableListIterator<CSite> iSite(g_bscs[m_bsc].m_sites);
		while (iSite.hasNext()) {
			CSite &oldSite = iSite.next();
			if (id == oldSite.getID()) {
				found = true;
				oldSite.m_zeei = newSite.m_zeei;
				break;
			}
		}

		if (!found) {
			foreach (CGIS gis, g_locs)
				if (id == gis.m_id) {
					newSite.m_loc = gis.m_loc;
					break;
				}

			g_bscs[m_bsc].m_sites << newSite;
		}

		newSites << newSite;
	}

	QMutableListIterator<CSite> iSite(g_bscs[m_bsc].m_sites);
	while (iSite.hasNext()) {
		bool found = false;
		QList<quint32> id = iSite.next().getID();

		foreach (CSite newSite, newSites)
			if (id == newSite.getID()) {
				found = true;
				break;
			}

		if (!found)
			iSite.remove();
	}

	emit zeeiChecked(g_bscs[m_bsc].m_ip);
}

void CSiteSpy::handleZEOL() {
	QStringList zeolList = m_msg.split("\r\n");
	QStringList alarmList = zeolList.filter(QRegExp("^\\*{1,3}\\s+ALARM\\s"));

	QMutableListIterator<CSite> iSite(g_bscs[m_bsc].m_sites);
	while (iSite.hasNext())
		iSite.next().m_zeol.clear();

	int begin = 0;
	int end = 0;

	for (int i = 0; i < alarmList.count(); i++) {
		begin = zeolList.lastIndexOf("", zeolList.indexOf(alarmList[i], end + 1));
		end = zeolList.indexOf("", begin + 1);
		QStringList siteZEOL = zeolList.mid(begin + 1, end - begin);

		if (!siteZEOL[0].contains("BCF-"))
			continue;

		QString siteBCF = siteZEOL[0].split(" ").filter("BCF-").first();

		QMutableListIterator<CSite> iSite(g_bscs[m_bsc].m_sites);
		while (iSite.hasNext()) {
			CSite &site = iSite.next();

			if (site.m_zeei.contains(siteBCF)) {
				site.m_zeol += '\n' + siteZEOL.join("\n");
				break;
			}
		}
	}

	emit zeolChecked(g_bscs[m_bsc].m_ip);
}

void CSiteSpy::handleZERO() {
	QStringList zeroList = m_msg.split("\r\n");
	QStringList btsList = zeroList.filter(QRegExp("^BTS-\\d{4}"));

	QMap<QString, quint8> siteCAP;

	int begin = 0;
	int end = 0;

	for (int i = 0; i < btsList.count(); i++) {
		begin = zeroList.indexOf(btsList[i], end + 1);
		end = zeroList.indexOf("------------------------------------------------------------------------------", begin + 1);

		QStringList chList = zeroList.mid(end + 1, 8);
		QString siteBTS = btsList[i].split(" ").filter("BTS-").first();
		quint8 btsCAP = (chList.filter("TCHD").count() + chList.filter("TCHH").count()) * 2 + chList.filter("TCHF").count();

		if (siteCAP.contains(siteBTS))
			btsCAP += siteCAP.value(siteBTS);

		siteCAP.insert(siteBTS, btsCAP);
	}

	g_bscs[m_bsc].m_sites[m_site].m_caps = siteCAP.values();

	emit zeroChecked(g_bscs[m_bsc].m_sites[m_site].getID());
}

void CSiteSpy::startCheck() {
	m_bsc = -1;
	checkNextBSC();
}

void CSiteSpy::stopCheck() {
	m_timer->stop();
	m_telnet->socket()->abort();
}

void CSiteSpy::checkNextBSC() {
	if (m_bsc != -1) {
		stopCheck();
		emit bscChecked(g_bscs[m_bsc].m_ip);
	}

	if (!(++m_bsc < g_bscs.count())) {
		emit checkFinished();
		return;
	}

	m_msg.clear();
	m_state = STATE_USERNAME;

	m_telnet->connectToHost(g_bscs[m_bsc].m_ip);
	m_timer->start();
}

void CSiteSpy::checkNextSite() {
	forever {
		if (!(++m_site < g_bscs[m_bsc].m_sites.count())) {
			g_bscs[m_bsc].m_state = "CHECKED";
			checkNextBSC();
			return;
		}

		if (g_bscs[m_bsc].m_sites[m_site].m_caps.isEmpty())
			break;

		while (!(g_bscs[m_bsc].m_sites[m_site].m_logs.count() < g_config.m_limit))
			g_bscs[m_bsc].m_sites[m_site].m_logs.removeFirst();

		CLog log;
		CSite site = g_bscs[m_bsc].m_sites[m_site];

		log.m_btss = site.traffic(log.m_busy);
		log.m_time = QDateTime::currentDateTime().toString();
		log.m_alarm = site.alarm();

		g_bscs[m_bsc].m_sites[m_site].m_logs << log;

		emit siteLogUpdated(site.getID());
	}

	m_msg.clear();

	m_telnet->sendData(QString("ZERO:BCF=%1;").arg(g_bscs[m_bsc].m_sites[m_site].getBCF()));
	m_timer->start();
}

void CSiteSpy::timerTicked() {
	switch (m_state) {
	case STATE_USERNAME:
		g_bscs[m_bsc].m_state = "TIMEOUT ON USERNAME";
		break;
	case STATE_PASSWORD:
		g_bscs[m_bsc].m_state = "TIMEOUT ON PASSWORD";
		break;
	case STATE_READY:
		g_bscs[m_bsc].m_state = "TIMEOUT ON LOGIN";
		break;
	case STATE_COMMAND:
		switch (m_command) {
		case COMMAND_ZEEI:
			g_bscs[m_bsc].m_state = "TIMEOUT ON ZEEI";
			break;
		case COMMAND_ZEOL:
			g_bscs[m_bsc].m_state = "TIMEOUT ON ZEOL";
			break;
		case COMMAND_ZERO:
			g_bscs[m_bsc].m_state = QString("TIMEOUT ON ZERO ON SITE %1").arg(m_site + 1);
			break;
		}
		break;
	}

	checkNextBSC();
}
