/*
 * SiteWidget.cpp
 *
 *  Created on: Dec 6, 2009
 *      Author: jalil
 */

#include <QtGui/QtGui>

#include "Site.h"
#include "SiteWidget.h"

CSiteWidgetItem::CSiteWidgetItem(QTreeWidget* parent) :
	QTreeWidgetItem(parent) {
}

bool CSiteWidgetItem::operator<(const QTreeWidgetItem &other) const {
	int column = treeWidget()->sortColumn();

	switch (column) {
	case SITE_INDEX:
	case SITE_BUSY:
		return text(column).toUInt() < other.text(column).toUInt();

	case SITE_ALARM:
		if (text(column).contains(" | ") || other.text(column).contains(" | "))
			return text(column).split(" | ").count() < other.text(column).split(" | ").count();

		return text(column).toUInt() < other.text(column).toUInt();

	case SITE_BTS_1:
	case SITE_BTS_2:
	case SITE_BTS_3:
		return text(column).split(" / ").first().toUShort() < other.text(column).split(" / ").first().toUShort();
	}

	return QTreeWidgetItem::operator<(other);
}

CSiteWidget::CSiteWidget(QWidget *parent) :
	QTreeWidget(parent) {
	setIndentation(0);
	setMouseTracking(true);
	setSortingEnabled(true);
	setAllColumnsShowFocus(true);
	setAlternatingRowColors(true);
	header()->setSortIndicatorShown(true);

	headerItem()->setText(SITE_INDEX, "INDEX");
	headerItem()->setText(SITE_BUSY, "BUSY");
	headerItem()->setText(SITE_ALARM, "ALARM");
	headerItem()->setText(SITE_BTS_1, "BTS-1");
	headerItem()->setText(SITE_BTS_2, "BTS-2");
	headerItem()->setText(SITE_BTS_3, "BTS-3");

	connect(this, SIGNAL(itemEntered(QTreeWidgetItem*, int)), this, SLOT(siteEntered(QTreeWidgetItem*)));
	connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(currentSiteChanged(QTreeWidgetItem*)));

	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
	m_timer->setInterval(ANNOTATION_DELAY * 100);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(timerTicked()));

	m_font = new QFont(QToolTip::font());
}

QList<quint32> CSiteWidget::siteID(QTreeWidgetItem *item) {
	QList<quint32> id;
	id << item->data(SITE_DATA, SITE_LAC).toUInt() << item->data(SITE_DATA, SITE_CI).toUInt();

	return id;
}

void CSiteWidget::setItem(QTreeWidgetItem *item, const CSite &site) {
	quint16 busy;
	QStringList btss = site.traffic(busy);
	QList<quint32> id = site.getID();

	item->setData(SITE_DATA, SITE_LAC, id[0]);
	item->setData(SITE_DATA, SITE_CI, id[1]);

	if (site.m_loc.isEmpty())
		item->setTextColor(SITE_INDEX, QColor(255, 0, 0));
	else
		item->setTextColor(SITE_INDEX, QColor(0, 0, 0));

	item->setText(SITE_BUSY, QString::number(busy));
	item->setText(SITE_ALARM, site.alarm());

	for (int i = 0; i < btss.count(); ++i) {
		QString state = btss[i];

		if (i < site.m_caps.count())
			state += " / " + QString::number(site.m_caps[i]);

		item->setText(SITE_BTS_1 + i, state);
	}
}

void CSiteWidget::resetList() {
	clear();

	int index = 1;
	foreach (CBSC bsc, g_bscs)
		foreach (CSite site, bsc.m_sites) {
			QTreeWidgetItem *item = new CSiteWidgetItem(this);

			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			item->setText(SITE_INDEX, QString::number(index++));
			setItem(item, site);
		}

	header()->resizeSections(QHeaderView::ResizeToContents);
}

