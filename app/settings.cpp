#include "settings.h"
#include "ui_settings.h"
#include "taskcomplete.h"



Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui_settings(new Ui::Settings)
{
    ui_settings->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);
}

Settings::~Settings()
{
    delete ui_settings;
}

void Settings::on_closeWindow_clicked() // Close settings window
{
    this->close();
}

void Settings::on_pushButton_7_clicked() // Close settings window
{
    this->close();
}

void Settings::on_expandWindow_clicked()
{
    if (_expandWindowsState == false)
    {
        this->showMaximized();
        _expandWindowsState = true;
        QIcon iconShowMax;
        iconShowMax.addFile(QString::fromUtf8(":/16x16/icons/16x16/cil-clone.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui_settings->expandWindow->setIcon(iconShowMax);
    } else {
        this->showNormal();
        _expandWindowsState = false;
        QIcon iconShowNorm;
        iconShowNorm.addFile(QString::fromUtf8(":/16x16/icons/16x16/cil-media-stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui_settings->expandWindow->setIcon(iconShowNorm);
    }
}

void Settings::on_hideWindow_clicked()
{
    this->showMinimized();
}

void Settings::closeEvent(QCloseEvent *close_settings)  // Show prompt when close app
{
    close_settings->ignore();
    if (_flag_save == false)
    {
        *_ptr_output_folder = _curr_output_folder;
        *_ptr_temp_folder = _curr_temp_folder;
        *_ptr_batch_mode = _curr_batch_mode;
        *_ptr_showHDR_mode = _curr_showHDR_mode;
        *_ptr_protection = _curr_protection;
        *_ptr_timer_interval = _curr_timer_interval;
        *_ptr_theme = _curr_theme;
    };
    close_settings->accept();
}

void Settings::setParameters(bool *ptr_batch_mode, QFile *ptr_stn_file,
                             QString *ptr_output_folder, QString *ptr_temp_folder,
                             bool *ptr_protection, bool *ptr_showHDR_mode, int *ptr_timer_interval, int *ptr_theme)  // Set parameters
{
    ui_settings->frame_hint->installEventFilter(this);
    ui_settings->widget->installEventFilter(this);
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    _ptr_batch_mode = ptr_batch_mode;
    _ptr_showHDR_mode = ptr_showHDR_mode;
    _ptr_stn_file = ptr_stn_file;
    _ptr_output_folder = ptr_output_folder;
    _ptr_temp_folder = ptr_temp_folder;
    _ptr_protection = ptr_protection;
    _ptr_timer_interval = ptr_timer_interval;
    _ptr_theme = ptr_theme;

    setTheme(*_ptr_theme);
    ui_settings->lineEdit_9->setText(*_ptr_temp_folder);
    ui_settings->lineEdit_10->setText(*_ptr_output_folder);
    ui_settings->spinBox_3->setValue(*_ptr_timer_interval);
    if (*_ptr_batch_mode == true)
    {
        ui_settings->checkBox_1->setChecked(true);
    }
    if (*_ptr_showHDR_mode == true)
    {
        ui_settings->checkBox_2->setChecked(true);
    }
    if (*_ptr_protection == true)
    {
        ui_settings->checkBox_3->setChecked(true);
        ui_settings->spinBox_3->setEnabled(true);
    }
    ui_settings->comboBox_1->setCurrentIndex(*_ptr_theme);
    _curr_output_folder = *_ptr_output_folder;
    _curr_temp_folder = *_ptr_temp_folder;
    _curr_batch_mode = *_ptr_batch_mode;
    _curr_showHDR_mode = *_ptr_showHDR_mode;
    _curr_protection = *_ptr_protection;
    _curr_timer_interval = *_ptr_timer_interval;
    _curr_theme = *_ptr_theme;
    _flag_save = false;
}

void Settings::on_buttonReset_clicked() // Reset settings
{
    ui_settings->lineEdit_9->clear();
    ui_settings->lineEdit_10->clear();
    ui_settings->checkBox_1->setChecked(false);
    ui_settings->checkBox_2->setChecked(false);
    ui_settings->checkBox_3->setChecked(false);
    ui_settings->spinBox_3->setEnabled(false);
    ui_settings->comboBox_1->setCurrentIndex(0);
    *_ptr_temp_folder = "";
    *_ptr_output_folder = "";
    *_ptr_protection = false;
    *_ptr_showHDR_mode = false;
    *_ptr_batch_mode = false;
    *_ptr_theme = 0;
}

void Settings::on_pushButton_6_clicked() // Save settings
{
    if ((*_ptr_stn_file).open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString line_0 = "temp_folder:" + *_ptr_temp_folder + "\n";
        (*_ptr_stn_file).write(line_0.toUtf8());
        QString line_1 = "output_folder:" + *_ptr_output_folder + "\n";
        (*_ptr_stn_file).write(line_1.toUtf8());
        if (*_ptr_batch_mode == true)
        {
            (*_ptr_stn_file).write("batch_mode:true\n");
        }
        else
        {
            (*_ptr_stn_file).write("batch_mode:false\n");
        }
        if (*_ptr_showHDR_mode == true)
        {
            (*_ptr_stn_file).write("show_hdr:true\n");
        }
        else
        {
            (*_ptr_stn_file).write("show_hdr:false\n");
        }
        if (*_ptr_protection == true)
        {
            (*_ptr_stn_file).write("protection:true\n");
        }
        else
        {
            (*_ptr_stn_file).write("protection:false\n");
        }
        *_ptr_timer_interval = ui_settings->spinBox_3->value();
        QString line_2 = "timer_interval:" + QString::number(*_ptr_timer_interval) + "\n";
        (*_ptr_stn_file).write(line_2.toUtf8());
        *_ptr_theme = ui_settings->comboBox_1->currentIndex();
        QString line_3 = "theme:" + QString::number(*_ptr_theme) + "\n";
        (*_ptr_stn_file).write(line_3.toUtf8());
        (*_ptr_stn_file).close();
        _flag_save = true;
        this->close();
    }
    else
    {
        _message = "Settings file not found!\n";
        call_task_complete(_message, false);
    };
}

void Settings::on_pushButton_5_clicked() // Select temp folder
{
    /*QFileDialog *selectFolderWindow = new QFileDialog(this);
    selectFolderWindow->setOptions(QFileDialog::DontUseNativeDialog | QFileDialog::DontResolveSymlinks);
    //selectFolderWindow->setOptions(QFileDialog::DontResolveSymlinks);
    selectFolderWindow->setWindowFlags(Qt::Dialog | Qt::SubWindow);
    selectFolderWindow->setFileMode(QFileDialog::Directory);
    selectFolderWindow->setDirectory(QDir::homePath());
    selectFolderWindow->setMinimumWidth(600);
    selectFolderWindow->setWindowTitle("Select temp folder");
    selectFolderWindow->exec();
    int res = selectFolderWindow->result();
    QDir temp_folder_name_dir;
    temp_folder_name_dir = selectFolderWindow->directory();
    delete selectFolderWindow;
    if (res == 0) {
        return;
    }
    QString temp_folder_name = temp_folder_name_dir.dirName();*/
    QString temp_folder_name = QFileDialog::getExistingDirectory(this, tr("Select temp folder"), QDir::currentPath());
    if (temp_folder_name.isEmpty())
    {
        return;
    }
    ui_settings->lineEdit_9->setText(temp_folder_name);
    *_ptr_temp_folder = temp_folder_name;
}

void Settings::on_pushButton_4_clicked()  // Select output folder
{
    QString output_folder_name = QFileDialog::getExistingDirectory(this, tr("Select output folder"), QDir::currentPath());
    if (output_folder_name.isEmpty())
    {
        return;
    }
    ui_settings->lineEdit_10->setText(output_folder_name);
    *_ptr_output_folder = output_folder_name;
}

void Settings::on_checkBox_1_clicked()  // Batch mode select
{
    int stts_1 = ui_settings->checkBox_1->checkState();
    if (stts_1 == 2)
    {
        *_ptr_batch_mode = true;
    }
    else
    {
        *_ptr_batch_mode = false;
    }
}

void Settings::on_checkBox_2_clicked()  // Show HDR info select
{
    int stts_2 = ui_settings->checkBox_2->checkState();
    if (stts_2 == 2)
    {
        *_ptr_showHDR_mode = true;
    }
    else
    {
        *_ptr_showHDR_mode = false;
    }
}

void Settings::on_checkBox_3_clicked()  // Protection mode select
{
    int stts_3 = ui_settings->checkBox_3->checkState();
    if (stts_3 == 2)
    {
        *_ptr_protection = true;
        ui_settings->spinBox_3->setEnabled(true);
    }
    else
    {
        *_ptr_protection = false;
        ui_settings->spinBox_3->setEnabled(false);
    };
}

void Settings::call_task_complete(const QString &_message, const bool &_timer_mode)   // Call task complete
{
    Taskcomplete taskcomplete(this);
    taskcomplete.setMessage(_message, _timer_mode, *_ptr_theme);
    taskcomplete.setModal(true);
    taskcomplete.exec();
}

void Settings::setTheme(int &ind_theme)   // Set theme
{
    switch (ind_theme)
    {
        case 0:
        {
            ui_settings->frame_main->setStyleSheet("QFrame {background-color: rgb(5, 20, 28);} QLineEdit {color: rgb(98, 103, 111); "
                                                   "background-color: rgb(27, 29, 35); border-radius: 0px; border: 2px solid rgb(27, 29, 35); "
                                                   "padding-left: 10px;} QLineEdit:hover {border: 2px solid rgb(64, 71, 88);} "
                                                   "QLineEdit:focus {border: 2px solid rgb(91, 101, 124);} QCheckBox {spacing: 12px; "
                                                   "color: rgb(255, 255, 255); background-color: transparent;} QCheckBox::indicator {border: "
                                                   "2px solid #5A5A5A; background: none; width: 12px; height: 12px;} QCheckBox::indicator:checked "
                                                   "{image: url(:/16x16/icons/16x16/cil-check-alt.png); border: 2px solid #5A5A5A; background: none; "
                                                   "width: 12px; height: 12px;}");
            ui_settings->frame->setStyleSheet("background-color: rgb(5, 20, 28);");
            ui_settings->label_temp->setStyleSheet("QLabel {color: rgb(255, 255, 255);}");
            ui_settings->label_output->setStyleSheet("QLabel {color: rgb(255, 255, 255);}");
            ui_settings->label_sec->setStyleSheet("QLabel {color: rgb(255, 255, 255);}");
            ui_settings->label_theme->setStyleSheet("QLabel {color: rgb(255, 255, 255);}");
            ui_settings->checkBox_1->setStyleSheet("color: rgb(255, 255, 255);");
            ui_settings->checkBox_3->setStyleSheet("color: rgb(255, 255, 255);");
            ui_settings->frame_3->setStyleSheet("background-color: rgba(27, 29, 35, 20);");
        }; break;
        case 1:
        {
            ui_settings->frame_main->setStyleSheet("QFrame {background-color: rgb(3, 3, 5);} QLineEdit {color: rgb(98, 103, 111); "
                                                   "background-color: rgb(27, 29, 35); border-radius: 0px; border: 2px solid rgb(27, 29, 35); "
                                                   "padding-left: 10px;} QLineEdit:hover {border: 2px solid rgb(64, 71, 88);} "
                                                   "QLineEdit:focus {border: 2px solid rgb(91, 101, 124);} QCheckBox {spacing: 12px; "
                                                   "color: rgb(255, 255, 255); background-color: transparent;} QCheckBox::indicator {border: "
                                                   "2px solid #5A5A5A; background: none; width: 12px; height: 12px;} QCheckBox::indicator:checked "
                                                   "{image: url(:/16x16/icons/16x16/cil-check-alt.png); border: 2px solid #5A5A5A; background: none; "
                                                   "width: 12px; height: 12px;}");
            ui_settings->frame->setStyleSheet("background-color: rgb(3, 3, 5);");
            ui_settings->label_temp->setStyleSheet("QLabel {color: rgb(255, 255, 255);}");
            ui_settings->label_output->setStyleSheet("QLabel {color: rgb(255, 255, 255);}");
            ui_settings->label_sec->setStyleSheet("QLabel {color: rgb(255, 255, 255);}");
            ui_settings->label_theme->setStyleSheet("QLabel {color: rgb(255, 255, 255);}");
            ui_settings->checkBox_1->setStyleSheet("color: rgb(255, 255, 255);");
            ui_settings->checkBox_3->setStyleSheet("color: rgb(255, 255, 255);");
            ui_settings->frame_3->setStyleSheet("background-color: rgba(27, 29, 35, 20); border-top: 1px solid rgba(100, 100, 168, 50);");
        }; break;
        case 2:
        {
            ui_settings->frame_main->setStyleSheet("QFrame {background-color: rgb(39, 44, 54);} QLineEdit {color: rgb(98, 103, 111); "
                                                   "background-color: rgb(27, 29, 35); border-radius: 0px; border: 2px solid rgb(27, 29, 35); "
                                                   "padding-left: 10px;} QLineEdit:hover {border: 2px solid rgb(64, 71, 88);} "
                                                   "QLineEdit:focus {border: 2px solid rgb(91, 101, 124);} QCheckBox {spacing: 12px; "
                                                   "color: rgb(255, 255, 255); background-color: transparent;} QCheckBox::indicator {border: "
                                                   "2px solid #5A5A5A; background: none; width: 12px; height: 12px;} QCheckBox::indicator:checked "
                                                   "{image: url(:/16x16/icons/16x16/cil-check-alt.png); border: 2px solid #5A5A5A; background: none; "
                                                   "width: 12px; height: 12px;}");
            ui_settings->frame->setStyleSheet("background-color: rgb(39, 44, 54);");
            ui_settings->label_temp->setStyleSheet("QLabel {color: rgb(255, 255, 255);}");
            ui_settings->label_output->setStyleSheet("QLabel {color: rgb(255, 255, 255);}");
            ui_settings->label_sec->setStyleSheet("QLabel {color: rgb(255, 255, 255);}");
            ui_settings->label_theme->setStyleSheet("QLabel {color: rgb(255, 255, 255);}");
            ui_settings->checkBox_1->setStyleSheet("color: rgb(255, 255, 255);");
            ui_settings->checkBox_3->setStyleSheet("color: rgb(255, 255, 255);");
            ui_settings->frame_3->setStyleSheet("background-color: rgba(27, 29, 35, 150); border-top: 1px solid rgba(100, 100, 168, 50);");
        }; break;
        case 3:
        {
            ui_settings->frame_main->setStyleSheet("QFrame {background-color: rgb(220, 220, 220);} QLineEdit {color: rgb(98, 103, 111); "
                                                   "background-color: rgb(27, 29, 35); border-radius: 0px; border: 2px solid rgb(27, 29, 35); "
                                                   "padding-left: 10px;} QLineEdit:hover {border: 2px solid rgb(64, 71, 88);} "
                                                   "QLineEdit:focus {border: 2px solid rgb(91, 101, 124);} QCheckBox {spacing: 12px; "
                                                   "color: rgb(255, 255, 255); background-color: transparent;} QCheckBox::indicator {border: "
                                                   "2px solid #5A5A5A; background: none; width: 12px; height: 12px;} QCheckBox::indicator:checked "
                                                   "{image: url(:/16x16/icons/16x16/cil-check-alt.png); border: 2px solid #5A5A5A; background: none; "
                                                   "width: 12px; height: 12px;}");
            ui_settings->frame->setStyleSheet("background-color: rgb(220, 220, 220);");
            ui_settings->label_temp->setStyleSheet("QLabel {color: rgb(3, 3, 5);}");
            ui_settings->label_output->setStyleSheet("QLabel {color: rgb(3, 3, 5);}");
            ui_settings->label_sec->setStyleSheet("QLabel {color: rgb(3, 3, 5);}");
            ui_settings->label_theme->setStyleSheet("QLabel {color: rgb(3, 3, 5);}");
            ui_settings->checkBox_1->setStyleSheet("color: rgb(3, 3, 5);");
            ui_settings->checkBox_3->setStyleSheet("color: rgb(3, 3, 5);");
            ui_settings->frame_3->setStyleSheet("background-color: rgba(27, 29, 35, 120); border-top: 1px solid rgba(100, 100, 168, 50);");
        }; break;
    }
}

bool Settings::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) // *************** Reset ************************* //
    {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->button() == Qt::LeftButton)
        {
            QGuiApplication::restoreOverrideCursor();
            clickPressedFlag = false;
            clickPressed_Left_ResizeFlag = false;
            clickPressed_Left_Top_ResizeFlag = false;
            clickPressed_Top_ResizeFlag = false;
            clickPressed_Right_Top_ResizeFlag = false;
            clickPressed_Right_ResizeFlag = false;
            clickPressed_Right_Bottom_ResizeFlag = false;
            clickPressed_Bottom_ResizeFlag = false;
            clickPressed_Left_Bottom_ResizeFlag = false;
            return QDialog::eventFilter(watched, event);
        }
    }

    if (watched == ui_settings->widget) // *************** Resize window realisation ************************* //
    {
        if (!this->isMaximized())
        {
            if (event->type() == QEvent::MouseButtonPress)
            {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->button() == Qt::LeftButton)
                {
                    _posX = this->pos().x();
                    _posY = this->pos().y();
                    _width = this->width();
                    _height = this->height();
                    mouseClickCoordinate = mouse_event->pos();
                    if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() > 62) && (mouseClickCoordinate.y() < (_height-12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                        clickPressed_Left_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() < 12))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                        clickPressed_Left_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > 12) && (mouseClickCoordinate.x() < (_width - 120)) && (mouseClickCoordinate.y() < 3))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                        clickPressed_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (_width - 12)) && (mouseClickCoordinate.y() < 12))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                        clickPressed_Right_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (_width - 12)) && (mouseClickCoordinate.y() > 62) && (mouseClickCoordinate.y() < (_height-12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                        clickPressed_Right_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (_width - 12)) && (mouseClickCoordinate.y() > (_height - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                        clickPressed_Right_Bottom_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > 12) && (mouseClickCoordinate.x() < (_width - 12)) && (mouseClickCoordinate.y() > (_height - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                        clickPressed_Bottom_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() > (_height - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                        clickPressed_Left_Bottom_ResizeFlag = true;
                    }
                    return QDialog::eventFilter(watched, event);
                }
            }
            else if (event->type() == QEvent::MouseMove)
            {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->buttons() & Qt::LeftButton)
                {
                    int deltaX = mouse_event->globalPos().x() - mouseClickCoordinate.x();
                    int deltaY = mouse_event->globalPos().y() - mouseClickCoordinate.y();
                    int deltaWidth = (int)mouse_event->localPos().x() - mouseClickCoordinate.x();
                    int deltaHeight = (int)mouse_event->localPos().y() - mouseClickCoordinate.y();
                    if (clickPressed_Left_ResizeFlag == true)
                    {
                        this->setGeometry(deltaX, _posY, this->width() - deltaWidth, _height);
                    }
                    else if (clickPressed_Left_Top_ResizeFlag == true)
                    {
                        this->setGeometry(deltaX, deltaY, this->width() - deltaWidth, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Top_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, deltaY, _width, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Right_Top_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, deltaY, _width + deltaWidth, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Right_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, _posY, _width + deltaWidth, _height);
                    }
                    else if (clickPressed_Right_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, _posY, _width + deltaWidth, _height + deltaHeight);
                    }
                    else if (clickPressed_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, _posY, _width, _height + deltaHeight);
                    }
                    else if (clickPressed_Left_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(deltaX, _posY, this->width() - deltaWidth, _height + deltaHeight);
                    }
                    return QDialog::eventFilter(watched, event);
                }
            }
        }
    }
    else if (watched == ui_settings->frame_hint) // *************** Drag window realisation ************************* //
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
                if (this->isMaximized())
                {
                    on_expandWindow_clicked();
                }
                this->move(mouse_event->globalPos() - mouseClickCoordinate);
                return QDialog::eventFilter(watched, event);
            }
        }
        else if (event->type() == QEvent::MouseButtonDblClick)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                on_expandWindow_clicked();
                return QDialog::eventFilter(watched, event);
            }
        }
    }
    return QDialog::eventFilter(watched, event);
}




