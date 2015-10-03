/*
 * SiteWidget.h
 *
 *  Created on: Dec 6, 2009
 *      Author: jalil
 */

#ifndef SITEWIDGET_H_
#define SITEWIDGET_H_

#include <QtGui/QTreeWidget>

enum {
	SITE_INDEX = 0,
	SITE_BUSY = 1,
	SITE_ALARM = 2,
	SITE_BTS_1 = 3,
	SITE_BTS_2 = 4,
	SITE_BTS_3 = 5,
};

class CSite;

class CSiteWidgetItem: public QTreeWidgetItem {
public:
	CSiteWidgetItem(QTreeWidget* parent);

private:
	bool operator<(const QTreeWidgetItem &other) const;
};

class CSiteWidget: public QTreeWidget {
	Q_OBJECT

public:
	CSiteWidget(QWidget *parent = 0);

private:
	enum {
		SITE_DATA = 0, SITE_LAC = Qt::UserRole + 0, SITE_CI = Qt::UserRole + 1,

		ANNOTATION_DELAY = 5,
	};

	QFont *m_font;
	QTimer *m_timer;
	QTreeWidgetItem *m_item;

protected:
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);

private:
	QList<quint32> siteID(QTreeWidgetItem *item);
	void setItem(QTreeWidgetItem *item, const CSite &site);

	void showSiteInfo();

private slots:
	void currentSiteChanged(QTreeWidgetItem *item);
	void siteEntered(QTreeWidgetItem *item);
	void timerTicked();

signals:
	void siteDeleted(const QList<quint32> &id);
	void siteSelected(const QList<quint32> &id);

public slots:
	void resetList();
	void updateZEEI(const QString &ip);
	void updateZEOL(const QString &ip);
	void updateZERO(const QList<quint32> &id);

	void selectSite(const QString &ip);
	void selectSite(const QList<quint32> &id);
};

#endif /* SITEWIDGET_H_ */
