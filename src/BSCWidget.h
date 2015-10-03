/*
 * BSCWidget.h
 *
 *  Created on: Jan 29, 2010
 *      Author: jalil
 */

#ifndef BSCWIDGET_H_
#define BSCWIDGET_H_

#include <QtGui/QTreeWidget>

enum {
	BSC_INDEX = 0,
	BSC_IP = 1,
	BSC_STATE = 2,
	BSC_SITES = 3,
	BSC_ALARMS = 4,
	BSC_LOCATIONS = 5,
};

class CBSC;

class CBSCWidgetItem: public QTreeWidgetItem {
public:
	CBSCWidgetItem(QTreeWidget* parent);

private:
	bool operator<(const QTreeWidgetItem &other) const;
};

class CBSCWidget: public QTreeWidget {
	Q_OBJECT

public:
	CBSCWidget(QWidget *parent = 0);

private:
	void setItem(QTreeWidgetItem *item, const CBSC &bsc);

private slots:
	void currentBSCChanged(QTreeWidgetItem *item);

signals:
	void bscSelected(const QString &ip);

public slots:
	void resetList();
	void updateBSC(const QString &ip);
	void selectBSC(const QList<quint32> &id);
};

#endif /* BSCWIDGET_H_ */
