﻿#ifndef _UNICODE
#define _UNICODE
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "donate.h"
#include "settings.h"
#include "selectpreset.h"
#include "taskcomplete.h"
#include "dialog.h"
//#include "MediaInfoDLL/MediaInfoDLL.h"
#include <MediaInfo/MediaInfo.h>

#ifdef __MINGW32__
    #ifdef _UNICODE
        #define _itot _itow
    #else //_UNICODE
        #define _itot itoa
    #endif //_UNICODE
#endif //__MINGW32

//using namespace MediaInfoDLL;
using namespace MediaInfoLib;


extern QString _cur_param[23];
extern QVector <QVector <QString> > _preset_table;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)   //************** Call set parameters ****************//
{
    QMainWindow::showEvent(event);
    if (_windowActivated == false)
    {
        _windowActivated = true;
        setParameters();
    }
}

void MainWindow::setParameters()    //***************************** Set parameters ******************//
{
    ui->frame_hint->installEventFilter(this);
    ui->centralwidget->installEventFilter(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(repeat_handler()));
    _preset_table.resize(24);
    for (int i = 0; i < 24; i++) {
      _preset_table[i].resize(5);
    }
    // ***************************** Top menu actions ***********************************//
    connect(add_files, &QAction::triggered, this, &MainWindow::on_actionAdd_clicked);
    connect(remove_files, &QAction::triggered, this, &MainWindow::on_actionRemove_clicked);
    connect(close_prog, &QAction::triggered, this, &MainWindow::on_closeWindow_clicked);
    QMenu* menuFiles = new QMenu(this);
    menuFiles->addAction(add_files);
    menuFiles->addAction(remove_files);
    menuFiles->addSeparator();
    menuFiles->addAction(close_prog);
    menuFiles->setStyleSheet("QMenu {background-color: rgb(5, 20, 25);} "
                             "QMenu::item {color: rgb(255, 255, 255); background-color: transparent;} "
                             "QMenu::item:selected {background-color: rgb(5, 40, 45);}");
    ui->menuFileButton->setMenu(menuFiles);

    connect(select_preset, &QAction::triggered, this, &MainWindow::on_actionPreset_clicked);
    connect(encode_files, &QAction::triggered, this, &MainWindow::on_actionEncode_clicked);
    connect(stop_encode, &QAction::triggered, this, &MainWindow::on_actionStop_clicked);
    QMenu* menuEdit = new QMenu(this);
    menuEdit->addAction(select_preset);
    menuEdit->addSeparator();
    menuEdit->addAction(encode_files);
    menuEdit->addAction(stop_encode);
    menuEdit->setStyleSheet("QMenu {background-color: rgb(5, 20, 25);} "
                            "QMenu::item {color: rgb(255, 255, 255); background-color: transparent;} "
                            "QMenu::item:selected {background-color: rgb(5, 40, 45);}");
    ui->menuEditButton->setMenu(menuEdit);

    connect(settings, &QAction::triggered, this, &MainWindow::on_actionSettings_clicked);
    QMenu* menuPreferences = new QMenu(this);
    menuPreferences->addAction(settings);
    menuPreferences->setStyleSheet("QMenu {background-color: rgb(5, 20, 25);} "
                                   "QMenu::item {color: rgb(255, 255, 255); background-color: transparent;} "
                                   "QMenu::item:selected {background-color: rgb(5, 40, 45);}");
    ui->menuPreferencesButton->setMenu(menuPreferences);

    connect(about, &QAction::triggered, this, &MainWindow::on_actionAbout_clicked);
    connect(donate, &QAction::triggered, this, &MainWindow::on_actionDonate_clicked);
    QMenu* menuAbout = new QMenu(this);
    menuAbout->addAction(about);
    menuAbout->addAction(donate);
    menuAbout->setStyleSheet("QMenu {background-color: rgb(5, 20, 25);} "
                             "QMenu::item {color: rgb(255, 255, 255); background-color: transparent;} "
                             "QMenu::item:selected {background-color: rgb(5, 40, 45);}");
    ui->menuAboutButton->setMenu(menuAbout);
    // *********************************************************************************//

    QDesktopWidget *screenSize = QApplication::desktop();
    int screenWidth = screenSize->width();
    int screenHeight = screenSize->height();
    int widthMainWindow = 1024;
    int heightMainWindow = 670;
    int x_pos = static_cast<int>((float)(screenWidth - widthMainWindow)/2);
    int y_pos = static_cast<int>((float)(screenHeight - heightMainWindow)/2);
    bool expandFlag = false;
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    _openDir = QDir::homePath();
    _settings_path = QDir::homePath() + "/CineEncoder";
    _thumb_path = _settings_path + "/thumbnails";
    _settings_file = _settings_path + "/ce_settings";
    _preset_file = _settings_path + "/preset";
    _window_file = _settings_path + "/ce_window";
    _status_encode_btn = "start";
    _timer_interval = 30;
    _input_file = "";
    _output_folder = "";
    _output_file = "";
    _temp_folder = "";
    _temp_file = "";
    _protection = false;
    _batch_mode = false;
    _showHDR_mode = false;
    _row = -1;
    _theme = 0;
    ui->label_53->hide();
    ui->label_54->hide();
    ui->label_55->hide();
    ui->progressBar->hide();
    ui->tableWidget->verticalHeader()->setFixedWidth(0);
    ui->tableWidget->setColumnWidth(0, 250);
    ui->tableWidget->setColumnWidth(1, 80);
    ui->tableWidget->setColumnWidth(2, 85);
    ui->tableWidget->resizeColumnToContents(3);
    ui->tableWidget->setColumnWidth(4, 70);
    ui->tableWidget->setColumnWidth(5, 60);
    ui->tableWidget->setColumnWidth(6, 80);
    int i = 7;
    while (i <= 35) {
        ui->tableWidget->resizeColumnToContents(i);
        i++;
    };
    i = 7;
    while (i <= 10) {
        ui->tableWidget->hideColumn(i);
        i++;
    };
    i = 21;
    while (i <= 35) {
        ui->tableWidget->hideColumn(i);
        i++;
    };
    ui->tableWidget->setRowCount(0);
    process_1->setProcessChannelMode(QProcess::MergedChannels);
    process_1->setWorkingDirectory(QDir::homePath());
    if (!QDir(_settings_path).exists()) {
        QDir().mkdir(_settings_path);
        std::cout << "Setting path not existed and was created ..." << std::endl;  // Debug info //
    };
    if (QDir(_thumb_path).exists()) {
        unsigned int count_thumb = QDir(_thumb_path).count();
        std::cout << "Number of thumbnails: " << count_thumb << std::endl; // Debug info //
        if (count_thumb > 200) {
            QDir(_thumb_path).removeRecursively();
            std::cout << "Thumbnails removed... " << std::endl; // Debug info //
        };
    };
    if (!QDir(_thumb_path).exists()) {
        QDir().mkdir(_thumb_path);
        std::cout << "Thumbnail path not existed and was created ..." << std::endl;  // Debug info //
    };
    _stn_file.setFileName(_settings_file);
    _prs_file.setFileName(_preset_file);
    _wind_file.setFileName(_window_file);
    if (_stn_file.exists()) {
        std::cout << "Settings file exist ..." << std::endl;  // Debug info //
        if (_stn_file.open(QIODevice::ReadOnly | QIODevice::Text)) {  // Read settings from file
            QStringList line;
            while(!_stn_file.atEnd()) {
                line << _stn_file.readLine();
            };
            std::cout << "Number of lines in settings file: " << line.size() << std::endl; // Debug info //
            if (line.size() == 7) {
                _temp_folder = line[0].replace("temp_folder:", "").replace("\n", "");
                _output_folder = line[1].replace("output_folder:", "").replace("\n", "");
                if (line[2].indexOf("true") != -1) {
                    _batch_mode = true;
                };
                if (line[3].indexOf("true") != -1) {
                    _showHDR_mode = true;
                };
                if (line[4].indexOf("true") != -1) {
                    _protection = true;
                };
                _timer_interval = (line[5].replace("timer_interval:", "").replace("\n", "")).toInt();
                if (_timer_interval < 15) {
                    _timer_interval = 30;
                }
                timer->setInterval(_timer_interval*1000);
                _theme = (line[6].replace("theme:", "").replace("\n", "")).toInt();
                setTheme(_theme);
            } else {
                std::cout << "Setting file error, not enough parameters!!! " << std::endl;  // Debug info //
            };
            std::cout << "Temp folder: " << _temp_folder.toStdString() << std::endl;  // Debug info //
            std::cout << "Output folder: " << _output_folder.toStdString() << std::endl;  // Debug info //
            std::cout << "Batch mode: " << _batch_mode << std::endl;  // Debug info //
            std::cout << "Protection: " << _protection << std::endl;  // Debug info //
            std::cout << "Timer interval: " << _timer_interval << std::endl;  // Debug info //
            _stn_file.close();
        } else {
            std::cout << "Setting file error (cannot be open)." << std::endl;  // Debug info //
        }
    } else {
        std::cout << "Setting file not exist ..." << std::endl;  // Debug info //
    }
    if (_prs_file.exists()) {
        std::cout << "Preset file exist ..." << std::endl;  // Debug info //
        if (_prs_file.open(QIODevice::ReadOnly | QIODevice::Text)) {  // Read preset from file
            QStringList line;
            while(!_prs_file.atEnd()) {
                line << _prs_file.readLine();
            };
            std::cout << "Number of lines in preset file: " << line.size() << std::endl; // Debug info //
            if (line.size() > 2) {
                QStringList cur_param = line[0].split("<%>");
                if (cur_param.size() == 26) {
                    for (int k = 0; k < 23; k++) {
                        _cur_param[k] = cur_param[k];
                    };
                    _pos_top = cur_param[23].toInt();
                    _pos_cld = cur_param[24].toInt();
                    int n = line.size() - 1;
                    for (int i = 0; i < 24; i++) {
                      _preset_table[i].resize(n);
                    };
                    for (int j = 1; j <= n; j++) {
                        cur_param = line[j].split("<&>");
                        if (cur_param.size() == 25) {
                            for (int m = 0; m < 24; m++) {
                                _preset_table[m][j-1] = cur_param[m];
                            };
                        } else {
                            std::cout << "Preset column size: " << cur_param.size() << ". Error!!! Break!!!" << std::endl;  // Debug info //
                            break;
                        };
                    };
                } else {
                    std::cout << "Preset file error, uncorrect parameters!!! " << std::endl;  // Debug info //
                    set_defaults();
                };
            } else {
                std::cout << "Preset file error, not enough parameters!!! " << std::endl;  // Debug info //
                set_defaults();
            };
            _prs_file.close();
        } else {
            std::cout << "Preset file error (cannot be open)." << std::endl;  // Debug info //
            set_defaults();
        };
    } else {
        std::cout << "Preset file not exist ..." << std::endl;  // Debug info //
        set_defaults();
    }
    if (_wind_file.exists())
    {
        std::cout << "Window settings file exist ..." << std::endl;  // Debug info //
        if (_wind_file.open(QIODevice::ReadOnly | QIODevice::Text))   // Read settings from file
        {
            QStringList line;
            while(!_wind_file.atEnd())
            {
                line << _wind_file.readLine();
            }
            std::cout << "Number of lines in window settings file: " << line.size() << std::endl; // Debug info //
            if (line.size() == 6)
            {
                if (line[0].indexOf("true") != -1) {
                    expandFlag = true;
                }
                x_pos = line[1].replace("pos_x:", "").replace("\n", "").toInt();
                y_pos = line[2].replace("pos_y:", "").replace("\n", "").toInt();
                widthMainWindow = line[3].replace("width:", "").replace("\n", "").toInt();
                heightMainWindow = line[4].replace("height:", "").replace("\n", "").toInt();
                _openDir = line[5].replace("open_dir:", "").replace("\n", "");
            } else {
                std::cout << "Window settings file error, not enough parameters!!! " << std::endl;  // Debug info //
            }
            std::cout << "x_pos: " << x_pos << std::endl;  // Debug info //
            std::cout << "y_pos: " << y_pos << std::endl;  // Debug info //
            std::cout << "widthMainWindow: " << widthMainWindow << std::endl;  // Debug info //
            std::cout << "heightMainWindow: " << heightMainWindow << std::endl;  // Debug info //
            std::cout << "expand flag: " << expandFlag << std::endl;  // Debug info //
            _wind_file.close();
        } else {
            std::cout << "Window settings file error (cannot be open)." << std::endl;  // Debug info //
        }
    } else {
        std::cout << "Window settings file not exist ..." << std::endl;  // Debug info //
    }
    if (expandFlag == true) {
        on_expandWindow_clicked();
    } else {
        this->setGeometry(x_pos, y_pos, widthMainWindow, heightMainWindow);
    }
}

