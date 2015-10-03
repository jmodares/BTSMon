/*
 * LogWidget.h
 *
 *  Created on: Jan 4, 2010
 *      Author: jalil
 */

#ifndef LOGWIDGET_H_
#define LOGWIDGET_H_

#include <QtGui/QTreeWidget>

enum {
	LOG_INDEX = 0,
	LOG_TIME = 1,
	LOG_BUSY = 2,
	LOG_ALARM = 3,
	LOG_BTS_1 = 4,
	LOG_BTS_2 = 5,
	LOG_BTS_3 = 6,
};

class CLog;

class CLogWidgetItem: public QTreeWidgetItem {
public:
	CLogWidgetItem(QTreeWidget* parent);

private:
	bool operator<(const QTreeWidgetItem &other) const;
};

class CLogWidget: public QTreeWidget {
	Q_OBJECT

public:
	CLogWidget(QWidget *parent = 0);

private:
	QList<quint32> m_id;

private:
	void setItem(QTreeWidgetItem *item, const CLog &log);

public slots:
	void setLog(const QList<quint32> &id);
	void unsetLog(const QList<quint32> &id);
	void updateLog(const QList<quint32> &id);
};

#endif /* LOGWIDGET_H_ */
