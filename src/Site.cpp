/*
 * Site.cpp
 *
 *  Created on: Jan 24, 2010
 *      Author: jalil
 */

#include <QtCore/QtCore>

#include "Site.h"

QList<CGIS> g_locs;
QList<CBSC> g_bscs;

CGIS::CGIS(const QList<quint32> &id, const QList<qreal> &loc) :
	m_id(id), m_loc(loc) {
}

CBSC::CBSC(const QString &ip) :
	m_ip(ip) {
	m_state = "UNCHECKED";
}

QList<quint32> CSite::getID() const {
	QStringList zeeiList = m_zeei.split("\n");
	QStringList idLine = zeeiList[1].split(" ");
	idLine.removeAll("");

	return QList<quint32> () << idLine[0].toUInt() << idLine[1].toUInt();
}

quint32 CSite::getBCF() const {
	QStringList zeeiList = m_zeei.split("\n");
	QStringList bcfLine = zeeiList[0].split(" ");
	bcfLine.removeAll("");

	return bcfLine[0].remove("BCF-").toUInt();
}

QString CSite::alarm() const {
	QStringList zeolList = m_zeol.split("\n");
	QStringList alarmList = zeolList.filter(QRegExp("^\\s{3}\\(\\d{5}\\)\\s"));

	QStringList alarms;

	for (int i = 0; i < alarmList.count(); i++) {
		QStringList codeLine = alarmList[i].split(" ");
		codeLine.removeAll("");

		alarms << codeLine[1];
	}

	alarms.removeDuplicates();
	return alarms.join(" | ");
}

QStringList CSite::traffic(quint16 &busy) const {
	QStringList zeeiList = m_zeei.split("\n");
	QStringList btsList = zeeiList.filter("BTS-");

	busy = 0;
	QStringList btss;

	for (int i = 0; i < btsList.count(); i++) {
		QStringList btsLine = btsList[i].split(" ");
		btsLine.removeAll("");

		QString state = btsLine[btsLine.indexOf(btsLine.filter("BTS-").first()) + 2];
		if (state.contains("BL-")) {
			btss << state;
			continue;
		}

		QString fr = btsLine.takeLast();
		QString hr = btsLine.takeLast();

		quint8 btsBusy = fr.toUShort() + hr.toUShort();

		if (!btsBusy)
			btss << "EMPTY";
		else if (i < m_caps.count() && btsBusy > m_caps[i] - BUSY_LIMIT)
			btss << "FULL";
		else
			btss << QString::number(btsBusy);

		busy += btsBusy;
	}

	return btss;
}