void CSiteWidget::updateZEEI(const QString &ip) {
	foreach (CBSC bsc, g_bscs)
		if (bsc.m_ip == ip) {
			foreach (CSite site, bsc.m_sites) {
				QTreeWidgetItem *item = NULL;
				QList<quint32> id = site.getID();

				for (int i = 0; i < topLevelItemCount(); ++i)
					if (id == siteID(topLevelItem(i))) {
						item = topLevelItem(i);
						break;
					}

				if (!item) {
					item = new CSiteWidgetItem(this);
					item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
				}

				setItem(item, site);
			}

			break;
		}

	QTreeWidgetItem *nextItem = topLevelItem(0);
	QTreeWidgetItem *indexItem = NULL;

	while (nextItem) {
		indexItem = nextItem;
		nextItem = itemBelow(indexItem);

		bool found = false;
		QList<quint32> id = siteID(indexItem);

		int index = 0;

		foreach (CBSC bsc, g_bscs) {
			foreach (CSite site, bsc.m_sites) {
				index++;

				if (id == site.getID()) {
					found = true;
					indexItem->setText(SITE_INDEX, QString::number(index));

					break;
				}
			}

			if (found)
				break;
		}

		if (!found) {
			emit siteDeleted(id);
			delete indexItem;
		}
	}
}

void CSiteWidget::updateZEOL(const QString &ip) {
	foreach (CBSC bsc, g_bscs)
		if (bsc.m_ip == ip) {
			foreach (CSite site, bsc.m_sites) {
				QList<quint32> id = site.getID();

				for (int i = 0; i < topLevelItemCount(); ++i)
					if (id == siteID(topLevelItem(i))) {
						topLevelItem(i)->setText(SITE_ALARM, site.alarm());
						break;
					}
			}

			return;
		}
}

void CSiteWidget::updateZERO(const QList<quint32> &id) {
	foreach (CBSC bsc, g_bscs)
		foreach (CSite site, bsc.m_sites)
			if (id == site.getID()) {
				for (int i = 0; i < topLevelItemCount(); ++i)
					if (id == siteID(topLevelItem(i))) {
						setItem(topLevelItem(i), site);
						return;
					}

				return;
			}
}

void CSiteWidget::selectSite(const QList<quint32> &id) {
	for (int i = 0; i < topLevelItemCount(); ++i)
		if (id == siteID(topLevelItem(i))) {
			setCurrentItem(topLevelItem(i));
			return;
		}
}

void CSiteWidget::selectSite(const QString &ip) {
	foreach (CBSC bsc, g_bscs)
		if (bsc.m_ip == ip) {
			if (bsc.m_sites.count()) {
				QTreeWidgetItem *item = currentItem();

				if (item) {
					QList<quint32> id = siteID(item);

					foreach (CSite site, bsc.m_sites)
						if (id == site.getID())
							return;
				}

				QList<quint32> id = bsc.m_sites.first().getID();

				for (int i = 0; i < topLevelItemCount(); ++i)
					if (id == siteID(topLevelItem(i))) {
						setCurrentItem(topLevelItem(i));
						return;
					}
			}

			return;
		}
}

void CSiteWidget::currentSiteChanged(QTreeWidgetItem *item) {
	if (item)
		emit siteSelected(siteID(item));
}

void CSiteWidget::siteEntered(QTreeWidgetItem *item) {
	m_item = item;
	m_timer->start();
}

void CSiteWidget::showSiteInfo() {
	if (!m_item)
		return;

	QList<quint32> id = siteID(m_item);

	foreach (CBSC bsc, g_bscs)
		foreach (CSite site, bsc.m_sites)
			if (id == site.getID()) {
				if (qApp->keyboardModifiers() == Qt::ControlModifier && !site.m_zeol.isEmpty())
					QToolTip::showText(QCursor::pos(), site.m_zeol);
				else
					QToolTip::showText(QCursor::pos(), site.m_zeei);

				return;
			}
}

void CSiteWidget::timerTicked() {
	if (m_item == itemAt(viewport()->mapFromGlobal(QCursor::pos())))
		showSiteInfo();
}

void CSiteWidget::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Control && QToolTip::isVisible())
		showSiteInfo();

	QTreeWidget::keyPressEvent(e);
}

void CSiteWidget::keyReleaseEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Control && QToolTip::isVisible())
		showSiteInfo();

	QTreeWidget::keyReleaseEvent(e);
}

void CSiteWidget::enterEvent(QEvent *e) {
	setFocus();

	QFont fnt = *m_font;
	fnt.setFamily("Courier");
	fnt.setFixedPitch(true);

	QToolTip::setFont(fnt);

	QTreeWidget::enterEvent(e);
}

void CSiteWidget::leaveEvent(QEvent *e) {
	QToolTip::hideText();
	QToolTip::setFont(*m_font);

	QTreeWidget::leaveEvent(e);
}
