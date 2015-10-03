/*
 * MainFrame.h
 *
 *  Created on: Dec 6, 2009
 *      Author: jalil
 */

#ifndef MAINFRAME_H_
#define MAINFRAME_H_

#include <QtGui/QMainWindow>
#include <QtGui/QSystemTrayIcon>

class QTimer;
class QAction;
class QProgressBar;

class CMapWidget;
class CBSCWidget;
class CSiteWidget;
class CLogWidget;
class CPlotWidget;

class CSiteSpy;
class CSysTray;

class CMainFrame: public QMainWindow {
	Q_OBJECT

public:
	CMainFrame(QWidget *parent = NULL);

private:
	CMapWidget *m_mapWnd;
	CBSCWidget *m_bscWnd;
	CSiteWidget *m_siteWnd;
	CLogWidget *m_logWnd;
	CPlotWidget *m_plotWnd;

	QAction *m_configAct;
	QAction *m_refreshAct;
	QAction *m_updateAct;
	QAction *m_fullAct;
	QAction *m_animationAct;
	QAction *m_quitAct;

	QAction *m_bscAct;
	QAction *m_siteAct;
	QAction *m_logAct;
	QAction *m_plotAct;
	QAction *m_notificationAct;

	QAction *m_aboutAct;
	QAction *m_aboutQtAct;

	CSiteSpy *m_spy;
	CSysTray *m_tray;

	QTimer *m_timer;
	QProgressBar *m_progressBar;

private:
	void createMenus();
	void createActions();
	void createToolBars();
	void createStatusBar();
	void createDockWindows();
	void createConnections();

	void resetViews();
	void resetDatabase();

private slots:
	void aboutApp();
	void configTriggered();
	void updateTriggered();
	void refreshTriggered();
	void setFullScreen(bool enable);
	void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

public slots:
	void startApp();
};

#endif /* MAINFRAME_H_ */
