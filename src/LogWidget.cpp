/*
 * LogWidget.cpp
 *
 *  Created on: Jan 4, 2010
 *      Author: jalil
 */

#include <QtGui/QtGui>

#include "Site.h"
#include "LogWidget.h"

CLogWidgetItem::CLogWidgetItem(QTreeWidget* parent) :
	QTreeWidgetItem(parent) {
}

bool CLogWidgetItem::operator<(const QTreeWidgetItem &other) const {
	int column = treeWidget()->sortColumn();

	switch (column) {
	case LOG_INDEX:
	case LOG_BUSY:
		return text(column).toUInt() < other.text(column).toUInt();

	case LOG_TIME:
		return QDateTime::fromString(text(column)) < QDateTime::fromString(other.text(column));

	case LOG_ALARM:
		if (text(column).contains(" | ") || other.text(column).contains(" | "))
			return text(column).split(" | ").count() < other.text(column).split(" | ").count();

		return text(column).toUInt() < other.text(column).toUInt();

	case LOG_BTS_1:
	case LOG_BTS_2:
	case LOG_BTS_3:
		return text(column).split(" / ").first().toUShort() < other.text(column).split(" / ").first().toUShort();
	}

	return QTreeWidgetItem::operator<(other);
}

CLogWidget::CLogWidget(QWidget *parent) :
	QTreeWidget(parent) {
	setIndentation(0);
	setSortingEnabled(true);
	setAllColumnsShowFocus(true);
	setAlternatingRowColors(true);
	header()->setSortIndicatorShown(true);

	headerItem()->setText(LOG_INDEX, "INDEX");
	headerItem()->setText(LOG_TIME, "TIME");
	headerItem()->setText(LOG_BUSY, "BUSY");
	headerItem()->setText(LOG_ALARM, "ALARM");
	headerItem()->setText(LOG_BTS_1, "BTS-1");
	headerItem()->setText(LOG_BTS_2, "BTS-2");
	headerItem()->setText(LOG_BTS_3, "BTS-3");
}

void CLogWidget::setItem(QTreeWidgetItem *item, const CLog &log) {
	item->setText(LOG_TIME, log.m_time);
	item->setText(LOG_BUSY, QString::number(log.m_busy));
	item->setText(LOG_ALARM, log.m_alarm);

	for (int i = 0; i < log.m_btss.count(); i++)
		item->setText(LOG_BTS_1 + i, log.m_btss[i]);
}

void CLogWidget::setLog(const QList<quint32> &id) {
	m_id = id;

	clear();

	int index = 1;

	foreach (CBSC bsc, g_bscs)
		foreach (CSite site, bsc.m_sites)
			if (m_id == site.getID()) {
				foreach (CLog log, site.m_logs) {
					QTreeWidgetItem *item = new CLogWidgetItem(this);

					item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
					item->setText(LOG_INDEX, QString::number(index++));
					setItem(item, log);
				}

				return;
			}

	header()->resizeSections(QHeaderView::ResizeToContents);
}

void CLogWidget::unsetLog(const QList<quint32> &id) {
	if (m_id == id) {
		m_id.clear();
		clear();
	}
}

void CLogWidget::updateLog(const QList<quint32> &id) {
	if (m_id != id)
		return;

	foreach (CBSC bsc, g_bscs)
		foreach (CSite site, bsc.m_sites)
			if (m_id == site.getID()) {
				QTreeWidgetItem *item = new CLogWidgetItem(this);

				item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
				setItem(item, site.m_logs.last());

				QTreeWidgetItem *nextItem = topLevelItem(0);
				QTreeWidgetItem *indexItem = NULL;

				while (nextItem) {
					indexItem = nextItem;
					nextItem = itemBelow(indexItem);

					bool found = false;
					QString time = indexItem->text(LOG_TIME);

					int index = 0;

					foreach(CLog log, site.m_logs) {
						index++;

						if (time == log.m_time) {
							found = true;
							indexItem->setText(LOG_INDEX, QString::number(index));

							break;
						}
					}

					if (!found) {
						delete indexItem;
						return;
					}
				}

				return;
			}
}
