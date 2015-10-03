/*
 * PlotWidget.cpp
 *
 *  Created on: Jan 18, 2010
 *      Author: jalil
 */

#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>

#include "Site.h"
#include "Config.h"
#include "PlotWidget.h"

class CPlotZoomer: public QwtPlotZoomer {
public:
	CPlotZoomer(QwtPlotCanvas *canvas) :
		QwtPlotZoomer(canvas) {
		setTrackerMode(AlwaysOn);
	}

private:
	CSite m_site;

public:
	virtual QwtText trackerText(const QPointF &pos) const {
		if ((pos.x() < 0) || (pos.x() >= m_site.m_logs.count()))
			return QwtText("");

		CLog log = m_site.m_logs[pos.x()];
		QString msg = "TIME:\t%1\nBUSY:\t%2\nALARM:\t%3\nBTS-1:\t%4\nBTS-2:\t%5\nBTS-3:\t%6";

		msg = msg.arg(log.m_time);
		msg = msg.arg(log.m_busy);
		msg = msg.arg(log.m_alarm);
		for (int i = 0; i < log.m_btss.count(); ++i)
			msg = msg.arg(log.m_btss[i]);

		QColor bg(Qt::white);
		bg.setAlpha(101);

		QwtText text = msg;
		text.setRenderFlags(Qt::AlignLeft);
		text.setBackgroundBrush(QBrush(bg));

		return text;
	}

	void setSite(const CSite &site) {
		m_site = site;
	}
};

CPlotWidget::CPlotWidget(QWidget *parent) :
	QwtPlot(parent), m_zoomer(NULL), m_curve(NULL), m_panner(NULL) {
	setCanvasBackground(QColor(Qt::darkGray));
	plotLayout()->setAlignCanvasToScales(true);

	setAxisTitle(yLeft, "Site Busy Channels");
	setAxisScale(yLeft, 0, 3 * 4 * 8 * 2);

	QwtPlotGrid *grid = new QwtPlotGrid();
	grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
	grid->attach(this);
}

void CPlotWidget::setPlot(const QList<quint32> &id) {
	m_id = id;

	foreach (CBSC bsc, g_bscs)
		foreach (CSite site, bsc.m_sites)
			if (m_id == site.getID()) {
				QVector<QPointF> samples(site.m_logs.count());

				for (int i = 0; i < site.m_logs.count(); ++i)
					samples[i] = QPointF(i, site.m_logs[i].m_busy);

				if (!m_curve) {
					QColor bgColor(Qt::blue);
					bgColor.setAlpha(150);

					m_curve = new QwtPlotCurve();
					m_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
					m_curve->setPen(QPen(Qt::white));
					m_curve->setBrush(bgColor);
					m_curve->attach(this);

					m_zoomer = new CPlotZoomer(canvas());
					m_zoomer->setRubberBandPen(QColor(Qt::black));
					m_zoomer->setTrackerPen(QColor(Qt::black));
					m_zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
					m_zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

					m_panner = new QwtPlotPanner(canvas());
					m_panner->setMouseButton(Qt::MidButton);
				}

				m_curve->setSamples(samples);
				m_zoomer->setSite(site);
				replot();

				return;
			}
}

void CPlotWidget::unsetPlot(const QList<quint32> &id) {
	if (m_id == id) {
		m_id.clear();
		resetPlot();
	}
}

void CPlotWidget::resetPlot() {
	if (m_curve) {
		m_curve->detach();

		delete m_panner;
		delete m_zoomer;
		delete m_curve;

		m_curve = NULL;
	}

	setAxisScale(xBottom, 0, g_config.m_limit);

	replot();
}

void CPlotWidget::updatePlot(const QList<quint32> &id) {
	if (m_id != id)
		return;

	setPlot(m_id);
}
