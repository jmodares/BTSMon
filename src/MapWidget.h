/*
 * MapWidget.h
 *
 *  Created on: Dec 5, 2009
 *      Author: jalil
 */

#ifndef MAPWIDGET_H_
#define MAPWIDGET_H_

#include <marble/MarbleWidget.h>
using namespace Marble;

class CSite;
class QTimeLine;

class CMapWidget: public MarbleWidget {
	Q_OBJECT

public:
	CMapWidget(QWidget *parent = 0);

protected:
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	void customPaint(GeoPainter *painter);
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

private:
	enum {
		ZOOM_DURATION = 1,
		TRAVEL_DURATION = 2,

		ANNOTATION_DELAY = 5,

		BUSY_LOW = 20,
		BUSY_HIGH = 40,
		BUSY_TRANSPARENCY = 50,

		INTERNATIONAL_DISTANCE = 5,
		INTERSTATE_DISTANCE = 3,
		INTRASTATE_DISTANCE = 1,

		LEVEL_05 = 1525,
		LEVEL_07 = 1803,
		LEVEL_09 = 2080,
		LEVEL_13 = 2634,
		LEVEL_15 = 2912,
		LEVEL_16 = 3050,
		LEVEL_17 = 3189,
		LEVEL_18 = 3328,

		SITE_LEVEL = LEVEL_15,
		SITE_VIEW = LEVEL_16,
	};

	enum EStage {
		STAGE_ONE, STAGE_TWO, STAGE_THREE,
	};

	QTimer *m_timer;
	QTimeLine *m_timeLine;

	EStage m_stage;

	qreal m_lon;
	qreal m_lat;
	qreal m_siteLON;
	qreal m_siteLAT;

	int m_zoom;
	int m_level;

	QList<quint32> m_id;

	bool m_animation;

private:
	void drawSite(GeoPainter *painter, const CSite &site);
	void showSiteInfo(GeoPainter *painter);

private Q_SLOTS:
	void timerTicked();
	void valueChanged(qreal factor);
	void stageFinished();

public Q_SLOTS:
	void centerSite(const QList<quint32> &id);
	void goAnimation(bool animation);

signals:
	void siteSelected(const QList<quint32> &id);
};

#endif /* MAPWIDGET_H_ */
