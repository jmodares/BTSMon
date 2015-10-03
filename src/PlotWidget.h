/*
 * PlotWidget.h
 *
 *  Created on: Jan 18, 2010
 *      Author: jalil
 */

#ifndef PLOTWIDGET_H_
#define PLOTWIDGET_H_

#include <qwt_plot.h>

class CPlotZoomer;
class QwtPlotCurve;
class QwtPlotPanner;

class CPlotWidget: public QwtPlot {
	Q_OBJECT

public:
	CPlotWidget(QWidget *parent = 0);

private:
	QList<quint32> m_id;

	CPlotZoomer *m_zoomer;
	QwtPlotCurve *m_curve;
	QwtPlotPanner *m_panner;

public slots:
	void resetPlot();
	void setPlot(const QList<quint32> &id);
	void unsetPlot(const QList<quint32> &id);
	void updatePlot(const QList<quint32> &id);
};

#endif /* PLOTWIDGET_H_ */
