#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

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
    // themeSelection(); will be added
    // coloruI(); will be added
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initConnections()
{
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
    connect(serialCom, &SerialCommunication::dataReceived,
            this, [this](const QByteArray &data)
            {
                qDebug() << "Data received:" << data;

                if(ui->rbASCII->isChecked())
                    ui->pteReceivedData->setPlainText(QString::fromUtf8(data));
                else
                {
                    QString text = data.toHex(' ').toUpper();
                    ui->pteReceivedData->setPlainText(text);
                }
            });
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
    qDebug() << "Serial connected!";
}

void MainWindow::onSerialError(const QString &msg)
{
    QMessageBox::critical(this, "Serial Error", msg);
}

void MainWindow::loadStyleSheet(const QString &path)
{
    QFile file(path);

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "Could not open stylesheet file";
        return;
    }

    QTextStream stream(&file);
    QString styleSheet = stream.readAll();

    qApp->setStyleSheet(styleSheet);

    file.close();
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

            ui->pbClear->setEnabled(true);
            ui->pbSend->setEnabled(true);
            ui->leInputData->setEnabled(true);
            ui->pteReceivedData->setEnabled(true);
        }
        else
        {
            serialCom->disconnectPort();

            ui->pbClear->setEnabled(false);
            ui->pbSend->setEnabled(false);
            ui->leInputData->setEnabled(false);
            ui->pteReceivedData->setEnabled(false);

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


void MainWindow::on_pbSend_clicked()
{
    if(ui->rbASCII->isChecked())
        serialCom->sendCommand(ui->leInputData->text().toUtf8());
    else
    {
        QByteArray data = QByteArray::fromHex(
            ui->leInputData->text().toUtf8()
            );
        serialCom->sendCommand(data);
    }
}

void MainWindow::on_pbClear_clicked()
{
    ui->pteReceivedData->clear();
}

