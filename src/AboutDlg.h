#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <QDialog>

class QTimer;
class QTimeLine;

namespace Ui {
    class aboutDlg;
}

class CAboutDlg : public QDialog {
    Q_OBJECT
public:
    CAboutDlg(QWidget *parent = 0);
    ~CAboutDlg();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::aboutDlg *ui;

private:
        enum {
                ANIMATION_INTERVAL = 10, ANIMATION_DURATION = 1
        };

        QTimer *m_timer;
        QTimeLine *m_timeLine;

private Q_SLOTS:
        void linkClicked(const QString &url);
        void updateLogo(int frame);
};

#endif // ABOUTDLG_H
