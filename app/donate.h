#ifndef DONATE_H
#define DONATE_H

#include <QDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QMouseEvent>

namespace Ui
{
class Donate;
}


class Donate : public QDialog
{
    Q_OBJECT

public:

    explicit Donate(QWidget *parent = nullptr);

    ~Donate();

    void setParameters(int &ind_theme);

private slots:

    bool eventFilter(QObject *watched, QEvent *event);

    void on_toolButton_6_clicked();

    void on_toolButton_7_clicked();

    void on_toolButton_8_clicked();

    void on_closeWindow_clicked();

private:

    Ui::Donate *ui_donate;

    bool clickPressedFlag = false;

    QPoint mouseClickCoordinate;

    void setTheme(int &ind_theme);

};

#endif // DONATE_H
