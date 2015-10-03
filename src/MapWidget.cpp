/*
 * MapWidget.cpp
 *
 *  Created on: Dec 5, 2009
 *      Author: jalil
 */

#include <QtGui/QtGui>

#include <marble/GeoPainter.h>

#include "Site.h"
#include "Icon.h"
#include "MapWidget.h"

#define BUSY_BOUND 0.01
#define SITE_BOUND 0.0005

CMapWidget::CMapWidget(QWidget *parent) :
	MarbleWidget(parent) {
	setMapThemeId("earth/openstreetmap/openstreetmap.dgml");
	inputHandler()->setMouseButtonPopupEnabled(Qt::LeftButton, false);

	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
	m_timer->setInterval(ANNOTATION_DELAY * 100);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(timerTicked()));

	m_timeLine = new QTimeLine(ZOOM_DURATION * 1000, this);
	m_timeLine->setFrameRange(0, 500);
	m_timeLine->setUpdateInterval(0);
	connect(m_timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(valueChanged(qreal)));
	connect(m_timeLine, SIGNAL(finished()), this, SLOT(stageFinished()));

	zoomView(LEVEL_05);
}

void CMapWidget::drawSite(GeoPainter *painter, const CSite &site) {
	quint16 busy;
	QStringList btss = site.traffic(busy);
	GeoDataCoordinates point(site.m_loc[1], site.m_loc[0], 0, GeoDataCoordinates::Degree);

	QColor color(Qt::transparent);

	if (!busy)
		color = Qt::yellow;
	else if (busy < BUSY_LOW)
		color = Qt::green;
	else if (busy < BUSY_HIGH)
		color = Qt::blue;
	else
		color = Qt::red;

	color.setAlpha(BUSY_TRANSPARENCY);
	painter->setBrush(color);
	painter->drawEllipse(point, BUSY_BOUND, BUSY_BOUND, true);

	if (zoom() < SITE_LEVEL)
		return;

	QPixmap sitePixmap;
	sitePixmap = g_icons[ICON_SITE_BLUE];

	int pixmapWidth = sitePixmap.width() / 3;
	int pixmapHeight = sitePixmap.height() / 3;
	int offset = 2 * pixmapWidth;

	QPainter pntr(&sitePixmap);

	if (!site.m_zeol.isEmpty())
		pntr.drawPixmap(0, 0, pixmapWidth, pixmapHeight, g_icons[ICON_STATE_ALARM]);

	for (int i = 0; i < btss.count(); ++i) {
		QPixmap btsPixmap;

		if (btss[i].contains("BL-"))
			btsPixmap = g_icons[ICON_BTS_ORANGE];
		else if (btss[i].contains("EMPTY"))
			btsPixmap = g_icons[ICON_BTS_LILAC];
		else if (btss[i].contains("FULL"))
			btsPixmap = g_icons[ICON_BTS_BLUE];
		else
			btsPixmap = g_icons[ICON_BTS_GREEN];

		pntr.drawPixmap(offset, i * pixmapHeight, pixmapWidth, pixmapHeight, btsPixmap);
	}

	painter->drawPixmap(point, sitePixmap.scaledToWidth(((16 - 256) * zoom() + (SITE_LEVEL * 256 - LEVEL_18 * 16)) / (SITE_LEVEL - LEVEL_18)));
}

void CMapWidget::showSiteInfo(GeoPainter *painter) {
	QFont fnt = painter->font();
	fnt.setFamily("Courier");
	fnt.setFixedPitch(true);

	QFontMetricsF metrics(fnt);

	painter->setFont(fnt);
	painter->setPen(palette().toolTipText().color());
	painter->setBrush(palette().toolTipBase());

	foreach (CBSC bsc, g_bscs)
		foreach (CSite site, bsc.m_sites)
			if (m_id == site.getID()) {
				GeoDataCoordinates point(site.m_loc[1], site.m_loc[0], 0, GeoDataCoordinates::Degree);

				if (qApp->keyboardModifiers() == Qt::ControlModifier && !site.m_zeol.isEmpty())
					painter->drawAnnotation(point, site.m_zeol, metrics.size(0, site.m_zeol) + metrics.size(0, "XX"));
				else
					painter->drawAnnotation(point, site.m_zeei, metrics.size(0, site.m_zeei) + metrics.size(0, "XX"));

				return;
			}
}

void CMapWidget::customPaint(GeoPainter *painter) {
	painter->setPen(Qt::NoPen);

	foreach (CBSC bsc, g_bscs)
		foreach (CSite site, bsc.m_sites)
			if (!site.m_loc.isEmpty())
				drawSite(painter, site);

	if (!m_id.isEmpty() && !m_timer->isActive())
		showSiteInfo(painter);
}

