#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialcommunication.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void disconnect();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void baudRates() ;
    SerialCommunication *serialCom;
    void onSerialConnected();
    void onSerialError(const QString &msg);
    void themeSelection();
    void loadStyleSheet(const QString &path);
    void coloruI();

private slots:

    // void on_pbConnect_clicked();
    // void on_pbReloadPorts_clicked();

    void on_pbConnect_clicked();
    void on_pbRefresh_clicked();
};
#endif // MAINWINDOW_H
