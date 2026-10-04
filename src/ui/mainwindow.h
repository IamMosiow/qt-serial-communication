#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include "serialcommunication.h"
#include "logger.h"

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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // Existing slots preserved
    void on_pbConnect_clicked();
    void on_pbRefresh_clicked();
    void on_pbSend_clicked();
    void on_pbClear_clicked();

    // New slots for logging & UI features
    void on_cbEnableLogging_toggled(bool checked);
    void on_pbBrowseLogDir_clicked();
    void on_pbOpenLogDir_clicked();
    void onResetCountersClicked();

    // Backend handlers
    void onSerialConnected();
    void onSerialDisconnected();
    void onSerialError(const QString &msg);
    void onSerialDataReceived(const QByteArray &data);
    void onSerialDataSent(const QByteArray &data);

private:
    void initUi();
    void populateBaudRates();
    void populateSerialOptions();
    void initConnections();
    void setupStatusBar();
    void updateCountersDisplay();
    void updateUiForConnectionState(bool connected);

    Ui::MainWindow *ui;
    SerialCommunication *m_serialCom;
    Logger *m_logger;

    // Status bar widgets
    QLabel *m_lblStatusBadge;
    QLabel *m_lblPortInfo;
    QLabel *m_lblCounters;
    QPushButton *m_btnResetCounters;

    // Byte counters
    quint64 m_rxBytes = 0;
    quint64 m_txBytes = 0;
};

#endif // MAINWINDOW_H