void MainWindow::on_closeWindow_clicked()
{
    this->close();
}

void MainWindow::closeEvent(QCloseEvent *event) //*********** Show prompt when close app ************//
{
    event->ignore();
    _message = "Quit program?";
    bool confirm = call_dialog(_message);
    if (confirm == true)
    {
        std::cout << "Exit confirmed!" << std::endl;  // Debug info //
        short s1 = process_1->state();
        if (s1 != 0) {
            process_1->kill();
        }
        if (_prs_file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QString line = "";
            for (int i = 0; i < 23; i++)
            {
                line += _cur_param[i] + "<%>";
            }
            line += QString::number(_pos_top) + "<%>" +  QString::number(_pos_cld) + "<%>" + "\n";
            _prs_file.write(line.toUtf8());
            int m = _preset_table.size();  // Column count
            int n =  _preset_table[0].size();  // Row count
            if (n < 500 && m < 25)
            {
                for (int i = 0; i < n; i++)
                {
                    line = "";
                    for (int j = 0; j < m; j++)
                    {
                        line += _preset_table[j][i] + "<&>";
                    }
                    line += "\n";
                    _prs_file.write(line.toUtf8());
                }
            }
            _prs_file.close();
            std::cout << "Preset file saved..." << std::endl;  // Debug info //
        }
        if (_wind_file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            if (_expandWindowsState) {
                _wind_file.write("expand:true\n");
            } else {
                _wind_file.write("expand:false\n");
            };
            QString line_1 = "pos_x:" + QString::number(this->pos().x()) + "\n";
            _wind_file.write(line_1.toUtf8());
            QString line_2 = "pos_y:" + QString::number(this->pos().y()) + "\n";
            _wind_file.write(line_2.toUtf8());
            QString line_3 = "width:" + QString::number(this->width()) + "\n";
            _wind_file.write(line_3.toUtf8());
            QString line_4 = "height:" + QString::number(this->height()) + "\n";
            _wind_file.write(line_4.toUtf8());
            QString line_5 = "open_dir:" + _openDir + "\n";
            _wind_file.write(line_5.toUtf8());
            _wind_file.close();
        }
        event->accept();
    }
}

