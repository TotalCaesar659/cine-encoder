#include "donate.h"
#include "ui_donate.h"


Donate::Donate(QWidget *parent) :
    QDialog(parent),
    ui_donate(new Ui::Donate)
{
    ui_donate->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);
}

Donate::~Donate()
{
    delete ui_donate;
}

void Donate::on_toolButton_6_clicked()   // Close window
{
    this->close();
}

void Donate::on_closeWindow_clicked()
{
    this->close();
}

void Donate::setParameters(int &ind_theme)
{
    ui_donate->frame_hint->installEventFilter(this);
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    setTheme(ind_theme);
}

void Donate::on_toolButton_7_clicked()   // Open browser PayPal
{
    QDesktopServices::openUrl(QUrl("https://paypal.me/KozhukharenkoOleg?locale.x=ru_RU", QUrl::TolerantMode));
}

void Donate::on_toolButton_8_clicked()   // Open browser Bitcoin
{
    QDesktopServices::openUrl(QUrl("https://blockchain.com/btc/payment_request?address=14ukiWCK2f8vFNPP3qKbo2vfaSGRMN63qL&amount=0.00005448&message=Cine Encoder donation", QUrl::TolerantMode));
}

void Donate::setTheme(int &ind_theme)
{
    switch (ind_theme)
    {
        case 0:
        {
            ui_donate->frame_main->setStyleSheet("QFrame {background-color: rgb(5, 20, 28);} QLabel {color: rgb(210, 210, 210);}");
        }; break;
        case 1:
        {
            ui_donate->frame_main->setStyleSheet("QFrame {background-color: rgb(3, 3, 5);} QLabel {color: rgb(210, 210, 210);}");
        }; break;
        case 2:
        {
            ui_donate->frame_main->setStyleSheet("QFrame {background-color: rgb(39, 44, 54);} QLabel {color: rgb(210, 210, 210);}");
        }; break;
        case 3:
        {
            ui_donate->frame_main->setStyleSheet("QFrame {background-color: rgb(220, 220, 220);} QLabel {color: rgb(3, 3, 5);}");
        }; break;
    }
}

bool Donate::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->button() == Qt::LeftButton)
        {
            clickPressedFlag = false;
            return QDialog::eventFilter(watched, event);
        }
    }

    if (watched == ui_donate->frame_hint)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton)
            {
                mouseClickCoordinate = mouse_event->pos();
                clickPressedFlag = true;
                return QDialog::eventFilter(watched, event);
            }
        }
        else if ((event->type() == QEvent::MouseMove) && clickPressedFlag == true)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                this->move(mouse_event->globalPos() - mouseClickCoordinate);
                return QDialog::eventFilter(watched, event);
            }
        }
    }
    return QDialog::eventFilter(watched, event);
}