void CMapWidget::valueChanged(qreal factor) {
	setViewContext(Marble::Animation);

	switch (m_stage) {
	case STAGE_ONE:
	case STAGE_THREE:
		zoomView(m_zoom + factor * (m_level - m_zoom));
		break;
	case STAGE_TWO:
		centerOn(m_lon + factor * (m_siteLON - m_lon), m_lat + factor * (m_siteLAT - m_lat), false);
		break;
	}

	setViewContext(Marble::Still);
}

void CMapWidget::stageFinished() {
	switch (m_stage) {
	case STAGE_ONE:
		m_stage = STAGE_TWO;
		m_timeLine->setDuration(TRAVEL_DURATION * 1000);
		m_timeLine->start();
		break;
	case STAGE_TWO:
		m_stage = STAGE_THREE;
		m_zoom = zoom();
		m_level = SITE_VIEW;
		m_timeLine->setDuration(ZOOM_DURATION * 1000);
		m_timeLine->start();
		break;
	case STAGE_THREE:
		setInputEnabled(true);
		break;
	}
}

void CMapWidget::centerSite(const QList<quint32> &id) {
	if (!m_id.isEmpty())
		return;

	foreach (CBSC bsc, g_bscs)
		foreach (CSite site, bsc.m_sites)
			if (id == site.getID()) {
				if (site.m_loc.isEmpty())
					return;

				if (QTimeLine::Running == m_timeLine->state()) {
					m_timeLine->stop();
					setInputEnabled(true);
				}

				m_siteLAT = site.m_loc[0];
				m_siteLON = site.m_loc[1];

				if (!m_animation) {
					setViewContext(Marble::Animation);
					zoomView(SITE_VIEW);
					centerOn(m_siteLON, m_siteLAT, false);
					setViewContext(Marble::Still);

					return;
				}

				m_stage = STAGE_ONE;
				m_lon = centerLongitude();
				m_lat = centerLatitude();
				m_zoom = zoom();

				qreal dist = qMax(qAbs(m_siteLON - m_lon), qAbs(m_siteLAT - m_lat));

				if (dist > INTERNATIONAL_DISTANCE)
					m_level = LEVEL_05;
				else if (dist > INTERSTATE_DISTANCE)
					m_level = LEVEL_07;
				else if (dist > INTRASTATE_DISTANCE)
					m_level = LEVEL_09;
				else
					m_level = LEVEL_13;

				m_timeLine->setDuration(ZOOM_DURATION * 1000);
				m_timeLine->start();

				setInputEnabled(false);

				return;
			}
}

void CMapWidget::mouseMoveEvent(QMouseEvent *event) {
	if (zoom() >= SITE_LEVEL) {
		qreal posLon, siteLon;
		qreal posLat, siteLat;

		geoCoordinates(event->x(), event->y(), posLon, posLat, GeoDataCoordinates::Degree);

		foreach (CBSC bsc, g_bscs)
			foreach (CSite site, bsc.m_sites) {
				if (site.m_loc.isEmpty())
					continue;

				siteLat = site.m_loc[0];
				siteLon = site.m_loc[1];

				if (posLon > siteLon - SITE_BOUND && posLon < siteLon + SITE_BOUND && posLat > siteLat - SITE_BOUND && posLat < siteLat + SITE_BOUND) {
					if (m_id.isEmpty()) {
						m_id = site.getID();

						qApp->setOverrideCursor(Qt::PointingHandCursor);
						m_timer->start();
					}

					return;
				}
			}
	}

	if (!m_id.isEmpty()) {
		m_id.clear();
		qApp->restoreOverrideCursor();
		update();
	}
}

void CMapWidget::mouseReleaseEvent(QMouseEvent *e) {
	if (e->button() == Qt::LeftButton && !m_id.isEmpty())
		emit siteSelected(m_id);

	MarbleWidget::mouseReleaseEvent(e);
}

void CMapWidget::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Control && !m_id.isEmpty() && !m_timer->isActive()) {
		update();
		return;
	}

	MarbleWidget::keyPressEvent(e);
}

void CMapWidget::keyReleaseEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Control && !m_id.isEmpty() && !m_timer->isActive()) {
		update();
		return;
	}

	MarbleWidget::keyReleaseEvent(e);
}

void CMapWidget::enterEvent(QEvent *e) {
	setFocus();

	MarbleWidget::enterEvent(e);
}

void CMapWidget::leaveEvent(QEvent *e) {
	if (!m_id.isEmpty()) {
		m_id.clear();
		qApp->restoreOverrideCursor();
		update();
	}

	MarbleWidget::leaveEvent(e);
}

void CMapWidget::goAnimation(bool animation) {
	m_animation = animation;
}

void CMapWidget::timerTicked() {
	if (!m_id.isEmpty())
		update();
}