void MainWindow::on_expandWindow_clicked()
{
    if (_expandWindowsState == false)
    {
        this->showMaximized();
        _expandWindowsState = true;
        QIcon iconShowMax;
        iconShowMax.addFile(QString::fromUtf8(":/16x16/icons/16x16/cil-clone.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->expandWindow->setIcon(iconShowMax);
    } else {
        this->showNormal();
        _expandWindowsState = false;
        QIcon iconShowNorm;
        iconShowNorm.addFile(QString::fromUtf8(":/16x16/icons/16x16/cil-media-stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->expandWindow->setIcon(iconShowNorm);
    }
}

void MainWindow::on_hideWindow_clicked()
{
    this->showMinimized();
}

void MainWindow::on_actionAdd_clicked() //**************************** Add files ********************//
{
    ui->label_53->hide();
    ui->label_54->hide();
    ui->label_55->hide();
    ui->progressBar->hide();
    QFileDialog *openFilesWindow = new QFileDialog(this);
    //openFilesWindow->setOptions(QFileDialog::DontUseNativeDialog | QFileDialog::DontResolveSymlinks);
    openFilesWindow->setOptions(QFileDialog::DontResolveSymlinks);
    openFilesWindow->setFileMode(QFileDialog::ExistingFiles);
    openFilesWindow->setDirectory(_openDir);
    openFilesWindow->setMinimumWidth(600);
    openFilesWindow->setWindowTitle("Open Files");
    openFilesWindow->setNameFilter(tr("Video Files: *.avi, *.m2ts, *.m4v, *.mkv, *.mov, *.mp4, "
                                      "*.mpeg, *.mpg, *.mxf, *.ts, *.webm (*.avi *.m2ts *.m4v "
                                      "*.mkv *.mov *.mp4 *.mpeg *.mpg *.mxf *.ts *.webm);;All files (*.*)"));
    openFilesWindow->setWindowFlags(Qt::Dialog | Qt::SubWindow);
    openFilesWindow->exec();
    int res = openFilesWindow->result();
    QStringList file_name_open;
    file_name_open = openFilesWindow->selectedFiles();
    delete openFilesWindow;
    if (res == 0) {
        return;
    } else {
        MediaInfo MI;
        int i = 1;
        unsigned long sep = 0;
        int count = file_name_open.size();
        while (i <= count) {
            int numRows = ui->tableWidget->rowCount();
            ui->tableWidget->setRowCount(numRows + 1);
            QString file_qstr = file_name_open.at(i-1);
            std::wstring file_wstr = file_qstr.toStdWString();
            sep = file_wstr.rfind('/');
            std::wstring folder_input_str = file_wstr.substr(0, sep);
            std::wstring file_input_str = file_wstr.substr(sep+1);
            QString folder_input = QString::fromStdWString(folder_input_str);
            if (i == 1) {
                _openDir = folder_input;
            }
            QString file_input = QString::fromStdWString(file_input_str);
            QTableWidgetItem *newItem_file = new QTableWidgetItem(file_input);
            ui->tableWidget->setItem(numRows, 0, newItem_file);
            QTableWidgetItem *newItem_folder = new QTableWidgetItem(folder_input);
            ui->tableWidget->setItem(numRows, 20, newItem_folder);
            MI.Open(file_wstr);
            std::wstring fmt_wstr = MI.Get(Stream_Video, 0, L"Format");
            std::wstring stream_size_wstr = MI.Get(Stream_Video, 0, L"StreamSize");
            std::wstring dur_wstr = MI.Get(Stream_Video, 0, L"Duration");
            std::wstring aspect_ratio_wstr = MI.Get(Stream_Video, 0, L"DisplayAspectRatio");
            std::wstring color_space_wstr = MI.Get(Stream_Video, 0, L"ColorSpace");
            std::wstring bit_depth_wstr = MI.Get(Stream_Video, 0, L"BitDepth");
            std::wstring width_wstr = MI.Get(Stream_Video, 0, L"Width");
            std::wstring height_wstr = MI.Get(Stream_Video, 0, L"Height");
            std::wstring fps_wstr = MI.Get(Stream_Video, 0, L"FrameRate");
            std::wstring chroma_subsampling_wstr = MI.Get(Stream_Video, 0, L"ChromaSubsampling");
            std::wstring bitrate_wstr = MI.Get(Stream_Video, 0, L"BitRate");
            std::wstring color_range_wstr = MI.Get(Stream_Video, 0, L"colour_range");
            std::wstring color_primaries_wstr = MI.Get(Stream_Video, 0, L"colour_primaries");
            std::wstring matrix_coefficients_wstr = MI.Get(Stream_Video, 0, L"matrix_coefficients");
            std::wstring transfer_characteristics_wstr = MI.Get(Stream_Video, 0, L"transfer_characteristics");
            std::wstring mastering_display_luminance_wstr = MI.Get(Stream_Video, 0, L"MasteringDisplay_Luminance");
            std::wstring maxCll_wstr = MI.Get(Stream_Video, 0, L"MaxCLL");
            std::wstring maxFall_wstr = MI.Get(Stream_Video, 0, L"MaxFALL");
            std::wstring mastering_display_color_primaries_wstr = MI.Get(Stream_Video, 0, L"MasteringDisplay_ColorPrimaries");
            int dur_int = 0;
            try {
                dur_int = int(0.001 * std::stoi(dur_wstr));
            }  catch (...) {
                std::cout << "No duration info!" << std::endl;
            };
            int h = static_cast<int>(trunc(dur_int / 3600));
            int m = static_cast<int>(trunc((dur_int - (h * 3600)) / 60));
            int s = static_cast<int>(trunc(dur_int - (h * 3600) - (m * 60)));
            QString hrs = QString::number(h);
            QString min = QString::number(m);
            QString sec = QString::number(s);
            std::ostringstream sstr;
            sstr << std::setw(2) << std::setfill('0') << hrs.toStdString() << ":"
                 << std::setw(2) << std::setfill('0') << min.toStdString() << ":"
                 << std::setw(2) << std::setfill('0') << sec.toStdString();
            std::string tm = sstr.str();
            int bit_rate = 0;
            try {
                bit_rate = int(0.001 * std::stoi(bitrate_wstr));
            }  catch (...) {
                std::cout << "No bitrate info!" << std::endl;
            };
            QString dur_qstr = QString::number(dur_int);
            QString stream_size_qstr = QString::fromStdWString(stream_size_wstr);
            QString mastering_display_luminance_qstr = QString::fromStdWString(mastering_display_luminance_wstr);
            QString mastering_display_luminance_rep = mastering_display_luminance_qstr.replace("min: ", "").replace("max: ", "").replace(" cd/m2", "");
            QString min_luminance = mastering_display_luminance_rep.split(", ")[0];
            QString max_luminance = mastering_display_luminance_rep.replace(min_luminance, "").replace(", ", "");
            QString color_primaries = QString::fromStdWString(color_primaries_wstr);
            QString matrix_coefficients = QString::fromStdWString(matrix_coefficients_wstr);
            QString transfer_characteristics = QString::fromStdWString(transfer_characteristics_wstr);
            QString mastering_display_color_primaries = QString::fromStdWString(mastering_display_color_primaries_wstr);
            QString color_range = QString::fromStdWString(color_range_wstr);
            QString maxCll = QString::fromStdWString(maxCll_wstr);
            QString maxFall = QString::fromStdWString(maxFall_wstr);
            QString width_qstr = QString::fromStdWString(width_wstr);
            QString height_qstr = QString::fromStdWString(height_wstr);
            QString h_qstr = QString::number(h);
            QString m_qstr = QString::number(m);
            QString s_qstr = QString::number(s);
            QString chroma_subsampling = QString::fromStdWString(chroma_subsampling_wstr);
            QString bit_rate_qstr = QString::number(bit_rate);
            QString aspect_ratio = QString::fromStdWString(aspect_ratio_wstr);
            QString color_space = QString::fromStdWString(color_space_wstr);
            QString bit_depth = QString::fromStdWString(bit_depth_wstr);
            QString fmt_qstr = QString::fromStdWString(fmt_wstr);
            QString fps_qstr = QString::fromStdWString(fps_wstr);
            QString size = width_qstr + "x" + height_qstr;
            if (fmt_qstr == "") {
                fmt_qstr = "Undef";
            }
            if (fps_qstr == "") {
                fps_qstr = "Undef";
            }
            if (width_qstr == "") {
                size = "Undef";
            }
            if (tm == "00:00:00") {
                tm = "Undef";
            }
            QTableWidgetItem *newItem_fmt = new QTableWidgetItem(fmt_qstr);
            ui->tableWidget->setItem(numRows, 1, newItem_fmt);
            QTableWidgetItem *newItem_resolution = new QTableWidgetItem(size);
            ui->tableWidget->setItem(numRows, 2, newItem_resolution);
            QTableWidgetItem *newItem_duration = new QTableWidgetItem(QString::fromStdString(tm));
            ui->tableWidget->setItem(numRows, 3, newItem_duration);
            QTableWidgetItem *newItem_fps = new QTableWidgetItem(fps_qstr);
            ui->tableWidget->setItem(numRows, 4, newItem_fps);
            QTableWidgetItem *newItem_aspect_ratio = new QTableWidgetItem(aspect_ratio);
            ui->tableWidget->setItem(numRows, 5, newItem_aspect_ratio);
            QTableWidgetItem *newItem_bitrate = new QTableWidgetItem(bit_rate_qstr);
            ui->tableWidget->setItem(numRows, 7, newItem_bitrate);
            QTableWidgetItem *newItem_subsampling = new QTableWidgetItem(chroma_subsampling);
            ui->tableWidget->setItem(numRows, 8, newItem_subsampling);
            QTableWidgetItem *newItem_bit_depth = new QTableWidgetItem(bit_depth);
            ui->tableWidget->setItem(numRows, 9, newItem_bit_depth);
            QTableWidgetItem *newItem_color_space = new QTableWidgetItem(color_space);
            ui->tableWidget->setItem(numRows, 10, newItem_color_space);
            QTableWidgetItem *newItem_color_range = new QTableWidgetItem(color_range);
            ui->tableWidget->setItem(numRows, 11, newItem_color_range);
            QTableWidgetItem *newItem_color_primaries = new QTableWidgetItem(color_primaries.replace(".", ""));
            ui->tableWidget->setItem(numRows, 12, newItem_color_primaries);
            QTableWidgetItem *newItem_color_matrix = new QTableWidgetItem(matrix_coefficients.replace(" ", "").replace(".", "").replace("on-", "").replace("onstant", ""));
            ui->tableWidget->setItem(numRows, 13, newItem_color_matrix);
            QTableWidgetItem *newItem_transfer_characteristics = new QTableWidgetItem(transfer_characteristics.replace(".", ""));
            ui->tableWidget->setItem(numRows, 14, newItem_transfer_characteristics);
            QTableWidgetItem *newItem_max_lum = new QTableWidgetItem(max_luminance);
            ui->tableWidget->setItem(numRows, 15, newItem_max_lum);
            QTableWidgetItem *newItem_min_lum = new QTableWidgetItem(min_luminance);
            ui->tableWidget->setItem(numRows, 16, newItem_min_lum);
            QTableWidgetItem *newItem_max_cll = new QTableWidgetItem(maxCll.replace(" cd/m2", ""));
            ui->tableWidget->setItem(numRows, 17, newItem_max_cll);
            QTableWidgetItem *newItem_max_fall = new QTableWidgetItem(maxFall.replace(" cd/m2", ""));
            ui->tableWidget->setItem(numRows, 18, newItem_max_fall);
            int len = mastering_display_color_primaries.length();
            if (len > 15) {
                QStringList mdcp = mastering_display_color_primaries.split(",");
                QString r = mdcp[0].replace("R: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
                QString g = mdcp[1].replace(" G: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
                QString b = mdcp[2].replace(" B: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
                QString white_coord = mdcp[3].replace(" White point: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
                QString chr_coord = r + "," + g + "," + b;
                QTableWidgetItem *newItem_mastering_display = new QTableWidgetItem("Undefined");
                ui->tableWidget->setItem(numRows, 19, newItem_mastering_display);
                QTableWidgetItem *newItem_chr_coord = new QTableWidgetItem(chr_coord);
                ui->tableWidget->setItem(numRows, 22, newItem_chr_coord);
                QTableWidgetItem *newItem_white_coord = new QTableWidgetItem(white_coord);
                ui->tableWidget->setItem(numRows, 23, newItem_white_coord);
            } else {
                QTableWidgetItem *newItem_mastering_display = new QTableWidgetItem(mastering_display_color_primaries);
                ui->tableWidget->setItem(numRows, 19, newItem_mastering_display);
                QTableWidgetItem *newItem_chr_coord = new QTableWidgetItem("");
                ui->tableWidget->setItem(numRows, 22, newItem_chr_coord);
                QTableWidgetItem *newItem_white_coord = new QTableWidgetItem("");
                ui->tableWidget->setItem(numRows, 23, newItem_white_coord);
            };
            QTableWidgetItem *newItem_dur = new QTableWidgetItem(dur_qstr);
            ui->tableWidget->setItem(numRows, 21, newItem_dur);
            QTableWidgetItem *newItem_stream_size = new QTableWidgetItem(stream_size_qstr);
            ui->tableWidget->setItem(numRows, 24, newItem_stream_size);
            QTableWidgetItem *newItem_width = new QTableWidgetItem(width_qstr);
            ui->tableWidget->setItem(numRows, 25, newItem_width);
            QTableWidgetItem *newItem_height = new QTableWidgetItem(height_qstr);
            ui->tableWidget->setItem(numRows, 26, newItem_height);
            int j = 0;
            int smplrt_int = 0;
            std::wstring format_wstr;
            std::wstring smplrt_wstr;
            QString format;
            QString smplrt = "";
            while (j <= 8) {
                format_wstr = MI.Get(Stream_Audio, size_t(j), L"Format");
                smplrt_wstr = MI.Get(Stream_Audio, size_t(j), L"SamplingRate");
                format = QString::fromStdWString(format_wstr);
                try {
                    smplrt_int = int(std::stoi(smplrt_wstr) / 1000);
                }  catch (...) {
                    smplrt_int = 0;
                };
                if (smplrt_int != 0) {
                    smplrt = QString::number(smplrt_int);
                } else {
                    smplrt = "";
                };
                if (format != "") {
                    QTableWidgetItem *newItem_audio = new QTableWidgetItem(format + "  " + smplrt + " kHz");
                    ui->tableWidget->setItem(numRows, j + 27, newItem_audio);
                } else {
                    QTableWidgetItem *newItem_audio = new QTableWidgetItem("");
                    ui->tableWidget->setItem(numRows, j + 27, newItem_audio);
                };
                j++;
            };
            i++;
        };
        MI.Close();
        ui->tableWidget->selectRow(0);
    };
}

void MainWindow::on_actionRemove_clicked()  //******************* Remove file from table ************//
{
    _row = ui->tableWidget->currentRow();
    if (_row != -1) {
        ui->tableWidget->removeRow(_row);
    }
}

void MainWindow::on_tableWidget_itemSelectionChanged()  //******* Item selection changed ************//
{
    _row = ui->tableWidget->currentRow();
    if (_row != -1) {
        get_current_data();
    } else {
        ui->labelThumb->clear();
        ui->labelThumb->setText("Preview");
        ui->label_source->setText("");
        ui->textBrowser_1->clear();
        ui->label_output->setText("");
        ui->textBrowser_2->clear();
        _dur = 0;
        _stream_size = "";
        _width = "";
        _height = "";
        _fmt = "";
        _fps = "";
        _hdr[0] = "";  // color range
        _hdr[1] = "";  // color primary
        _hdr[2] = "";  // color matrix
        _hdr[3] = "";  // transfer
        _hdr[4] = "";  // max lum
        _hdr[5] = "";  // min lum
        _hdr[6] = "";  // max cll
        _hdr[7] = "";  // max fall
        _hdr[8] = "";  // master display
        _hdr[9] = "";  // chr coord
        _hdr[10] = "";  // white coord
        _input_file = "";
    };
}

void MainWindow::make_preset()  //*********************************** Make preset *******************//
{
    std::cout << "Make preset..." << std::endl; // Debug information //
    int p1 = _cur_param[1].toInt();         //p1 - int codec
    int p2 = _cur_param[2].toInt();         //p2 - int mode
    QString p4 = _cur_param[4];             //p4 - QString bitrate/quantizer/ratefactor
    QString p5 = _cur_param[5];             //p5 - QString maxrate
    QString p6 = _cur_param[6];             //p6 - QString bufsize
    bool p7 = bool(_cur_param[7].toInt());  //p7 - bool checkBox_2 isEnabled
    int p8 = _cur_param[8].toInt();         //p8 - int  checkBox_2 checkState
    QString p9 = _cur_param[9];             //p9 - QString width
    QString p10 = _cur_param[10];           //p10 - QString height
    int p11 = _cur_param[11].toInt();       //p11 - int pass
    int p12 = _cur_param[12].toInt();       //p12 - int preset
    int p13 = _cur_param[13].toInt();       //p13 - int color_range
    QString p14 = _cur_param[14];           //p14 - QString min_lum
    QString p15 = _cur_param[15];           //p15 - QString max_lum
    QString p16 = _cur_param[16];           //p16 - QString max_cll
    QString p17 = _cur_param[17];           //p17 - QString max_fall
    int p18 = _cur_param[18].toInt();       //p18 - int master_disp
    QString p19 = _cur_param[19];           //p19 - QString chroma_coord
    QString p20 = _cur_param[20];           //p20 - QString white_coord
    int p21 = _cur_param[21].toInt();       //p21 - int acodec
    int p22 = _cur_param[22].toInt();       //p22 - int abitrate

    QTableWidgetItem *newItem_status = new QTableWidgetItem("");
    ui->tableWidget->setItem(_row, 6, newItem_status);


    _preset_0 = "";
    _preset_pass1 = "";
    _preset = "";
    _preset_mkvmerge = "";
    _error_message = "";
    _flag_two_pass = false;
    _flag_hdr = false;
    _mux_mode = false;
    _fr_count = 0;

    QString resize;
    if ((p7 == true) && (p8 == 2)) {
        if ((p9 != "") && (p10 != "") && (p9 != "Source") && (p10 != "Source")) {
            resize = QString("-vf scale=%1:%2 ").arg(p9).arg(p10);
        };
    };

    QString arr_codec[12][2] = {
        {"-map_metadata -1 -map ?0:v:0 -pix_fmt yuv420p10le -c:v libx265 -profile:v main10 ",        ""},
        {"-map_metadata -1 -map ?0:v:0 -pix_fmt yuv420p -c:v libx265 -profile:v main ",              ""},
        {"-map_metadata -1 -map ?0:v:0 -pix_fmt yuv420p -c:v libx264 -profile:v high ",              ""},
        {"-map_metadata -1 -map ?0:v:0 -pix_fmt p010le -c:v hevc_nvenc -profile:v main10 ",          " -hwaccel cuda"},
        {"-map_metadata -1 -map ?0:v:0 -pix_fmt yuv420p -c:v hevc_nvenc -profile:v main ",           " -hwaccel cuda"},
        {"-map_metadata -1 -map ?0:v:0 -pix_fmt yuv420p -c:v h264_nvenc -profile:v high ",           " -hwaccel cuda"},
        {"-map_metadata -1 -map ?0:v:0 -pix_fmt yuv420p10le -c:v libvpx-vp9 -speed 4 -profile:v 2 ", ""},
        {"-map_metadata -1 -map ?0:v:0 -pix_fmt yuv420p10le -c:v librav1e -strict -2 ",              ""},
        {"-map ?0:v:0 -pix_fmt yuv422p10le -c:v prores_ks -profile:v 3 ",                            ""},
        {"-map ?0:v:0 -pix_fmt yuv444p10 -c:v prores_ks -profile:v 4 ",                              ""},
        {"-map ?0:v:0 -pix_fmt yuv422p10le -c:v dnxhd -profile:v dnxhr_hqx ",                        ""},
        {"-map ?0:v:0 -movflags +write_colr -c:v copy ",                                             ""}
    };
    QString codec = arr_codec[p1][0];
    QString hwaccel = arr_codec[p1][1];
    if ((p1 == 0) or (p1 == 3) or (p1 > 5)) {
        _flag_hdr = true;
    };

    QString arr_mode[12][5] = {
        {"CBR", "ABR", "VBR", "CRF", "CQP"},
        {"CBR", "ABR", "VBR", "CRF", "CQP"},
        {"CBR", "ABR", "VBR", "CRF", "CQP"},
        {"VBR", "",    "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"ABR", "CRF", "",    "",    ""},
        {"ABR", "CRF", "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""}
    };
    QString mode = "";
    if (arr_mode[p1][p2] == "CBR") {
        mode = QString("-b:v %1 -minrate %1 -maxrate %1 -bufsize %1 ").arg(p4);
    };
    if (arr_mode[p1][p2] == "ABR") {
        mode = QString("-b:v %1 ").arg(p4);
    };
    if (arr_mode[p1][p2] == "VBR") {
        if ((p1 >= 3) && (p1 <= 5)) {
            mode = QString("-b:v %1 -maxrate %2 -bufsize %3 -rc vbr_hq -rc-lookahead:v 32 ").arg(p4, p5, p6);
        } else {
            mode = QString("-b:v %1 -maxrate %2 -bufsize %3 ").arg(p4, p5, p6);
        };
    };
    if (arr_mode[p1][p2] == "CRF") {
        mode = QString("-b:v 0 -crf %1 ").arg(p4);
    };
    if (arr_mode[p1][p2] == "CQP") {
        mode = QString("-b:v 0 -cq %1 -qmin %1 -qmax %1 ").arg(p4);
    };

    QString arr_preset[12][9] = {
        {"Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow", "Slower", "Veryslow"},
        {"Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow", "Slower", "Veryslow"},
        {"Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow", "Slower", "Veryslow"},
        {"Slow",      "",          "",         "",       "",     "",       "",     "",       ""},
        {"Slow",      "",          "",         "",       "",     "",       "",     "",       ""},
        {"Slow",      "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""}
    };
    QString preset = "";
    if (arr_preset[p1][p12] != "") {
        preset = "-preset " + arr_preset[p1][p12].toLower() + " ";
    };

    QString arr_pass[12][2] = {
        {"",                     "-x265-params pass=2 "},
        {"",                     "-x265-params pass=2 "},
        {"",                     "-pass 2 "},
        {"-2pass 1 ",            ""},
        {"-2pass 1 ",            ""},
        {"-2pass 1 ",            ""},
        {"",                     "-pass 2 "},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""}
    };
    QString pass1 = "";
    QString pass = arr_pass[p1][p11];
    if (pass == "-x265-params pass=2 ") {
        pass1 = "-x265-params pass=1 ";
        _flag_two_pass = true;
    };
    if (pass == "-pass 2 ") {
        pass1 = "-pass 1 ";
        _flag_two_pass = true;
    };

    QString arr_acodec[12][6] = {
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"Opus",  "Vorbis", "Source", "",       "",     ""},
        {"Opus",  "Vorbis", "Source", "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"AAC",   "AC3",    "DTS",    "Vorbis", "Opus", "Source"}
    };

    QString arr_bitrate[5][17] = {
        {"384k",  "320k",  "256k",  "192k",  "128k",  "96k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AAC
        {"640k",  "448k",  "384k",  "256k",  "",      "",      "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AC3
        {"3840k", "3072k", "2048k", "1920k", "1536k", "1472k", "1344k", "1280k", "1152k", "1024k", "960k", "768k", "640k", "576k", "512k", "448k", "384k"}, // DTS
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // Vorbis
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""} // Opus
    };
    QString acodec = "";
    if (arr_acodec[p1][p21] == "AAC") {
        acodec = QString("-map ?0:a -c:a aac -b:a %1").arg(arr_bitrate[0][p22]);
    };
    if (arr_acodec[p1][p21] == "AC3") {
        acodec = QString("-map ?0:a -c:a ac3 -b:a %1").arg(arr_bitrate[1][p22]);
    };
    if (arr_acodec[p1][p21] == "DTS") {
        acodec = QString("-map ?0:a -strict -2 -c:a dca -b:a %1 -ar 48000").arg(arr_bitrate[2][p22]);
    };
    if (arr_acodec[p1][p21] == "Vorbis") {
        acodec = QString("-map ?0:a -c:a libvorbis -b:a %1").arg(arr_bitrate[3][p22]);
    };
    if (arr_acodec[p1][p21] == "Opus") {
        acodec = QString("-map ?0:a -c:a libopus -b:a %1").arg(arr_bitrate[4][p22]);
    };
    if (arr_acodec[p1][p21] == "PCM16") {
        acodec = "-map ?0:a:0 -c:a pcm_s16le";
    };
    if (arr_acodec[p1][p21] == "PCM24") {
        acodec = "-map ?0:a:0 -c:a pcm_s24le";
    };
    if (arr_acodec[p1][p21] == "PCM32") {
        acodec = "-map ?0:a:0 -c:a pcm_s32le";
    };
    if (arr_acodec[p1][p21] == "Source") {
        acodec = "-map ?0:a -c:a copy";
    };

    QString colorprim = "";                         // color primaries
    if (_hdr[1] == "BT709") {
        colorprim = "-color_primaries bt709 ";
    };
    if (_hdr[1] == "BT2020") {
        colorprim = "-color_primaries bt2020 ";
    };
    if (_hdr[1] == "BT601 NTSC") {
        colorprim = "-color_primaries smpte170m ";
    };
    if (_hdr[1] == "BT601 PAL") {
        colorprim = "-color_primaries bt470bg ";
    };
    if (_hdr[1] == "BT470 System M") {
        colorprim = "-color_primaries bt470m ";
    };
    if (_hdr[1] == "SMPTE 240M") {
        colorprim = "-color_primaries smpte240m ";
    };
    if (_hdr[1] == "Generic film") {
        colorprim = "-color_primaries film ";
    };
    if (_hdr[1] == "DCI P3") {
        colorprim = "-color_primaries smpte431 ";
    };
//    if (_hdr[1] == "") {
//        colorprim = "-color_primaries smpte428 ";
//    };
//    if (_hdr[1] == "Display P3") {
//        colorprim = "-color_primaries smpte432 ";
//    };

    QString colormatrix = "";                       // color matrix
    if (_hdr[2] == "BT709") {
        colormatrix = "-colorspace bt709 ";
    };
    if (_hdr[2] == "BT2020nc") {
        colormatrix = "-colorspace bt2020nc ";
    };
    if (_hdr[2] == "BT2020c") {
        colormatrix = "-colorspace bt2020c ";
    };
    if (_hdr[2] == "FCC73682") {
        colormatrix = "-colorspace fcc ";
    };
    if (_hdr[2] == "BT470SystemB/G") {
        colormatrix = "-colorspace bt470bg ";
    };
    if (_hdr[2] == "SMPTE240M") {
        colormatrix = "-colorspace smpte240m ";
    };
    if (_hdr[2] == "YCgCo") {
        colormatrix = "-colorspace YCgCo ";
    };
    if (_hdr[2] == "Y'D'zD'x") {
        colormatrix = "-colorspace smpte2085 ";
    };
    if (_hdr[2] == "Chromaticity-derivednc") {
        colormatrix = "-colorspace chroma-derived-nc ";
    };
    if (_hdr[2] == "Chromaticity-derivedc") {
        colormatrix = "-colorspace chroma-derived-c ";
    };
    if (_hdr[2] == "ICtCp") {
        colormatrix = "-colorspace ICtCp ";
    };
//    if (_hdr[2] == "") {
//        colormatrix = "-colorspace smpte170m ";
//    };
//    if (_hdr[2] == "") {
//        colormatrix = "-colorspace GBR ";
//    };

    QString transfer = "";                          // transfer characteristics
    if (_hdr[3] == "BT709") {
        transfer = "-color_trc bt709 ";
    };
    if (_hdr[3] == "PQ") {
        transfer = "-color_trc smpte2084 ";
    };
    if (_hdr[3] == "HLG") {
        transfer = "-color_trc arib-std-b67 ";
    };
    if (_hdr[3] == "BT2020 (10-bit)") {
        transfer = "-color_trc bt2020-10 ";
    };
    if (_hdr[3] == "BT2020 (12-bit)") {
        transfer = "-color_trc bt2020-12 ";
    };
    if (_hdr[3] == "BT470 System M") {
        transfer = "-color_trc bt470m ";
    };
    if (_hdr[3] == "BT470 System B/G") {
        transfer = "-color_trc bt470bg ";
    };
    if (_hdr[3] == "SMPTE 240M") {
        transfer = "-color_trc smpte240m ";
    };
    if (_hdr[3] == "Linear") {
        transfer = "-color_trc linear ";
    };
    if (_hdr[3] == "Logarithmic (100:1)") {
        transfer = "-color_trc log100 ";
    };
    if (_hdr[3] == "Logarithmic (31622777:1)") {
        transfer = "-color_trc log316 ";
    };
    if (_hdr[3] == "xvYCC") {
        transfer = "-color_trc iec61966-2-4 ";
    };
    if (_hdr[3] == "BT1361") {
        transfer = "-color_trc bt1361e ";
    };
    if (_hdr[3] == "sRGB/sYCC") {
        transfer = "-color_trc iec61966-2-1 ";
    };
    if (_hdr[3] == "SMPTE 428M") {
        transfer = "-color_trc smpte428 ";
    };
//    if (_hdr[3] == "") {
//        transfer = "-color_trc smpte170m ";
//    };

    QString color_range = "";
    QString max_lum = "";
    QString min_lum = "";
    QString max_cll = "";
    QString max_fall = "";
    QString chroma_coord = "";
    QString white_coord = "";
    if (_flag_hdr == true) {
        if (p13 == 0) {                             // color range
            if (_hdr[0] == "Limited") {
                color_range = "-color_range tv ";
            };
            if (_hdr[0] == "Full") {
                color_range = "-color_range pc ";
            };
        };
        if (p13 == 1) {
            color_range = "-color_range pc ";
        };
        if (p13 == 2) {
            color_range = "-color_range tv ";
        };

        if (p15 != "") {                           // max lum
            max_lum = QString("-s max-luminance=%1 ").arg(p15);
        } else {
            if (_hdr[4] != "") {
                max_lum = QString("-s max-luminance=%1 ").arg(_hdr[4]);
            } else {
                max_lum = "-d max-luminance ";
            };
        };

        if (p14 != "") {                           // min lum
            min_lum = QString("-s min-luminance=%1 ").arg(p14);
        } else {
            if (_hdr[5] != "") {
                min_lum = QString("-s min-luminance=%1 ").arg(_hdr[5]);
            } else {
                min_lum = "-d min-luminance ";
            };
        };

        if (p16 != "") {                           // max cll
            max_cll = QString("-s max-content-light=%1 ").arg(p16);
        } else {
            if (_hdr[6] != "") {
                max_cll = QString("-s max-content-light=%1 ").arg(_hdr[6]);
            } else {
                max_cll = "-d max-content-light ";
            };
        };

        if (p17 != "") {                           // max fall
            max_fall = QString("-s max-frame-light=%1 ").arg(p17);
        } else {
            if (_hdr[7] != "") {
                max_fall = QString("-s max-frame-light=%1 ").arg(_hdr[7]);
            } else {
                max_fall = "-d max-frame-light ";
            };
        };

        QString chroma_coord_curr_red_x = "";
        QString chroma_coord_curr_red_y = "";
        QString chroma_coord_curr_green_x = "";
        QString chroma_coord_curr_green_y = "";
        QString chroma_coord_curr_blue_x = "";
        QString chroma_coord_curr_blue_y = "";
        QString white_coord_curr_x = "";
        QString white_coord_curr_y = "";
        if (p18 == 0) {     // From source
            if (_hdr[8] == "Display P3") {
                chroma_coord_curr_red_x = "0.680";
                chroma_coord_curr_red_y = "0.320";
                chroma_coord_curr_green_x = "0.265";
                chroma_coord_curr_green_y = "0.690";
                chroma_coord_curr_blue_x = "0.150";
                chroma_coord_curr_blue_y = "0.060";
                white_coord_curr_x = "0.3127";
                white_coord_curr_y = "0.3290";
            };
            if (_hdr[8] == "DCI P3") {
                chroma_coord_curr_red_x = "0.680";
                chroma_coord_curr_red_y = "0.320";
                chroma_coord_curr_green_x = "0.265";
                chroma_coord_curr_green_y = "0.690";
                chroma_coord_curr_blue_x = "0.150";
                chroma_coord_curr_blue_y = "0.060";
                white_coord_curr_x = "0.314";
                white_coord_curr_y = "0.3510";
            };
            if (_hdr[8] == "BT.2020") {
                chroma_coord_curr_red_x = "0.708";
                chroma_coord_curr_red_y = "0.292";
                chroma_coord_curr_green_x = "0.170";
                chroma_coord_curr_green_y = "0.797";
                chroma_coord_curr_blue_x = "0.131";
                chroma_coord_curr_blue_y = "0.046";
                white_coord_curr_x = "0.3127";
                white_coord_curr_y = "0.3290";
            };
            if (_hdr[8] == "BT.709") {
                chroma_coord_curr_red_x = "0.640";
                chroma_coord_curr_red_y = "0.330";
                chroma_coord_curr_green_x = "0.30";
                chroma_coord_curr_green_y = "0.60";
                chroma_coord_curr_blue_x = "0.150";
                chroma_coord_curr_blue_y = "0.060";
                white_coord_curr_x = "0.3127";
                white_coord_curr_y = "0.3290";
            };
            if (_hdr[8] == "Undefined") {
                QStringList chr = _hdr[9].split(",");
                if (chr.size() == 6) {
                    chroma_coord_curr_red_x = chr[0];
                    chroma_coord_curr_red_y = chr[1];
                    chroma_coord_curr_green_x = chr[2];
                    chroma_coord_curr_green_y = chr[3];
                    chroma_coord_curr_blue_x = chr[4];
                    chroma_coord_curr_blue_y = chr[5];
                } else {
                    _message = "Incorrect master display chroma coordinates source parameters!";
                    call_task_complete(_message, false);
                    return;
                };
                QStringList wht = _hdr[10].split(",");
                if (wht.size() == 2) {
                    white_coord_curr_x = wht[0];
                    white_coord_curr_y = wht[1];
                } else {
                    _message = "Incorrect master display white point coordinates source parameters!";
                    call_task_complete(_message, false);
                    return;
                };
            };
        };
        if (p18 == 1) {     // Display P3
            chroma_coord_curr_red_x = "0.680";
            chroma_coord_curr_red_y = "0.320";
            chroma_coord_curr_green_x = "0.265";
            chroma_coord_curr_green_y = "0.690";
            chroma_coord_curr_blue_x = "0.150";
            chroma_coord_curr_blue_y = "0.060";
            white_coord_curr_x = "0.3127";
            white_coord_curr_y = "0.3290";
        };
        if (p18 == 2) {     // DCI P3
            chroma_coord_curr_red_x = "0.680";
            chroma_coord_curr_red_y = "0.320";
            chroma_coord_curr_green_x = "0.265";
            chroma_coord_curr_green_y = "0.690";
            chroma_coord_curr_blue_x = "0.150";
            chroma_coord_curr_blue_y = "0.060";
            white_coord_curr_x = "0.314";
            white_coord_curr_y = "0.3510";
        };
        if (p18 == 3) {     // BT.2020
            chroma_coord_curr_red_x = "0.708";
            chroma_coord_curr_red_y = "0.292";
            chroma_coord_curr_green_x = "0.170";
            chroma_coord_curr_green_y = "0.797";
            chroma_coord_curr_blue_x = "0.131";
            chroma_coord_curr_blue_y = "0.046";
            white_coord_curr_x = "0.3127";
            white_coord_curr_y = "0.3290";
        };
        if (p18 == 4) {     // BT.709
            chroma_coord_curr_red_x = "0.640";
            chroma_coord_curr_red_y = "0.330";
            chroma_coord_curr_green_x = "0.30";
            chroma_coord_curr_green_y = "0.60";
            chroma_coord_curr_blue_x = "0.150";
            chroma_coord_curr_blue_y = "0.060";
            white_coord_curr_x = "0.3127";
            white_coord_curr_y = "0.3290";
        };
        if (p18 == 5) {     // Custom
            QStringList chr = p19.split(",");
            if (chr.size() == 6) {
                chroma_coord_curr_red_x = chr[0];
                chroma_coord_curr_red_y = chr[1];
                chroma_coord_curr_green_x = chr[2];
                chroma_coord_curr_green_y = chr[3];
                chroma_coord_curr_blue_x = chr[4];
                chroma_coord_curr_blue_y = chr[5];
            };
            QStringList wht = p20.split(",");
            if (wht.size() == 2) {
                white_coord_curr_x = wht[0];
                white_coord_curr_y = wht[1];
            };
        };

        if (chroma_coord_curr_red_x == "") {
            chroma_coord = "-d chromaticity-coordinates-red-x -d chromaticity-coordinates-red-y "
                           "-d chromaticity-coordinates-green-x -d chromaticity-coordinates-green-y "
                           "-d chromaticity-coordinates-blue-x -d chromaticity-coordinates-blue-y ";
        } else {
            chroma_coord = QString("-s chromaticity-coordinates-red-x=%1 -s chromaticity-coordinates-red-y=%2 "
                                   "-s chromaticity-coordinates-green-x=%3 -s chromaticity-coordinates-green-y=%4 "
                                   "-s chromaticity-coordinates-blue-x=%5 -s chromaticity-coordinates-blue-y=%6 ")
                                   .arg(chroma_coord_curr_red_x, chroma_coord_curr_red_y, chroma_coord_curr_green_x,
                                        chroma_coord_curr_green_y, chroma_coord_curr_blue_x, chroma_coord_curr_blue_y);
        };
        if (white_coord_curr_x == "") {
            white_coord = "-d white-coordinates-x -d white-coordinates-y ";
        } else {
            white_coord = QString("-s white-coordinates-x=%1 -s white-coordinates-y=%2 ").arg(white_coord_curr_x, white_coord_curr_y);
        };
    };

    _preset_0 = "-hide_banner" + hwaccel;
    _preset_pass1 = resize  + codec + preset + mode + pass1 + color_range + colorprim + colormatrix + transfer + "-an -f null /dev/null";
    _preset = resize + codec + preset + mode + pass + color_range + colorprim + colormatrix + transfer + acodec;
    _preset_mkvmerge = QString("%1%2%3%4%5%6 ").arg(max_cll, max_fall, max_lum, min_lum, chroma_coord, white_coord);
    std::cout << "Flag two-pass: " << _flag_two_pass << std::endl;
    std::cout << "Flag HDR: " << _flag_hdr << std::endl;
    std::cout << "preset_0: " << _preset_0.toStdString() << std::endl;
    if ((_flag_two_pass == true) && (_flag_hdr == true)) {
        std::cout << "preset_pass1: " << _preset_pass1.toStdString() << std::endl;
        std::cout << "preset: " << _preset.toStdString() << std::endl;
        std::cout << "preset_mkvpropedit: " << _preset_mkvmerge.toStdString() << std::endl;
    };
    if ((_flag_two_pass == true) && (_flag_hdr == false)) {
        std::cout << "preset_pass1: " << _preset_pass1.toStdString() << std::endl;
        std::cout << "preset: " << _preset.toStdString() << std::endl;
    };
    if ((_flag_two_pass == false) && (_flag_hdr == true)) {
        std::cout << "preset: " << _preset.toStdString() << std::endl;
        std::cout << "preset_mkvpropedit: " << _preset_mkvmerge.toStdString() << std::endl;
    };
    if ((_flag_two_pass == false) && (_flag_hdr == false)) {
        std::cout << "preset: " << _preset.toStdString() << std::endl;
    };
    encode();
}

void MainWindow::encode()   //***************************************** Encode **********************//
{
    std::cout << "Encode ..." << std::endl;  //  Debug info //
    QStringList arguments;
    _calling_pr_1 = true;
    process_1->disconnect();
    connect(process_1, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_1()));
    connect(process_1, SIGNAL(finished(int)), this, SLOT(error_1()));
    ui->progressBar->setValue(0);
    if (_mux_mode == true) {
        std::cout << "Muxing mode ..." << std::endl;  //  Debug info //
        ui->label_53->setText("Muxing:");
        arguments << "-hide_banner" << "-i" << _temp_file << "-map" << "0:0" << "-movflags" << "+write_colr"
                  << "-c:v" << "copy" << "-map" << "?0:a" << "-c:a" << "copy" << "-y" << _output_file;
    } else {
        int dur_mod = static_cast<int>(round(_dur));
        float fps_mod = _fps.toFloat();
        _fr_count = static_cast<int>(round(dur_mod * fps_mod));
        if (_fr_count == 0) {
            _status_encode_btn = "start";
            QIcon icon_start;
            icon_start.addFile(QString::fromUtf8(":/16x16/icons/16x16/cil-media-play.png"), QSize(), QIcon::Normal, QIcon::Off);
            ui->actionEncode->setIcon(icon_start);
            ui->actionEncode->setToolTip("Start");
            _message = "The file does not contain FPS information!\nSelect the correct input file!";
            call_task_complete(_message, false);
            return;
        };
        add_files->setEnabled(false);
        remove_files->setEnabled(false);
        select_preset->setEnabled(false);
        settings->setEnabled(false);
        ui->tableWidget->setEnabled(false);
        ui->actionAdd->setEnabled(false);
        ui->actionRemove->setEnabled(false);
        ui->actionPreset->setEnabled(false);
        ui->actionSettings->setEnabled(false);
        ui->label_53->show();
        ui->label_54->show();
        ui->label_55->show();
        ui->progressBar->show();
        _loop_start = time(nullptr);
        if (_flag_two_pass == false && _flag_hdr == false) {
            std::cout << "Encode non HDR..." << std::endl;  //  Debug info //
            ui->label_53->setText("Encoding:");
            arguments << _preset_0.split(" ") << "-i" << _input_file << _preset.split(" ") << "-y" << _output_file;
        };
        if (_flag_two_pass == false && _flag_hdr == true) {
            std::cout << "Encode HDR..." << std::endl;  //  Debug info //
            ui->label_53->setText("Encoding:");
            arguments << _preset_0.split(" ") << "-i" << _input_file << _preset.split(" ") << "-y" << _temp_file;
        };
        if (_flag_two_pass == true) {
            std::cout << "Encode 1-st pass..." << std::endl;  //  Debug info //
            ui->label_53->setText("1-st pass:");
            arguments << _preset_0.split(" ") << "-y" << "-i" << _input_file << _preset_pass1.split(" ");
        };
    };
    //qDebug() << arguments;
    process_1->start("ffmpeg", arguments);
    if (!process_1->waitForStarted()) {
        std::cout << "cmd command not found!!!" << std::endl;
        process_1->disconnect();
        restore_initial_state();
        _message = "An unknown error occurred!\n Possible FFMPEG not installed.\n";
        call_task_complete(_message, false);
    };
}

void MainWindow::add_metadata() //********************************** Add metedata *******************//
{
    std::cout << "Add metadata ..." << std::endl;  //  Debug info //
    _calling_pr_1 = true;
    process_1->disconnect();
    connect(process_1, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_2()));
    connect(process_1, SIGNAL(finished(int)), this, SLOT(error_1()));
    ui->label_53->setText("Add data:");
    ui->progressBar->setValue(0);
    QStringList arguments;
    arguments << "--edit" << "track:1" << _preset_mkvmerge.split(" ") << _temp_file;
    process_1->start("mkvpropedit", arguments);
    if (!process_1->waitForStarted()) {
        std::cout << "cmd command not found!!!" << std::endl;
        process_1->disconnect();
        restore_initial_state();
        _message = "An unknown error occured!\n Possible mkvtoolnix not installed.\n";
        call_task_complete(_message, false);
    };
}

void MainWindow::complete() //**************************************** Complete *********************//
{
    std::cout << "Complete ..." << std::endl;  //  Debug info //
    process_1->disconnect();
    QTableWidgetItem *newItem_status = new QTableWidgetItem("Done!");
    ui->tableWidget->setItem(_row, 6, newItem_status);
    if (_flag_hdr == true) {
        QDir().remove(_temp_file);
    };
    if (_batch_mode == true) {
        int row = ui->tableWidget->currentRow();
        int numRows = ui->tableWidget->rowCount();
        if (numRows > (row + 1)) {
            ui->tableWidget->selectRow(row + 1);
            make_preset();
        } else {
            restore_initial_state();
            time_t end_t = time(nullptr);
            int elps_t = static_cast<int>(end_t - _strt_t);
            if (elps_t < 0) {
                elps_t = 0;
            };
            int h = static_cast<int>(trunc(elps_t / 3600));
            int m = static_cast<int>(trunc((elps_t - (h * 3600)) / 60));
            int s = static_cast<int>(trunc(elps_t - (h * 3600) - (m * 60)));
            QString hrs = QString::number(h);
            QString min = QString::number(m);
            QString sec = QString::number(s);
            std::ostringstream sstr;
            sstr << std::setw(2) << std::setfill('0') << hrs.toStdString() << ":"
                 << std::setw(2) << std::setfill('0') << min.toStdString() << ":"
                 << std::setw(2) << std::setfill('0') << sec.toStdString();
            std::string tm = sstr.str();
            if (_protection == true) {
                timer->stop();
            }
            _message = "Task completed!\n\n Elapsed time: " + QString::fromStdString(tm);
            call_task_complete(_message, false);
        };
    } else {
        restore_initial_state();
        time_t end_t = time(nullptr);
        int elps_t = static_cast<int>(end_t - _strt_t);
        if (elps_t < 0) {
            elps_t = 0;
        };
        int h = static_cast<int>(trunc(elps_t / 3600));
        int m = static_cast<int>(trunc((elps_t - (h * 3600)) / 60));
        int s = static_cast<int>(trunc(elps_t - (h * 3600) - (m * 60)));
        QString hrs = QString::number(h);
        QString min = QString::number(m);
        QString sec = QString::number(s);
        std::ostringstream sstr;
        sstr << std::setw(2) << std::setfill('0') << hrs.toStdString() << ":"
             << std::setw(2) << std::setfill('0') << min.toStdString() << ":"
             << std::setw(2) << std::setfill('0') << sec.toStdString();
        std::string tm = sstr.str();
        if (_protection == true) {
            timer->stop();
        }
        _message = "Task completed!\n\n Elapsed time: " + QString::fromStdString(tm);
        call_task_complete(_message, false);
    };
}

void MainWindow::progress_1()   //*********************************** Progress 1 ********************//
{
    QString line = process_1->readAllStandardOutput();
    QString line_mod6 = line.replace("   ", " ").replace("  ", " ").replace("  ", " ").replace("= ", "=");
    std::cout << line_mod6.toStdString() << std::endl;
    int pos_err = line_mod6.indexOf("[error]:");
    if (pos_err != -1){
        QStringList error = line_mod6.split(":");
        _error_message = error[1];
    };
    int pos_st = line_mod6.indexOf("frame=");
    if (pos_st == 0) {
        QStringList data = line_mod6.split(" ");
        QString frame_qstr = data[0].replace("frame=", "");
        int frame = frame_qstr.toInt();
        time_t iter_start = time(nullptr);
        int timer = static_cast<int>(iter_start - _loop_start);
        float full_time = static_cast<float>(timer * _fr_count) / frame;
        int rem_time = int(full_time) - timer;
        if (rem_time < 0) {
            rem_time = 0;
        };
        int h = static_cast<int>(trunc(rem_time / 3600));
        int m = static_cast<int>(trunc((rem_time - (h * 3600)) / 60));
        int s = static_cast<int>(trunc(rem_time - (h * 3600) - (m * 60)));
        float percent = 0.9f + static_cast<float>(frame * 100) / _fr_count;
        if (percent > 100.f) {
            percent = 100.f;
        };
        ui->progressBar->setValue(int(round(percent)));
        QString hrs = QString::number(h);
        QString min = QString::number(m);
        QString sec = QString::number(s);
        std::ostringstream sstr;
        sstr << std::setw(2) << std::setfill('0') << hrs.toStdString() << ":"
             << std::setw(2) << std::setfill('0') << min.toStdString() << ":"
             << std::setw(2) << std::setfill('0') << sec.toStdString();
        std::string tm = sstr.str();
        ui->label_55->setText(QString::fromStdString(tm));
        if ((percent >= 50) && (_calling_pr_1 == true)) {
             disconnect(process_1, SIGNAL(finished(int)), this, SLOT(error_1()));
             if (_mux_mode == true) {
                 connect(process_1, SIGNAL(finished(int)), this, SLOT(complete()));
             } else {
                 if (_flag_two_pass == false && _flag_hdr == true) {
                     disconnect(process_1, SIGNAL(finished(int)), this, SLOT(encode()));
                     connect(process_1, SIGNAL(finished(int)), this, SLOT(add_metadata()));
                 };
                 if (_flag_two_pass == false && _flag_hdr == false) {
                     disconnect(process_1, SIGNAL(finished(int)), this, SLOT(encode()));
                     connect(process_1, SIGNAL(finished(int)), this, SLOT(complete()));
                 };
                 if (_flag_two_pass == true) {
                     connect(process_1, SIGNAL(finished(int)), this, SLOT(encode()));
                     _flag_two_pass = false;
                 };
             };
             _calling_pr_1 = false;
        };
    };
}

void MainWindow::progress_2()   //*********************************** Progress 2 ********************//
{
    QString line = process_1->readAllStandardOutput();
    int pos_st = line.indexOf("Done.");
    int pos_nf = line.indexOf("Nothing to do.");
    if ((pos_st != -1) or (pos_nf != -1)) {
        int percent = 100;
        ui->progressBar->setValue(percent);
        if ((percent == 100) && (_calling_pr_1 == true)) {
            disconnect(process_1, SIGNAL(finished(int)), this, SLOT(error_1()));
            _mux_mode = true;
            _loop_start = time(nullptr);
            _calling_pr_1 = false;
            connect(process_1, SIGNAL(finished(int)), this, SLOT(encode()));
        };
    };
}

void MainWindow::on_actionPreset_clicked()  // ****************** Call Preset Window *************** //
{
    SelectPreset select_preset(this);
    select_preset.setParameters(&_pos_top, &_pos_cld, &_theme);
    select_preset.setModal(true);
    select_preset.exec();  // ************************ Call preset window and wait for return ******** //
    if (_row != -1) {
        get_output_filename();
    };
}

void MainWindow::on_actionEncode_clicked()  // ********************* Encode button ***************** //
{
    if (_status_encode_btn == "start") {
        std::cout << "Status encode btn: start" << std::endl;  // Debug info //
        int cnt = ui->tableWidget->rowCount();
        if (cnt == 0) {
            _message = "Select input file first!";
            call_task_complete(_message, false);
            return;
        };
        if (_cur_param[0] == "") {
            _message = "Select preset first!";
            call_task_complete(_message, false);
            return;
        };
        _status_encode_btn = "pause";
        QIcon icon_pause;
        icon_pause.addFile(QString::fromUtf8(":/16x16/icons/16x16/cil-media-pause.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->actionEncode->setIcon(icon_pause);
        ui->actionEncode->setToolTip("Pause");
        _strt_t = time(nullptr);
        if (_protection == true) {
            timer->start();
        }
        make_preset();
        return;
    };
    if (_status_encode_btn == "pause") {
        std::cout << "Status encode btn: pause" << std::endl;  // Debug info //
        pause();
        _status_encode_btn = "resume";
        QIcon icon_resume;
        icon_resume.addFile(QString::fromUtf8(":/16x16/icons/16x16/cil-chevron-double-right.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->actionEncode->setIcon(icon_resume);
        ui->actionEncode->setToolTip("Resume");
        return;
    };
    if (_status_encode_btn == "resume") {
        std::cout << "Status encode btn: resume" << std::endl;  // Debug info //
        resume();
        _status_encode_btn = "pause";
        QIcon icon_pause;
        icon_pause.addFile(QString::fromUtf8(":/16x16/icons/16x16/cil-media-pause.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->actionEncode->setIcon(icon_pause);
        ui->actionEncode->setToolTip("Pause");
        return;
    };
}

void MainWindow::pause()    //***************************************** Pause ***********************//
{
    if (_protection == true) {
        timer->stop();
    }
    short s1 = process_1->state();
    qint64 pr1 = process_1->processId();
    std::cout << "State procedure_1: " << s1 << " PID: " << pr1 << std::endl;
    if (s1 != 0) {
        //_PROCESS_INFORMATION *pi = process_1->pid();  // pause for Windows
        //SuspendThread(pi->hThread);  // pause for Windows
        kill(pid_t(process_1->processId()), SIGSTOP);  // pause for Unix
    };
}

void MainWindow::resume()   //***************************************** Resume **********************//
{
    if (_protection == true) {
        timer->start();
    }
    short s1 = process_1->state();
    qint64 pr1 = process_1->processId();
    std::cout << "State procedure_1: " << s1 << " PID: " << pr1 << std::endl;
    if (s1 != 0) {
        //_PROCESS_INFORMATION *pi = process_1->pid();  // pause for Windows
        //ResumeThread(pi->hThread);  // pause for Windows
        kill(pid_t(process_1->processId()), SIGCONT); // resume for Unix
    };
}

void MainWindow::on_actionAbout_clicked()   //************************* About ***********************//
{
    About about(this);
    about.setModal(true);
    about.setParameters(_theme);
    about.exec();
}

void MainWindow::on_actionDonate_clicked()   //************************ Donate **********************//
{
    Donate donate(this);
    donate.setModal(true);
    donate.setParameters(_theme);
    donate.exec();
}

void MainWindow::on_actionSettings_clicked()    //******************** Settings *********************//
{
    Settings settings(this);
    settings.setParameters(&_batch_mode, &_stn_file, &_output_folder, &_temp_folder, &_protection, &_showHDR_mode, &_timer_interval, &_theme);
    settings.setModal(true);
    settings.exec();
    timer->setInterval(_timer_interval*1000);
    setTheme(_theme);
}

void MainWindow::on_actionStop_clicked()    //************************** Stop ***********************//
{
    std::cout << "Call Stop ..." << std::endl;  //  Debug info //
    short s1 = process_1->state();
    if (s1 != 0) {
        /*QMessageBox msgBox(this);
        msgBox.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(5, 30, 35);");
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setWindowTitle("Cine Encoder");
        msgBox.setText("Stop?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int confirm = msgBox.exec();
        if (confirm == QMessageBox::Yes) {*/
        _message = "Stop encoding?";
        bool confirm = call_dialog(_message);
        if (confirm == true) {
            process_1->disconnect();
            connect(process_1, SIGNAL(finished(int)), this, SLOT(cancel()));
            process_1->kill();
        }
    }
}

void MainWindow::cancel()   //************************************** Stop execute *******************//
{
    std::cout << "Stop execute ..." << std::endl;  //  Debug info //
    if (_protection == true) {
        timer->stop();
    }
    process_1->disconnect();
    restore_initial_state();
    _message = "The current encoding process has been canceled!\n";
    call_task_complete(_message, false);
}

void MainWindow::error_1()  //***************************************** Error ***********************//
{
    std::cout << "Error_1 ..." << std::endl;  //  Debug info //
    if (_protection == true) {
        timer->stop();
    }
    process_1->disconnect();
    QTableWidgetItem *newItem_status = new QTableWidgetItem("Error!");
    ui->tableWidget->setItem(_row, 6, newItem_status);
    restore_initial_state();
    _message = "An error occured!\n" + _error_message;
    call_task_complete(_message, false);
}

void MainWindow::get_current_data() //**************************** Get current data *****************//
{
    _dur = 0.0f;
    _stream_size = "";
    _width = "";
    _height = "";
    _fmt = "";
    _fps = "";
    _dur = (ui->tableWidget->item(_row, 21)->text()).toFloat();
    _stream_size = ui->tableWidget->item(_row, 24)->text();
    _width = ui->tableWidget->item(_row, 25)->text();
    _height = ui->tableWidget->item(_row, 26)->text();
    _fmt = ui->tableWidget->item(_row, 1)->text();
    _fps = ui->tableWidget->item(_row, 4)->text();
    _hdr[0] = ui->tableWidget->item(_row, 11)->text();  // color range
    _hdr[1] = ui->tableWidget->item(_row, 12)->text();  // color primary
    _hdr[2] = ui->tableWidget->item(_row, 13)->text(); // color matrix
    _hdr[3] = ui->tableWidget->item(_row, 14)->text();  // transfer
    _hdr[4] = ui->tableWidget->item(_row, 15)->text();  // max lum
    _hdr[5] = ui->tableWidget->item(_row, 16)->text();  // min lum
    _hdr[6] = ui->tableWidget->item(_row, 17)->text();  // max cll
    _hdr[7] = ui->tableWidget->item(_row, 18)->text();  // max fall
    _hdr[8] = ui->tableWidget->item(_row, 19)->text();  // master display
    _hdr[9] = ui->tableWidget->item(_row, 22)->text();  // chr coord
    _hdr[10] = ui->tableWidget->item(_row, 23)->text();  // white coord
    _input_file = (ui->tableWidget->item(_row, 20)->text()) + "/" + (ui->tableWidget->item(_row, 0)->text());
    QString tmb_name = (ui->tableWidget->item(_row, 0)->text()).replace(".", "_").replace(" ", "_");
    const QString tmb_file = _thumb_path + "/" + tmb_name + ".png";
    QFile tmb;
    tmb.setFileName(tmb_file);
    if (!tmb.exists()) {
        std::cout<< "Thumbnail file not exist and created..." << std::endl;
        QStringList cmd;
        cmd << "-hide_banner" << "-ss" << "4" << "-i" << _input_file << "-vf" << "scale=144:-1" << "-vframes" << "1" << "-q:v" << "3" << "-y" << tmb_file;
        process_5->start("ffmpeg", cmd);
        process_5->waitForFinished();
    };
    ui->labelThumb->setPixmap(QPixmap(tmb_file));
    ui->label_source->setText(ui->tableWidget->item(_row, 0)->text());
    QString a, b, c, d, e, f, g, h, aud = "";
    a = ui->tableWidget->item(_row, 1)->text();
    b = ui->tableWidget->item(_row, 2)->text();
    c = ui->tableWidget->item(_row, 4)->text();
    d = ui->tableWidget->item(_row, 5)->text();
    e = ui->tableWidget->item(_row, 7)->text();
    f = ui->tableWidget->item(_row, 8)->text();
    g = ui->tableWidget->item(_row, 9)->text();
    h = ui->tableWidget->item(_row, 10)->text();
    QString opt = a + ", " + b + ", " + c + " fps, " + d + ", " + h + ", " + f + ", " + g + " bit, " + e + " kbps; ";
    int q = 0;
    while (q <= 8 ) {
        aud = ui->tableWidget->item(_row, q + 27)->text();
        if (aud == "") {
            break;
        } else {
            opt = opt + "Audio #" + QString::number(q+1) + ": ";
            opt = opt + aud + "; ";
        };
        q++;
    }
    if (a == "Undef") {
        opt = "Undefined";
    }
    ui->textBrowser_1->clear();
    ui->textBrowser_1->setText(opt);
    get_output_filename();
}

void MainWindow::get_output_filename()  //************************ Get output data ******************//
{
    ui->textBrowser_2->clear();
    ui->textBrowser_2->setText(_cur_param[0]);
    QString file_name = ui->tableWidget->item(_row, 0)->text();
    QString file_without_ext = "";
    QString prefix = "";
    QString suffix = "";
    QString arr[12][5] = {
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"WebM", "MKV", "",    "",     ""},
        {"WebM", "MKV", "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"}
    };
    prefix = arr[_cur_param[1].toInt()][_cur_param[3].toInt()].toLower();
    //QTime ct = QTime::currentTime();
    //QString ct_qstr = ct.toString();
    suffix = "_encoded_00" + QString::number(_row) + ".";
    std::wstring file_name_wstr = file_name.toStdWString();
    unsigned long sep = file_name_wstr.rfind('.');
    std::wstring file_name_without_ext_wstr = file_name_wstr.substr(0, sep);
    file_without_ext = QString::fromStdWString(file_name_without_ext_wstr);
    QString _output_file_name = file_without_ext + suffix + prefix;
    ui->label_output->setText(_output_file_name);
    if (_output_folder == "") {
        _output_file = (ui->tableWidget->item(_row, 20)->text()) + "/" + _output_file_name;
    } else {
        _output_file = _output_folder + "/" + _output_file_name;
    };
    if (_temp_folder == "") {
        _temp_file = (ui->tableWidget->item(_row, 20)->text()) + "/temp.mkv";
    } else {
        _temp_file = _temp_folder + "/temp.mkv";
    };
}

void MainWindow::set_defaults() //****************************** Set default presets ****************//
{
    std::cout<< "Set defaults..." << std::endl;
    QStringList line;
    line << "HEVC, Res: Source, Fps: Source, YUV, 4:2:0, 10 bit, VBR 45M, Preset: Slow, 2 Pass, HDR: Enabled, Audio: AC3, 448k, MP4"
            "<%>0<%>2<%>2<%>45M<%>50M<%>50M<%>1<%>0<%>Source<%>Source<%>1<%>6<%>0<%><%><%><%><%>0<%>From source<%>From source<%>1<%>1<%>0<%>0<%>"

         << "High Quality H265 4K HDR 10 bit<&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&>TopLewelItem<&>"
         << "HEVC, Res: Source, Fps: Source, YUV, 4:2:0, 10 bit, VBR 45M, Preset: Slow, 2 Pass, HDR: Enabled, Audio: AC3, 448k, MP4"
            "<&>0<&>2<&>2<&>45M<&>50M<&>50M<&>1<&>0<&>Source<&>Source<&>1<&>6<&>0<&><&><&><&><&>0<&>From source<&>From source<&>1<&>1<&>ChildItem<&>"
         << "HEVC, Res: Source, Fps: Source, YUV, 4:2:0, 10 bit, CRF 23, Preset: Slow, 1 Pass, HDR: Enabled, Audio: AC3, 448k, MP4"
            "<&>0<&>3<&>2<&>23<&><&><&>1<&>0<&>Source<&>Source<&>0<&>6<&>0<&><&><&><&><&>0<&>From source<&>From source<&>1<&>1<&>ChildItem<&>"
         << "HEVC, NVENC, Res: Source, Fps: Source, YUV, 4:2:0, 10 bit, VBR 45M, Preset: Slow, 2 Pass, HDR: Enabled, Audio: AC3, 448k, MP4"
            "<&>3<&>0<&>2<&>45M<&>50M<&>50M<&>1<&>0<&>Source<&>Source<&>0<&>0<&>0<&><&><&><&><&>0<&>From source<&>From source<&>1<&>1<&>ChildItem<&>"

         << "High Quality H265 4K 8 bit<&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&>TopLewelItem<&>"
         << "HEVC, Res: Source, Fps: Source, YUV, 4:2:0, 8 bit, VBR 45M, Preset: Slow, 2 Pass, Audio: AC3, 448k, MP4<&>1<&>2<&>2<&>45M"
            "<&>50M<&>50M<&>1<&>0<&>Source<&>Source<&>1<&>6<&>0<&><&><&><&><&>0<&>From source<&>From source<&>1<&>1<&>ChildItem<&>"
         << "HEVC, Res: Source, Fps: Source, YUV, 4:2:0, 8 bit, CRF 23, Preset: Slow, 1 Pass, Audio: AC3, 448k, MP4<&>1<&>3<&>2<&>23"
            "<&><&><&>1<&>0<&>Source<&>Source<&>0<&>6<&>0<&><&><&><&><&>0<&>From source<&>From source<&>1<&>1<&>ChildItem<&>"
         << "HEVC, NVENC, Res: Source, Fps: Source, YUV, 4:2:0, 8 bit, VBR 45M, Preset: Slow, 2 Pass, Audio: AC3, 448k, MP4<&>4<&>0<&>"
            "2<&>45M<&>50M<&>50M<&>1<&>0<&>Source<&>Source<&>0<&>0<&>0<&><&><&><&><&>0<&>From source<&>From source<&>1<&>1<&>ChildItem<&>"

         << "High Quality H264 4K 8 bit<&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&>TopLewelItem<&>"
         << "AVC, Res: Source, Fps: Source, YUV, 4:2:0, 8 bit, VBR 60M, Preset: Slow, 2 Pass, Audio: AC3, 448k, MP4<&>2<&>2<&>2<&>60M"
            "<&>65M<&>65M<&>1<&>0<&>Source<&>Source<&>1<&>6<&>0<&><&><&><&><&>0<&>From source<&>From source<&>1<&>1<&>ChildItem<&>"
         << "AVC, Res: Source, Fps: Source, YUV, 4:2:0, 8 bit, CRF 23, Preset: Slow, 1 Pass, Audio: AC3, 448k, MP4<&>2<&>3<&>2<&>23<&>"
            "<&><&>1<&>0<&>Source<&>Source<&>0<&>6<&>0<&><&><&><&><&>0<&>From source<&>From source<&>1<&>1<&>ChildItem<&>"
         << "AVC, NVENC,  Res: Source, Fps: Source, YUV, 4:2:0, 8 bit, VBR 45M, Preset: Slow, 2 Pass, Audio: AC3, 448k, MP4<&>5<&>0<&>2"
            "<&>45M<&>50M<&>50M<&>1<&>0<&>Source<&>Source<&>0<&>0<&>0<&><&><&><&><&>0<&>From source<&>From source<&>1<&>1<&>ChildItem<&>"

         << "Web High Quality VP9 4K HDR 10 bit<&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&>TopLewelItem<&>"
         << "VP9, Res: Source, Fps: Source, YUV, 4:2:0, 10 bit, ABR 30M, 2 Pass, HDR: Enabled, Audio: Opus, 256k, WebM<&>6<&>0<&>0<&>"
            "30M<&><&><&>1<&>0<&>Source<&>Source<&>1<&>0<&>0<&><&><&><&><&>0<&>From source<&>From source<&>0<&>2<&>ChildItem<&>"
         << "VP9, Res: Source, Fps: Source, YUV, 4:2:0, 10 bit, CRF 23, 1 Pass, HDR: Enabled, Audio: Opus, 256k, WebM<&>6<&>1<&>0<&>23"
            "<&><&><&>1<&>0<&>Source<&>Source<&>0<&>0<&>0<&><&><&><&><&>0<&>From source<&>From source<&>0<&>2<&>ChildItem<&>"

         << "Studio Quality 4K HDR 10 bit<&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&><&>TopLewelItem<&>"
         << "ProRes HQ, Res: Source, Fps: Source, YUV, 4:2:2, 10 bit, HDR: Enabled, Audio: PCM 16 bit, MOV<&>8<&>0<&>0<&>Auto<&>Auto<&>"
            "Auto<&>0<&>0<&>Source<&>Source<&>0<&>0<&>0<&><&><&><&><&>0<&>From source<&>From source<&>0<&>0<&>ChildItem<&>"
         << "ProRes 4444, Res: Source, Fps: Source, YUV, 4:4:4, 10 bit, HDR: Enabled, Audio: PCM 16 bit, MOV<&>9<&>0<&>0<&>Auto<&>Auto<&>"
            "Auto<&>0<&>0<&>Source<&>Source<&>0<&>0<&>0<&><&><&><&><&>0<&>From source<&>From source<&>0<&>0<&>ChildItem<&>"
         << "DNxHR HQX, Res: Source, Fps: Source, YUV, 4:2:2, 10 bit, HDR: Enabled, Audio: PCM 16 bit, MOV<&>10<&>0<&>0<&>Auto<&>Auto<&>"
            "Auto<&>0<&>0<&>Source<&>Source<&>0<&>0<&>0<&><&><&><&><&>0<&>From source<&>From source<&>0<&>0<&>ChildItem<&>";
    QStringList cur_param = line[0].split("<%>");
    for (int k = 0; k < 23; k++) {
        _cur_param[k] = cur_param[k];
    };
    _pos_top = cur_param[23].toInt();
    _pos_cld = cur_param[24].toInt();
    int n = line.size() - 1;
    for (int i = 0; i < 24; i++) {
      _preset_table[i].resize(n);
    };
    for (int j = 1; j <= n; j++) {
        cur_param = line[j].split("<&>");
        for (int m = 0; m < 24; m++) {
            _preset_table[m][j-1] = cur_param[m];
        };
    };
}

void MainWindow::restore_initial_state()    //***************** Restore initial state ***************//
{
    add_files->setEnabled(true);
    remove_files->setEnabled(true);
    select_preset->setEnabled(true);
    settings->setEnabled(true);
    ui->tableWidget->setEnabled(true);
    ui->actionAdd->setEnabled(true);
    ui->actionRemove->setEnabled(true);
    ui->actionPreset->setEnabled(true);
    ui->actionSettings->setEnabled(true);
    _status_encode_btn = "start";
    QIcon icon_start;
    icon_start.addFile(QString::fromUtf8(":/16x16/icons/16x16/cil-media-play.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->actionEncode->setIcon(icon_start);
    ui->actionEncode->setToolTip("Start");
}

bool MainWindow::call_dialog(const QString &_message)  // Call dialog ******//
{
    bool acceptFlag = false;
    Dialog dialog(this);
    dialog.setMessage(_message, &acceptFlag, _theme);
    dialog.setModal(true);
    dialog.exec();
    return acceptFlag;
}

void MainWindow::call_task_complete(const QString &_message, const bool &_timer_mode)  // Call task complete ******//
{
    Taskcomplete taskcomplete(this);
    taskcomplete.setMessage(_message, _timer_mode, _theme);
    taskcomplete.setModal(true);
    taskcomplete.exec();
}

void MainWindow::setTheme(int &ind_theme)   //********************** Set theme **********************//
{
    QFile file;
    switch (ind_theme)
    {
        case 0:
        {
            file.setFileName(":/style_default.css");
            ui->frame_main->setStyleSheet("background-color: rgb(5, 20, 28);");
            ui->labelThumb->setStyleSheet("color: rgb(210, 210, 210); background-color: rgba(27, 29, 35, 50);");
            ui->frame_source->setStyleSheet("background-color: rgba(27, 29, 35, 50);");
            ui->frame_output->setStyleSheet("background-color: rgba(27, 29, 35, 50);");
            ui->textBrowser_1->setStyleSheet("color: rgb(210, 210, 210); background-color: transparent; border: 1px solid rgba(27, 29, 35, 0);");
            ui->textBrowser_2->setStyleSheet("color: rgb(210, 210, 210); background-color: transparent; border: 1px solid rgba(27, 29, 35, 0);");
            ui->line_1->setStyleSheet("background-color: rgba(100, 100, 168, 50);");
            ui->line_2->setStyleSheet("background-color: rgba(100, 100, 168, 50);");
        }; break;
        case 1:
        {
            file.setFileName(":/style_deep.css");
            ui->frame_main->setStyleSheet("background-color: rgb(3, 3, 5);");
            ui->labelThumb->setStyleSheet("color: rgb(210, 210, 210); background-color: rgba(27, 29, 35, 150);");
            ui->frame_source->setStyleSheet("background-color: rgba(27, 29, 35, 150);");
            ui->frame_output->setStyleSheet("background-color: rgba(27, 29, 35, 150);");
            ui->textBrowser_1->setStyleSheet("color: rgb(210, 210, 210); background-color: transparent; border: 1px solid rgba(27, 29, 35, 0);");
            ui->textBrowser_2->setStyleSheet("color: rgb(210, 210, 210); background-color: transparent; border: 1px solid rgba(27, 29, 35, 0);");
            ui->line_1->setStyleSheet("background-color: rgb(200, 200, 200);");
            ui->line_2->setStyleSheet("background-color: rgb(200, 200, 200);");
        }; break;
        case 2:
        {
            file.setFileName(":/style_wave.css");
            ui->frame_main->setStyleSheet("background-color: rgb(39, 44, 54);");
            ui->labelThumb->setStyleSheet("color: rgb(210, 210, 210); background-color: rgba(37, 42, 52, 150);");
            ui->frame_source->setStyleSheet("background-color: rgba(37, 42, 52, 150);");
            ui->frame_output->setStyleSheet("background-color: rgba(37, 42, 52, 150);");
            ui->textBrowser_1->setStyleSheet("color: rgb(210, 210, 210); background-color: transparent; border: 1px solid rgba(27, 29, 35, 0);");
            ui->textBrowser_2->setStyleSheet("color: rgb(210, 210, 210); background-color: transparent; border: 1px solid rgba(27, 29, 35, 0);");
            ui->line_1->setStyleSheet("background-color: rgb(200, 200, 200);");
            ui->line_2->setStyleSheet("background-color: rgb(200, 200, 200);");
        }; break;
        case 3:
        {
            file.setFileName(":/style_white.css");
            ui->frame_main->setStyleSheet("background-color: rgb(160, 160, 160);");
            ui->labelThumb->setStyleSheet("color: rgb(3, 3, 5); background-color: rgba(200, 200, 205, 150);");
            ui->frame_source->setStyleSheet("background-color: rgba(200, 200, 205, 150);");
            ui->frame_output->setStyleSheet("background-color: rgba(200, 200, 205, 150);");
            ui->textBrowser_1->setStyleSheet("color: rgb(3, 3, 5); background-color: transparent; border: 1px solid rgba(27, 29, 35, 0);");
            ui->textBrowser_2->setStyleSheet("color: rgb(3, 3, 5); background-color: transparent; border: 1px solid rgba(27, 29, 35, 0);");
            ui->line_1->setStyleSheet("background-color: rgb(20, 20, 20);");
            ui->line_2->setStyleSheet("background-color: rgb(20, 20, 20);");
        }; break;
    }
    file.open(QFile::ReadOnly);
    QString list = file.readAll();
    ui->frame_1->setStyleSheet(list);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    int i = 11;
    if (!_showHDR_mode)
    {
        while (i <= 19) {
            ui->tableWidget->hideColumn(i);
            i++;
        }
    } else {
        while (i <= 19) {
            ui->tableWidget->showColumn(i);
            i++;
        }
    }
}

void MainWindow::repeat_handler()   //**************************** Repeat handler *******************//
{
    std::cout<< "Call by timer..." << std::endl;
    on_actionEncode_clicked();
    call_task_complete("Pause", true);
    on_actionEncode_clicked();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
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
            return QMainWindow::eventFilter(watched, event);
        }
    }

    if (watched == ui->centralwidget) // *************** Resize window realisation ************************* //
    {
        if (!this->isMaximized())
        {
            if (event->type() == QEvent::MouseButtonPress)
            {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->button() == Qt::LeftButton)
                {
                    curWidth = this->width();
                    curHeight = this->height();
                    mouseClickCoordinate = mouse_event->pos();
                    if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() > 62) && (mouseClickCoordinate.y() < (curHeight-12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                        clickPressed_Left_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() < 12))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                        clickPressed_Left_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > 12) && (mouseClickCoordinate.x() < (curWidth - 120)) && (mouseClickCoordinate.y() < 3))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                        clickPressed_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (curWidth - 12)) && (mouseClickCoordinate.y() < 12))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                        clickPressed_Right_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (curWidth - 12)) && (mouseClickCoordinate.y() > 62) && (mouseClickCoordinate.y() < (curHeight-12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                        clickPressed_Right_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (curWidth - 12)) && (mouseClickCoordinate.y() > (curHeight - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                        clickPressed_Right_Bottom_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > 12) && (mouseClickCoordinate.x() < (curWidth - 12)) && (mouseClickCoordinate.y() > (curHeight - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                        clickPressed_Bottom_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() > (curHeight - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                        clickPressed_Left_Bottom_ResizeFlag = true;
                    }
                    return QMainWindow::eventFilter(watched, event);
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
                        this->setGeometry(deltaX, this->pos().y(), this->width() - deltaWidth, curHeight);
                    }
                    else if (clickPressed_Left_Top_ResizeFlag == true)
                    {
                        this->setGeometry(deltaX, deltaY, this->width() - deltaWidth, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Top_ResizeFlag == true)
                    {
                        this->setGeometry(this->pos().x(), deltaY, curWidth, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Right_Top_ResizeFlag == true)
                    {
                        this->setGeometry(this->pos().x(), deltaY, curWidth + deltaWidth, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Right_ResizeFlag == true)
                    {
                        this->setGeometry(this->pos().x(), this->pos().y(), curWidth + deltaWidth, curHeight);
                    }
                    else if (clickPressed_Right_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(this->pos().x(), this->pos().y(), curWidth + deltaWidth, curHeight + deltaHeight);
                    }
                    else if (clickPressed_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(this->pos().x(), this->pos().y(), curWidth, curHeight + deltaHeight);
                    }
                    else if (clickPressed_Left_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(deltaX, this->pos().y(), this->width() - deltaWidth, curHeight + deltaHeight);
                    }
                    return QMainWindow::eventFilter(watched, event);
                }
            }
        }
    }
    else if (watched == ui->frame_hint) // *************** Drag window realisation ************************* //
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton)
            {
                mouseClickCoordinate = mouse_event->pos();
                clickPressedFlag = true;
                return QMainWindow::eventFilter(watched, event);
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
                return QMainWindow::eventFilter(watched, event);
            }
        }
        else if (event->type() == QEvent::MouseButtonDblClick)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                on_expandWindow_clicked();
                return QMainWindow::eventFilter(watched, event);
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}
