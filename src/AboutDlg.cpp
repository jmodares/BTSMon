#include "AboutDlg.h"
#include "ui_AboutDlg.h"

#include <QDesktopServices>

#include "Icon.h"

CAboutDlg::CAboutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDlg)
{
    ui->setupUi(this);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(ANIMATION_INTERVAL * 1000);
    m_timer->start();

    m_timeLine = new QTimeLine(ANIMATION_DURATION * 1000, this);
    m_timeLine->setFrameRange(ICON_PERSIAL_0, ICON_PERSIAL_5 + 2);
    m_timeLine->setUpdateInterval(0);

    connect(m_timeLine, SIGNAL(frameChanged(int)), this, SLOT(updateLogo(int)));
    connect(m_timeLine, SIGNAL(finished()), m_timer, SLOT(start()));
    connect(m_timer, SIGNAL(timeout()), m_timeLine, SLOT(start()));
}

CAboutDlg::~CAboutDlg()
{
    delete ui;
}

void CAboutDlg::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CAboutDlg::linkClicked(const QString &url) {
        QDesktopServices::openUrl(QUrl(url));
}

void CAboutDlg::updateLogo(int frame) {
        if (frame < ICON_PERSIAL_5 + 2)
                labelLogo->setPixmap(g_icons[(frame % 6) + ICON_PERSIAL_0]);
}
