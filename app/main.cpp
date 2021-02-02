#include <QApplication>
#include <QSplashScreen>
//#include <QPainter>
#include <QElapsedTimer>
#include "mainwindow.h"


static const int LOAD_TIME_MSEC = 1200;
QString _cur_param[23];
QVector <QVector <QString> > _preset_table;


/*void message(const QString &path, QSplashScreen *&splash)
{
    QByteArray path_ba = path.toLocal8Bit();
    const char *path_ch = path_ba.data();

    splash->showMessage (
        QObject::tr(path_ch),
        Qt::AlignBottom | Qt::AlignLeft
    );
}*/


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //app.setWindowIcon(QIcon(":/icons/cine-encoder.ico"));

    QPixmap pixmap(":/icons/images/splash.png");
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();
    app.processEvents();

    /*foreach (const QString &path, app.libraryPaths()) {
        message(path, splash);
        QCoreApplication::processEvents();
    }*/

    QElapsedTimer time;
    time.start();
    while (time.elapsed() < LOAD_TIME_MSEC)
    {
        /*const int progress = 1 + static_cast< double >(time.elapsed()) / LOAD_TIME_MSEC * 100.0;
        splash->showMessage(
            QObject::tr("Loading... %1%").arg(progress),
            Qt::AlignBottom | Qt::AlignRight
        );*/

        /*QPainter painter;
        painter.begin(&pix);
        painter.fillRect(
            PROGRESS_X_PX,
            PROGRESS_Y_PX,
            progress / 100.0 * PROGRESS_WIDTH_PX,
            PROGRESS_HEIGHT_PX, Qt::gray);
        painter.end();
        splash->setPixmap(pixmap);*/
        app.processEvents();
    }

    MainWindow window;
    window.show();
    splash->finish(&window);
    delete splash;
    return app.exec();
}
