/*
 * BSCWidget.cpp
 *
 *  Created on: Jan 29, 2010
 *      Author: jalil
 */

#include <QtGui/QtGui>

#include "Site.h"
#include "BSCWidget.h"

CBSCWidgetItem::CBSCWidgetItem(QTreeWidget* parent) :
	QTreeWidgetItem(parent) {
}

bool CBSCWidgetItem::operator<(const QTreeWidgetItem &other) const {
	int column = treeWidget()->sortColumn();

	switch (column) {
	case BSC_INDEX:
	case BSC_SITES:
	case BSC_ALARMS:
	case BSC_LOCATIONS:
		return text(column).toUInt() < other.text(column).toUInt();
	}

	return QTreeWidgetItem::operator<(other);
}

CBSCWidget::CBSCWidget(QWidget *parent) :
	QTreeWidget(parent) {
	setIndentation(0);
	setSortingEnabled(true);
	setAllColumnsShowFocus(true);
	setAlternatingRowColors(true);
	header()->setSortIndicatorShown(true);

	headerItem()->setText(BSC_INDEX, "INDEX");
	headerItem()->setText(BSC_IP, "IP");
	headerItem()->setText(BSC_STATE, "STATE");
	headerItem()->setText(BSC_SITES, "SITES");
	headerItem()->setText(BSC_ALARMS, "ALARMS");
	headerItem()->setText(BSC_LOCATIONS, "LOCATIONS");

	connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(currentBSCChanged(QTreeWidgetItem*)));
}

void CBSCWidget::setItem(QTreeWidgetItem *item, const CBSC &bsc) {
	item->setText(BSC_IP, bsc.m_ip);
	item->setText(BSC_STATE, bsc.m_state);
	item->setText(BSC_SITES, QString::number(bsc.m_sites.count()));

	int alarms = 0;
	int locations = 0;

	foreach (CSite site, bsc.m_sites) {
		if (!site.m_zeol.isEmpty())
			alarms++;

		if (!site.m_loc.isEmpty())
			locations++;
	}

	item->setText(BSC_ALARMS, QString::number(alarms));
	item->setText(BSC_LOCATIONS, QString::number(locations));
}

void CBSCWidget::resetList() {
	clear();

	int index = 1;
	foreach (CBSC bsc, g_bscs) {
		QTreeWidgetItem *item = new CBSCWidgetItem(this);

		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item->setText(BSC_INDEX, QString::number(index++));
		setItem(item, bsc);
	}

	header()->resizeSections(QHeaderView::ResizeToContents);
}

void CBSCWidget::updateBSC(const QString &ip) {
	QList<QTreeWidgetItem*> items = findItems(ip, Qt::MatchFixedString, BSC_IP);

	if (!items.count())
		return;

	foreach (CBSC bsc, g_bscs)
		if (bsc.m_ip == ip) {
			setItem(items.first(), bsc);
			return;
		}
}

void CBSCWidget::currentBSCChanged(QTreeWidgetItem *item) {
	if (item)
		emit bscSelected(item->text(BSC_IP));
}

void CBSCWidget::selectBSC(const QList<quint32> &id) {
	foreach (CBSC bsc, g_bscs)
		foreach (CSite site, bsc.m_sites)
			if (id == site.getID()) {
				QList<QTreeWidgetItem*> items = findItems(bsc.m_ip, Qt::MatchFixedString, BSC_IP);

				if (!items.count())
					return;

				setCurrentItem(items.first());
				return;
			}
}
