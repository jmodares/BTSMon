//============================================================================
// Name        : SiteMon.cpp
// Author      : Jalil Modares
// Version     :
// Copyright   : Copyright 2010 Persial
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <QtGui/QtGui>

#include "MainFrame.h"

QList<QPixmap> g_icons;

void loadIcons() {
	g_icons << QPixmap(":/images/Persial0.png");
	g_icons << QPixmap(":/images/Persial1.png");
	g_icons << QPixmap(":/images/Persial2.png");
	g_icons << QPixmap(":/images/Persial3.png");
	g_icons << QPixmap(":/images/Persial4.png");
	g_icons << QPixmap(":/images/Persial5.png");

	g_icons << QPixmap(":/images/SiteBlue.png");
	g_icons << QPixmap(":/images/SiteGraphite.png");
	g_icons << QPixmap(":/images/SiteOrange.png");
	g_icons << QPixmap(":/images/SitePurple.png");

	g_icons << QPixmap(":/images/BTSBlue.png");
	g_icons << QPixmap(":/images/BTSGreen.png");
	g_icons << QPixmap(":/images/BTSLilac.png");
	g_icons << QPixmap(":/images/BTSOrange.png");

	g_icons << QPixmap(":/images/Alarm.png");
}

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	qApp->setQuitOnLastWindowClosed(false);

	QSplashScreen splash(QPixmap(":/images/Splash.png"));
	splash.show();

	splash.showMessage(QObject::tr("Loading Icons ..."));
	loadIcons();

	splash.showMessage(QObject::tr("Loading Modules ..."));
	CMainFrame mainWnd;

	mainWnd.startApp();
	splash.finish(&mainWnd);

	return qApp->exec();
}
