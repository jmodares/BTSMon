/*
 * Site.h
 *
 *  Created on: Jan 24, 2010
 *      Author: jalil
 */

#ifndef SITE_H_
#define SITE_H_

class CGIS {
public:
	CGIS(const QList<quint32> &id, const QList<qreal> &loc);

public:
	QList<quint32> m_id;
	QList<qreal> m_loc;
};

class CLog {
public:
	QString m_time;
	quint16 m_busy;
	QString m_alarm;
	QStringList m_btss;
};

class CSite {
private:
	enum {
		BUSY_LIMIT = 5,
	};

public:
	QList<qreal> m_loc;
	QList<quint8> m_caps;

	QString m_zeei;
	QString m_zeol;

	QList<CLog> m_logs;

	QList<quint32> getID() const;
	quint32 getBCF() const;

	QString alarm() const;
	QStringList traffic(quint16 &busy) const;
};

class CBSC {
public:
	CBSC(const QString &ip);

public:
	QString m_ip;
	QString m_state;

	QList<CSite> m_sites;
};

extern QList<CGIS> g_locs;
extern QList<CBSC> g_bscs;

#endif /* SITE_H_ */
