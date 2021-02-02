#include "about.h"
#include "ui_about.h"


About::About(QWidget *parent) :
    QDialog(parent),
    ui_about(new Ui::About)
{
    ui_about->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);
}

About::~About()
{
    delete ui_about;
}

void About::on_toolButton_6_clicked()   // Close window
{
    this->close();
}

void About::on_closeWindow_clicked()
{
    this->close();
}

//void About::showEvent(QShowEvent *event)    // Call set parameters
//{   if _windowActivated == false;
//    setParameters();
//    //setTheme(ind_theme);
//}

void About::setParameters(int &ind_theme) // Set parameters
{
    ui_about->frame_hint->installEventFilter(this);
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    setTheme(ind_theme);
    ui_about->label_11->setText("<a href=\"https://github.com/CineEncoder/cine-encoder\">https://github.com/CineEncoder/cine-encoder</a>");
    ui_about->label_11->setOpenExternalLinks(true);
    ui_about->label_12->setText("<a href=\"https://github.com/CineEncoder/cine-encoder/blob/master/LICENSE\">License: GNU Lesser General Public License Version 3 </a>");
    ui_about->label_12->setOpenExternalLinks(true);
}

void About::setTheme(int &ind_theme)
{
    switch (ind_theme)
    {
        case 0:
        {
            ui_about->frame_main->setStyleSheet("QFrame {background-color: rgb(5, 20, 28);} QLabel {color: rgb(210, 210, 210);}");
        }; break;
        case 1:
        {
            ui_about->frame_main->setStyleSheet("QFrame {background-color: rgb(3, 3, 5);} QLabel {color: rgb(210, 210, 210);}");
        }; break;
        case 2:
        {
            ui_about->frame_main->setStyleSheet("QFrame {background-color: rgb(39, 44, 54);} QLabel {color: rgb(210, 210, 210);}");
        }; break;
        case 3:
        {
            ui_about->frame_main->setStyleSheet("QFrame {background-color: rgb(220, 220, 220);} QLabel {color: rgb(3, 3, 5);}");
        }; break;
    }
}

bool About::eventFilter(QObject *watched, QEvent *event)
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

    if (watched == ui_about->frame_hint)
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
