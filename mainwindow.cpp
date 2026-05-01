#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serialCom = new SerialCommunication(this);
    baudRates();
    connect(serialCom, &SerialCommunication :: connected, this, &MainWindow :: onSerialConnected) ;//neede
    connect(serialCom, &SerialCommunication :: error, this, &MainWindow :: onSerialError);
    connect(serialCom, &SerialCommunication :: connected, this, [this](){
        ui->pbConnect->setText("Disconnect");
        ui->cbComPort->setEnabled(false);
    });
    connect(serialCom, &SerialCommunication :: disconnected, this, [this](){
        ui->pbConnect->setText("Connect");
        ui->cbComPort->setEnabled(true);
    });
    // connect(serial, &SerialCommunication :: dataReived, this, &Mainwindow :: onDataReceived);
    on_pbRefresh_clicked();
    // themeSelection(); will be added
    // coloruI(); will be added
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::baudRates()
{
    ui->cbBaudRate->addItem("1200", QSerialPort ::Baud1200);
    ui->cbBaudRate->addItem("2400",QSerialPort::Baud2400);
    ui->cbBaudRate->addItem("9600",QSerialPort :: Baud9600);
    ui->cbBaudRate->addItem("1900", QSerialPort::Baud19200) ;
    ui->cbBaudRate->addItem("38400",QSerialPort :: Baud38400);
    ui->cbBaudRate->addItem("57600",QSerialPort :: Baud57600);
    ui->cbBaudRate->addItem("115200", QSerialPort :: Baud115200);
    ui->cbBaudRate->setCurrentIndex(6);
}

// void MainWindow::on_pbReloadPorts_clicked()
// {
// }

void MainWindow::onSerialConnected()
{
    // ui->cmComport->setEnabled(true);
}

void MainWindow::onSerialError(const QString &msg)
{
    QMessageBox::critical(this, "Serial Error", msg);
}


void MainWindow::on_pbConnect_clicked()
{
        if(!serialCom->isConnected())
        {
            SerialConfig cfg;
            cfg.portName = ui->cbComPort->currentData() .toString();
            cfg.baudeRate = ui->cbBaudRate->currentData().toInt();
            cfg.dataBits = QSerialPort::Data8;
            cfg.parity = QSerialPort::NoParity;
            cfg.stopBits = QSerialPort::OneStop;
            serialCom->connectPort(cfg);
        }
        else
        {
            serialCom->disconnectPort();
        }
}


void MainWindow::on_pbRefresh_clicked()
{
        ui->cbComPort->clear();
        const auto ports = QSerialPortInfo :: availablePorts();
        for (const QSerialPortInfo &port : ports) {
            QString portName = port.portName();
            QString description = port.description();
            QString fullName = portName + " - "+ description;
            ui->cbComPort->addItem(fullName, portName);
        }
}

