#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui_dialog(new Ui::Dialog)
{
    ui_dialog->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);
}

Dialog::~Dialog()
{
    delete ui_dialog;
}

void Dialog::on_pushButton_4_clicked()
{
    *acceptFlag = true;
    this->close();
}

void Dialog::on_pushButton_5_clicked()
{
    this->close();
}

void Dialog::on_closeWindow_clicked()
{
    this->close();
}

void Dialog::setMessage(const QString &_message, bool *_acceptFlag, int &ind_theme)   // Set parameters
{
    acceptFlag = _acceptFlag;
    ui_dialog->frame_hint->installEventFilter(this);
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    setTheme(ind_theme);
    QSound *sound = new QSound("/usr/share/sounds/ce_task.wav", this);
    sound->play();
    show_message(_message);
}

void Dialog::show_message(QString _message)   // Show message
{
    ui_dialog->textBrowser->clear();
    ui_dialog->textBrowser->setAlignment(Qt::AlignCenter);
    ui_dialog->textBrowser->append(_message);
}

void Dialog::setTheme(int &ind_theme)
{
    switch (ind_theme)
    {
        case 0:
        {
            ui_dialog->frame_main->setStyleSheet("background-color: rgb(5, 20, 28);");
            ui_dialog->textBrowser->setStyleSheet("color: rgb(255, 255, 255);");
        }; break;
        case 1:
        {
            ui_dialog->frame_main->setStyleSheet("background-color: rgb(3, 3, 5);");
            ui_dialog->textBrowser->setStyleSheet("color: rgb(255, 255, 255);");
        }; break;
        case 2:
        {
            ui_dialog->frame_main->setStyleSheet("background-color: rgb(39, 44, 54);");
            ui_dialog->textBrowser->setStyleSheet("color: rgb(255, 255, 255);");
        }; break;
        case 3:
        {
            ui_dialog->frame_main->setStyleSheet("background-color: rgb(220, 220, 220);");
            ui_dialog->textBrowser->setStyleSheet("color: rgb(3, 3, 5);");
        }; break;
    }
}

bool Dialog::eventFilter(QObject *watched, QEvent *event)
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

    if (watched == ui_dialog->frame_hint)
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
