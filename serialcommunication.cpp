#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serialCom = new SerialCommunication(this);
    baudRates();
    initConnections();

    on_pbRefresh_clicked();
    loadStyleSheet("styles/Aqua.qss");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initConnections()
{
    connect(serialCom, &SerialCommunication::connected, this, &MainWindow::onSerialConnected);
    connect(serialCom, &SerialCommunication::error, this, &MainWindow::onSerialError);

    connect(serialCom, &SerialCommunication::connected, this, [this](){
        ui->pbConnect->setText("Disconnect");
        ui->cbComPort->setEnabled(false);
        ui->cbBaudRate->setEnabled(false);
        ui->cbParity->setEnabled(false);
        ui->cbStopBit->setEnabled(false);

        ui->pbClear->setEnabled(true);
        ui->pbSend->setEnabled(true);
        ui->leInputData->setEnabled(true);
        ui->pteReceivedData->setEnabled(true);
    });

    connect(serialCom, &SerialCommunication::disconnected, this, [this](){
        ui->pbConnect->setText("Connect");
        ui->cbComPort->setEnabled(true);
        ui->cbBaudRate->setEnabled(true);
        ui->cbParity->setEnabled(true);
        ui->cbStopBit->setEnabled(true);

        ui->pbClear->setEnabled(false);
        ui->pbSend->setEnabled(false);
        ui->leInputData->setEnabled(false);
        ui->pteReceivedData->setEnabled(false);
    });

    connect(serialCom, &SerialCommunication::dataReceived, this, [this](const QByteArray &data)
            {
                if (ui->rbASCII->isChecked()) {
                    ui->pteReceivedData->moveCursor(QTextCursor::End);
                    ui->pteReceivedData->insertPlainText(QString::fromUtf8(data));
                } else {
                    ui->pteReceivedData->moveCursor(QTextCursor::End);
                    ui->pteReceivedData->insertPlainText(data.toHex(' ').toUpper() + " ");
                }
            });

    ui->cbStopBit->addItem("One-Bit", QSerialPort::OneStop);
    ui->cbStopBit->addItem("Two-Bit", QSerialPort::TwoStop);

    ui->cbParity->addItem("NoParity", QSerialPort::NoParity);
    ui->cbParity->addItem("EvenParity", QSerialPort::EvenParity);
    ui->cbParity->addItem("MarkParity", QSerialPort::MarkParity);
    ui->cbParity->addItem("OddParity", QSerialPort::OddParity);
    ui->cbParity->addItem("SpaceParity", QSerialPort::SpaceParity);
}

void MainWindow::baudRates()
{
    ui->cbBaudRate->addItem("1200", QSerialPort::Baud1200);
    ui->cbBaudRate->addItem("2400", QSerialPort::Baud2400);
    ui->cbBaudRate->addItem("9600", QSerialPort::Baud9600);
    ui->cbBaudRate->addItem("19200", QSerialPort::Baud19200);
    ui->cbBaudRate->addItem("38400", QSerialPort::Baud38400);
    ui->cbBaudRate->addItem("57600", QSerialPort::Baud57600);
    ui->cbBaudRate->addItem("115200", QSerialPort::Baud115200);
    ui->cbBaudRate->setCurrentIndex(6);
}

void MainWindow::onSerialConnected()
{
    qDebug() << "Serial connected!";
}

void MainWindow::onSerialError(const QString &msg)
{
    QMessageBox::critical(this, "Serial Error", msg);
}

void MainWindow::loadStyleSheet(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Could not open stylesheet file:" << path;
        return;
    }
    QTextStream stream(&file);
    qApp->setStyleSheet(stream.readAll());
    file.close();
}

void MainWindow::on_pbConnect_clicked()
{
    if (!serialCom->isConnected()) {
        if (ui->cbComPort->currentIndex() < 0) {
            QMessageBox::warning(this, "Port Error", "No COM port selected.");
            return;
        }

        SerialConfig cfg;
        cfg.portName = ui->cbComPort->currentData().toString();
        cfg.baudeRate = ui->cbBaudRate->currentData().toInt();
        cfg.dataBits = QSerialPort::Data8;
        cfg.parity = ui->cbParity->currentData().value<QSerialPort::Parity>();
        cfg.stopBits = ui->cbStopBit->currentData().value<QSerialPort::StopBits>();

        serialCom->connectPort(cfg);
    } else {
        serialCom->disconnectPort();
    }
}

void MainWindow::on_pbRefresh_clicked()
{
    ui->cbComPort->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
        QString fullName = port.portName() + " - " + port.description();
        ui->cbComPort->addItem(fullName, port.portName());
    }
}

void MainWindow::on_pbSend_clicked()
{
    QString text = ui->leInputData->text();
    if (text.isEmpty()) return;

    if (ui->rbASCII->isChecked()) {
        serialCom->sendCommand(text.toUtf8());
    } else {
        serialCom->sendCommand(QByteArray::fromHex(text.toUtf8()));
    }
    ui->leInputData->clear();
}

void MainWindow::on_pbClear_clicked()
{
    ui->pteReceivedData->clear();
}
