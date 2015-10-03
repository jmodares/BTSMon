/*
 * MainFrame.cpp
 *
 *  Created on: Dec 6, 2009
 *      Author: jalil
 */

#include <QtGui/QtGui>

#include "Site.h"
#include "Config.h"
#include "SiteSpy.h"
#include "SysTray.h"
#include "MapWidget.h"
#include "BSCWidget.h"
#include "SiteWidget.h"
#include "LogWidget.h"
#include "PlotWidget.h"
#include "AboutDlg.h"
#include "ConfigDlg.h"
#include "MainFrame.h"

CMainFrame::CMainFrame(QWidget *parent) :
	QMainWindow(parent) {
	m_spy = new CSiteSpy(this);
	m_tray = new CSysTray(this);
	m_mapWnd = new CMapWidget(this);

	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);

	createDockWindows();

	setWindowTitle(tr("SiteMon, Monitoring Systems by Persial"));
	setWindowIcon(QIcon(":/images/Persial0.png"));
	setCentralWidget(m_mapWnd);

	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	createConnections();
}

void CMainFrame::createActions() {
	m_configAct = new QAction(QIcon(":/images/Config.png"), tr("&Config..."), this);
	m_configAct->setStatusTip(tr("Set the user account, BSC and check interval"));
	m_configAct->setToolTip(tr("Set Configuration"));

	m_refreshAct = new QAction(QIcon(":/images/Refresh.png"), tr("&Refresh"), this);
	m_refreshAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
	m_refreshAct->setStatusTip(tr("Refresh the capacity of sites"));
	m_refreshAct->setToolTip(tr("Refresh Sites Capacity - Ctrl + R"));

	m_updateAct = new QAction(this);
	m_updateAct->setText("Start");

	m_fullAct = new QAction(QIcon(":/images/Full.png"), tr("&Full Screen"), this);
	m_fullAct->setStatusTip(tr("Enable application to occupy whole screen"));
	m_fullAct->setToolTip(tr("Show full screen"));
	m_fullAct->setCheckable(true);
	m_fullAct->setChecked(true);

	m_animationAct = new QAction(QIcon(":/images/Animation.png"), tr("&Animation"), this);
	m_animationAct->setStatusTip(tr("Enable animation on site traveling"));
	m_animationAct->setToolTip(tr("Enable Animation"));
	m_animationAct->setCheckable(true);
	m_animationAct->setChecked(false);

	m_quitAct = new QAction(tr("&Quit"), this);
	m_quitAct->setShortcuts(QKeySequence::Quit);
	m_quitAct->setStatusTip(tr("Exit from SiteMon!"));

	m_bscAct->setText(tr("&BSCs"));
	m_bscAct->setIcon(QIcon(":/images/BSCs.png"));
	m_bscAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
	m_bscAct->setStatusTip(tr("Show all BSCs monitored by SiteMon"));
	m_bscAct->setToolTip(tr("Show BSCs - Ctrl + B"));

	m_siteAct->setText(tr("&Sites"));
	m_siteAct->setIcon(QIcon(":/images/Sites.png"));
	m_siteAct->setStatusTip(tr("Show all sites included with the BSC"));
	m_siteAct->setToolTip(tr("Show Sites"));

	m_logAct->setText(tr("&Logs"));
	m_logAct->setIcon(QIcon(":/images/Logs.png"));
	m_logAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
	m_logAct->setStatusTip(tr("Show all logs for the selected site"));
	m_logAct->setToolTip(tr("Show Logs - Ctrl + L"));

	m_plotAct->setText(tr("&Plot"));
	m_plotAct->setIcon(QIcon(":/images/Plot.png"));
	m_plotAct->setStatusTip(tr("Show channels which were busy on a plot"));
	m_plotAct->setToolTip(tr("Show Busy Plot"));

	m_notificationAct = new QAction(QIcon(":/images/Notification.png"), tr("&Notification"), this);
	m_notificationAct->setStatusTip(tr("Enable system notification for tips and warnings"));
	m_notificationAct->setToolTip(tr("Enable System Notification"));
	m_notificationAct->setCheckable(true);
	m_notificationAct->setChecked(false);

	m_aboutAct = new QAction(QIcon(":/images/About.png"), tr("About Site&Mon"), this);
	m_aboutAct->setStatusTip(tr("Information about SiteMon, a member of Monitoring Systems by Persial"));
	m_aboutAct->setToolTip(tr("About SiteMon"));

	m_aboutQtAct = new QAction(tr("About &Qt"), this);
	m_aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void CMainFrame::createMenus() {
	QMenu *menu;

	menu = menuBar()->addMenu(tr("&File"));
	menu->addAction(m_configAct);
	menu->addSeparator();
	menu->addAction(m_refreshAct);
	menu->addAction(m_updateAct);
	menu->addSeparator();
	menu->addAction(m_fullAct);
	menu->addAction(m_animationAct);
	menu->addSeparator();
	menu->addAction(m_quitAct);

	menu = menuBar()->addMenu(tr("&View"));
	menu->addAction(m_bscAct);
	menu->addAction(m_siteAct);
	menu->addAction(m_logAct);
	menu->addAction(m_plotAct);
	menu->addSeparator();
	menu->addAction(m_notificationAct);

	menuBar()->addSeparator();

	menu = menuBar()->addMenu(tr("&Help"));
	menu->addAction(m_aboutAct);
	menu->addSeparator();
	menu->addAction(m_aboutQtAct);

	menu = new QMenu(this);
	menu->addAction(m_configAct);
	menu->addSeparator();
	menu->addAction(m_refreshAct);
	menu->addAction(m_updateAct);
	menu->addSeparator();
	menu->addAction(m_aboutAct);
	menu->addSeparator();
	menu->addAction(m_quitAct);

	m_tray->setContextMenu(menu);
}

void CMainFrame::createToolBars() {
	QToolBar *toolBar;

	toolBar = addToolBar(tr("File"));
	toolBar->addAction(m_configAct);
	toolBar->addSeparator();
	toolBar->addAction(m_refreshAct);
	toolBar->addAction(m_updateAct);
	toolBar->addSeparator();
	toolBar->addAction(m_fullAct);
	toolBar->addAction(m_animationAct);

	toolBar = addToolBar(tr("View"));
	toolBar->addAction(m_bscAct);
	toolBar->addAction(m_siteAct);
	toolBar->addAction(m_logAct);
	toolBar->addAction(m_plotAct);
	toolBar->addSeparator();
	toolBar->addAction(m_notificationAct);

	toolBar = addToolBar(tr("Help"));
	toolBar->addAction(m_aboutAct);
}

void CMainFrame::createStatusBar() {
	m_progressBar = new QProgressBar;
	m_progressBar->setMaximumSize(170, 17);
	m_progressBar->setMaximum(0);

	statusBar()->addPermanentWidget(m_progressBar);
}

void CMainFrame::createDockWindows() {
	QDockWidget *dockWnd;

	dockWnd = new QDockWidget(tr("BSCs"), this);
	dockWnd->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	m_bscWnd = new CBSCWidget(dockWnd);
	dockWnd->setWidget(m_bscWnd);
	addDockWidget(Qt::RightDockWidgetArea, dockWnd);
	m_bscAct = dockWnd->toggleViewAction();

	dockWnd = new QDockWidget(tr("Sites"), this);
	dockWnd->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	m_siteWnd = new CSiteWidget(dockWnd);
	dockWnd->setWidget(m_siteWnd);
	addDockWidget(Qt::LeftDockWidgetArea, dockWnd);
	m_siteAct = dockWnd->toggleViewAction();

	dockWnd = new QDockWidget(tr("Logs"), this);
	dockWnd->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	m_logWnd = new CLogWidget(dockWnd);
	dockWnd->setWidget(m_logWnd);
	addDockWidget(Qt::BottomDockWidgetArea, dockWnd);
	m_logAct = dockWnd->toggleViewAction();

	dockWnd = new QDockWidget(tr("Plot"), this);
	dockWnd->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	m_plotWnd = new CPlotWidget(dockWnd);
	dockWnd->setWidget(m_plotWnd);
	addDockWidget(Qt::BottomDockWidgetArea, dockWnd);
	m_plotAct = dockWnd->toggleViewAction();
}

void CMainFrame::createConnections() {
	connect(m_spy, SIGNAL(zeeiChecked(QString)), m_mapWnd, SLOT(update()));
	connect(m_spy, SIGNAL(zeolChecked(QString)), m_mapWnd, SLOT(update()));
	connect(m_spy, SIGNAL(zeroChecked(QList<quint32>)), m_mapWnd, SLOT(update()));
	connect(m_spy, SIGNAL(zeeiChecked(QString)), m_bscWnd, SLOT(updateBSC(QString)));
	connect(m_spy, SIGNAL(zeolChecked(QString)), m_bscWnd, SLOT(updateBSC(QString)));
	connect(m_spy, SIGNAL(bscChecked(QString)), m_bscWnd, SLOT(updateBSC(QString)));
	connect(m_spy, SIGNAL(zeeiChecked(QString)), m_siteWnd, SLOT(updateZEEI(QString)));
	connect(m_spy, SIGNAL(zeolChecked(QString)), m_siteWnd, SLOT(updateZEOL(QString)));
	connect(m_spy, SIGNAL(zeroChecked(QList<quint32>)), m_siteWnd, SLOT(updateZERO(QList<quint32>)));
	connect(m_spy, SIGNAL(siteLogUpdated(QList<quint32>)), m_logWnd, SLOT(updateLog(QList<quint32>)));
	connect(m_spy, SIGNAL(siteLogUpdated(QList<quint32>)), m_plotWnd, SLOT(updatePlot(QList<quint32>)));
	connect(m_spy, SIGNAL(checkFinished()), m_updateAct, SLOT(trigger()));
	connect(m_spy, SIGNAL(checkFinished()), m_tray, SLOT(updateMSG()));

	connect(m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

	connect(m_timer, SIGNAL(timeout()), m_updateAct, SLOT(trigger()));

	connect(m_mapWnd, SIGNAL(siteSelected(QList<quint32>)), m_siteWnd, SLOT(selectSite(QList<quint32>)));
	connect(m_mapWnd, SIGNAL(mouseMoveGeoPosition(QString)), statusBar(), SLOT(showMessage(QString)));

	connect(m_bscWnd, SIGNAL(bscSelected(QString)), m_siteWnd, SLOT(selectSite(QString)));

	connect(m_siteWnd, SIGNAL(siteSelected(QList<quint32>)), m_mapWnd, SLOT(centerSite(QList<quint32>)));
	connect(m_siteWnd, SIGNAL(siteSelected(QList<quint32>)), m_bscWnd, SLOT(selectBSC(QList<quint32>)));
	connect(m_siteWnd, SIGNAL(siteSelected(QList<quint32>)), m_logWnd, SLOT(setLog(QList<quint32>)));
	connect(m_siteWnd, SIGNAL(siteSelected(QList<quint32>)), m_plotWnd, SLOT(setPlot(QList<quint32>)));
	connect(m_siteWnd, SIGNAL(siteDeleted(QList<quint32>)), m_logWnd, SLOT(unsetLog(QList<quint32>)));
	connect(m_siteWnd, SIGNAL(siteDeleted(QList<quint32>)), m_plotWnd, SLOT(unsetPlot(QList<quint32>)));

	connect(m_configAct, SIGNAL(triggered()), this, SLOT(configTriggered()));
	connect(m_refreshAct, SIGNAL(triggered()), this, SLOT(refreshTriggered()));
	connect(m_updateAct, SIGNAL(triggered()), this, SLOT(updateTriggered()));
	connect(m_fullAct, SIGNAL(toggled(bool)), this, SLOT(setFullScreen(bool)));
	connect(m_animationAct, SIGNAL(toggled(bool)), m_mapWnd, SLOT(goAnimation(bool)));
	connect(m_quitAct, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(m_notificationAct, SIGNAL(toggled(bool)), m_tray, SLOT(setVisible(bool)));
	connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(aboutApp()));
	connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void CMainFrame::resetViews() {
	m_mapWnd->update();
	m_bscWnd->resetList();
	m_siteWnd->resetList();
	m_logWnd->clear();
	m_plotWnd->resetPlot();
}

void CMainFrame::startApp() {
	g_config.load();
	resetDatabase();
	resetViews();

	m_timer->setInterval(g_config.m_interval * 60 * 1000);

	if (!g_config.m_save)
		QTimer::singleShot(0, m_configAct, SLOT(trigger()));

	m_updateAct->trigger();

	m_fullAct->setChecked(false);
	m_animationAct->setChecked(true);

	if (QSystemTrayIcon::isSystemTrayAvailable())
		m_notificationAct->setChecked(true);
	else
		m_notificationAct->setEnabled(false);
}

void CMainFrame::trayIconActivated(QSystemTrayIcon::ActivationReason reason) {
	if (reason == QSystemTrayIcon::Trigger) {
		if (isVisible()) {
			m_fullAct->setChecked(true);
			hide();
		} else
			m_fullAct->setChecked(false);
	}
}

void CMainFrame::configTriggered() {
	CConfigDlg dlg(this);

	if (dlg.exec()) {
		resetDatabase();
		resetViews();

		m_timer->setInterval(g_config.m_interval * 60 * 1000);

		m_updateAct->setText("Stop");
		m_updateAct->trigger();
		m_updateAct->trigger();
	}
}

void CMainFrame::refreshTriggered() {
	if (QMessageBox::Ok == QMessageBox::question(this, tr("Refreshing Sites Caps"), tr("Are you sure to recalculate all sites capacities?"), QMessageBox::Ok | QMessageBox::Cancel)) {
		QMutableListIterator<CBSC> iBSC(g_bscs);
		while (iBSC.hasNext()) {
			QMutableListIterator<CSite> iSite(iBSC.next().m_sites);
			while (iSite.hasNext())
				iSite.next().m_caps.clear();
		}

		resetViews();

		m_updateAct->setText("Stop");
		m_updateAct->trigger();
		m_updateAct->trigger();
	}
}

void CMainFrame::updateTriggered() {
	if (m_updateAct->text().contains("Start")) {
		m_updateAct->setText("Stop");
		m_updateAct->setIcon(QIcon(":/images/Stop.png"));
		m_updateAct->setToolTip(tr("Stop Check"));
		m_updateAct->setStatusTip(tr("Stop checking"));

		m_spy->startCheck();
		m_tray->startAnimation();
		m_timer->stop();
		m_progressBar->show();

		return;
	}

	m_updateAct->setText("Start");
	m_updateAct->setIcon(QIcon(":/images/Start.png"));
	m_updateAct->setToolTip(tr("Start Check"));
	m_updateAct->setStatusTip(tr("Start checking the state of sites and their sectors"));

	m_spy->stopCheck();
	m_tray->stopAnimation();
	m_timer->start();
	m_progressBar->hide();
}

void CMainFrame::setFullScreen(bool enable) {
	if (enable)
		showFullScreen();
	else
		showMaximized();

	raise();
	activateWindow();
}

void CMainFrame::aboutApp() {
	CAboutDlg dlg(this);
	dlg.exec();
}

void CMainFrame::resetDatabase() {
	QMutableListIterator<CBSC> i(g_bscs);
	while (i.hasNext()) {
		CBSC &bsc = i.next();
		bool found = false;

		foreach (QString ip, g_config.m_ips)
			if (ip == bsc.m_ip) {
				found = true;
				break;
			}

		if (!found)
			i.remove();
	}

	foreach (QString ip, g_config.m_ips) {
		bool found = false;

		foreach (CBSC bsc, g_bscs)
			if (bsc.m_ip == ip) {
				found = true;
				break;
			}

		if (!found)
			g_bscs << CBSC(ip);
	}

	g_locs.clear();

	if (!g_config.m_file.isEmpty()) {
		QFile file(g_config.m_file);

		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream gisIn(&file);

			while (!gisIn.atEnd()) {
				QStringList line = gisIn.readLine().split('\t');
				g_locs << CGIS(QList<quint32> () << line[7].toUInt() << line[8].toUInt(), QList<qreal> () << line[3].toDouble() << line[4].toDouble());
			}
		}
	}

	QMutableListIterator<CBSC> iBSC(g_bscs);
	while (iBSC.hasNext()) {
		QMutableListIterator<CSite> iSite(iBSC.next().m_sites);
		while (iSite.hasNext()) {
			CSite &site = iSite.next();
			QList<quint32> id = site.getID();

			site.m_loc.clear();
			foreach (CGIS gis, g_locs)
				if (gis.m_id == id) {
					site.m_loc = gis.m_loc;
					break;
				}

			while (site.m_logs.count() > g_config.m_limit)
				site.m_logs.removeFirst();
		}
	}
}
