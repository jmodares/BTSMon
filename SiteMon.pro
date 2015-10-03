# #####################################################################
# Automatically generated by qmake (2.01a) Sat May 29 10:37:04 2010
# #####################################################################
TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += AboutDlg.h \
    BSCWidget.h \
    Config.h \
    ConfigDlg.h \
    Icon.h \
    LogWidget.h \
    MainFrame.h \
    MapWidget.h \
    PlotWidget.h \
    Site.h \
    SiteSpy.h \
    SiteWidget.h \
    SysTray.h
FORMS += AboutDlg.ui \
    Config.ui
SOURCES += AboutDlg.cpp \
    BSCWidget.cpp \
    Config.cpp \
    ConfigDlg.cpp \
    LogWidget.cpp \
    MainFrame.cpp \
    MapWidget.cpp \
    PlotWidget.cpp \
    Site.cpp \
    SiteMon.cpp \
    SiteSpy.cpp \
    SiteWidget.cpp \
    SysTray.cpp
RESOURCES += resource.qrc

INCLUDEPATH += /home/jalil/Public/marble/include
INCLUDEPATH += /home/jalil/Public/qwt/include
INCLUDEPATH += /home/jalil/Public/qttelnet/include

LIBS += -L/home/jalil/Public/marble/lib -lmarblewidget
LIBS += -L/home/jalil/Public/qwt/lib -lqwt
LIBS += -L/home/jalil/Public/qttelnet/lib -lQtSolutions_Telnet-2.1