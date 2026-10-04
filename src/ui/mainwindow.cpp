#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stylemanager.h"
#include "byteutils.h"

#include <QSerialPortInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextCursor>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_serialCom(new SerialCommunication(this))
    , m_logger(new Logger(this))
    , m_lblStatusBadge(new QLabel(this))
    , m_lblPortInfo(new QLabel(this))
    , m_lblCounters(new QLabel(this))
    , m_btnResetCounters(new QPushButton("Reset Counters", this))
{
    ui->setupUi(this);

    initUi();
    populateBaudRates();
    populateSerialOptions();
    setupStatusBar();
    initConnections();

    // Populate COM ports on startup
    on_pbRefresh_clicked();

    // Load stylesheet
    StyleManager::loadApplicationStyle(":/styles/Aqua.qss");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUi()
{
    // Apply elegant drop shadows inspired by SerialCan
    StyleManager::applyShadowEffect(ui->gbConnection);
    StyleManager::applyShadowEffect(ui->gbLogging);
    StyleManager::applyShadowEffect(ui->gbDataTransfer);

    // Initial button styling
    StyleManager::applyConnectButtonStyle(ui->pbConnect, false);
    StyleManager::applySendButtonStyle(ui->pbSend);

    // Display current logging directory
    ui->leLogPath->setText(m_logger->logDirectory());
    ui->cbEnableLogging->setChecked(m_logger->isEnabled());

    // Initially disable transmission until connected
    updateUiForConnectionState(false);
}

void MainWindow::setupStatusBar()
{
    m_lblStatusBadge->setAlignment(Qt::AlignCenter);
    StyleManager::applyStatusBadgeStyle(m_lblStatusBadge, false);

    m_lblPortInfo->setText("Disconnected");
    m_lblPortInfo->setStyleSheet("QLabel { color: #555555; padding-left: 8px; }");

    updateCountersDisplay();
    m_lblCounters->setStyleSheet("QLabel { color: #333333; padding-left: 10px; padding-right: 10px; }");

    m_btnResetCounters->setFlat(true);
    m_btnResetCounters->setStyleSheet(
        "QPushButton { color: #2980B9; text-decoration: underline; padding: 2px 5px; }"
        "QPushButton:hover { color: #1B4F72; }"
    );

    ui->statusbar->addWidget(m_lblStatusBadge);
    ui->statusbar->addWidget(m_lblPortInfo);
    ui->statusbar->addPermanentWidget(m_lblCounters);
    ui->statusbar->addPermanentWidget(m_btnResetCounters);
}

void MainWindow::populateBaudRates()
{
    ui->cbBaudRate->clear();
    ui->cbBaudRate->addItem("1200", QSerialPort::Baud1200);
    ui->cbBaudRate->addItem("2400", QSerialPort::Baud2400);
    ui->cbBaudRate->addItem("4800", 4800);
    ui->cbBaudRate->addItem("9600", QSerialPort::Baud9600);
    ui->cbBaudRate->addItem("19200", QSerialPort::Baud19200);
    ui->cbBaudRate->addItem("38400", QSerialPort::Baud38400);
    ui->cbBaudRate->addItem("57600", QSerialPort::Baud57600);
    ui->cbBaudRate->addItem("115200", QSerialPort::Baud115200);
    ui->cbBaudRate->setCurrentIndex(7); // Default to 115200
}

void MainWindow::populateSerialOptions()
{
    ui->cbStopBit->clear();
    ui->cbStopBit->addItem("1 Stop Bit", QSerialPort::OneStop);
    ui->cbStopBit->addItem("1.5 Stop Bits", QSerialPort::OneAndHalfStop);
    ui->cbStopBit->addItem("2 Stop Bits", QSerialPort::TwoStop);

    ui->cbParity->clear();
    ui->cbParity->addItem("No Parity", QSerialPort::NoParity);
    ui->cbParity->addItem("Even Parity", QSerialPort::EvenParity);
    ui->cbParity->addItem("Odd Parity", QSerialPort::OddParity);
    ui->cbParity->addItem("Mark Parity", QSerialPort::MarkParity);
    ui->cbParity->addItem("Space Parity", QSerialPort::SpaceParity);
}

void MainWindow::initConnections()
{
    // SerialCommunication signals
    connect(m_serialCom, &SerialCommunication::connected, this, &MainWindow::onSerialConnected);
    connect(m_serialCom, &SerialCommunication::disconnected, this, &MainWindow::onSerialDisconnected);
    connect(m_serialCom, &SerialCommunication::error, this, &MainWindow::onSerialError);
    connect(m_serialCom, &SerialCommunication::dataReceived, this, &MainWindow::onSerialDataReceived);
    connect(m_serialCom, &SerialCommunication::dataSent, this, &MainWindow::onSerialDataSent);

    // Logger signals
    connect(m_logger, &Logger::loggerError, this, [this](const QString &err) {
        ui->statusbar->showMessage("Logger Error: " + err, 5000);
    });

    // Reset counters button
    connect(m_btnResetCounters, &QPushButton::clicked, this, &MainWindow::onResetCountersClicked);

    // Send on Enter pressed in input line edit
    connect(ui->leInputData, &QLineEdit::returnPressed, this, &MainWindow::on_pbSend_clicked);
}

void MainWindow::updateCountersDisplay()
{
    m_lblCounters->setText(QString("RX: %1 B | TX: %2 B").arg(m_rxBytes).arg(m_txBytes));
}

void MainWindow::updateUiForConnectionState(bool connected)
{
    ui->pbConnect->setText(connected ? "Disconnect" : "Connect");
    StyleManager::applyConnectButtonStyle(ui->pbConnect, connected);
    StyleManager::applyStatusBadgeStyle(m_lblStatusBadge, connected);

    ui->cbComPort->setEnabled(!connected);
    ui->cbBaudRate->setEnabled(!connected);
    ui->cbParity->setEnabled(!connected);
    ui->cbStopBit->setEnabled(!connected);
    ui->pbRefresh->setEnabled(!connected);

    ui->pbSend->setEnabled(connected);
    ui->leInputData->setEnabled(connected);
    ui->pbClear->setEnabled(true);

    if (connected) {
        m_lblPortInfo->setText(QString("%1 @ %2 baud").arg(m_serialCom->portName()).arg(m_serialCom->currentConfig().baudeRate));
    } else {
        m_lblPortInfo->setText("Disconnected");
    }
}

void MainWindow::on_pbConnect_clicked()
{
    if (!m_serialCom->isConnected()) {
        if (ui->cbComPort->currentIndex() < 0) {
            QMessageBox::warning(this, "Port Selection", "No COM port selected. Please select or refresh available ports.");
            return;
        }

        SerialConfig cfg;
        cfg.portName = ui->cbComPort->currentData().toString();
        cfg.baudeRate = ui->cbBaudRate->currentData().toInt();
        cfg.dataBits = QSerialPort::Data8;
        cfg.parity = ui->cbParity->currentData().value<QSerialPort::Parity>();
        cfg.stopBits = ui->cbStopBit->currentData().value<QSerialPort::StopBits>();
        cfg.flowControl = QSerialPort::NoFlowControl;

        if (m_serialCom->connectPort(cfg)) {
            m_logger->startSession("log_" + cfg.portName);
            m_logger->logEvent(QString("Opened connection to %1 at %2 baud").arg(cfg.portName).arg(cfg.baudeRate));
        }
    } else {
        m_logger->logEvent(QString("Closed connection to %1").arg(m_serialCom->portName()));
        m_serialCom->disconnectPort();
        m_logger->stopSession();
        emit disconnect();
    }
}

void MainWindow::on_pbRefresh_clicked()
{
    QString currentSelection = ui->cbComPort->currentData().toString();
    ui->cbComPort->clear();

    const auto ports = SerialCommunication::availablePorts();
    int selectedIndex = -1;

    for (int i = 0; i < ports.size(); ++i) {
        const auto &port = ports.at(i);
        QString display = QString("%1 - %2").arg(port.portName(), port.description());
        ui->cbComPort->addItem(display, port.portName());

        if (port.portName() == currentSelection) {
            selectedIndex = i;
        }
    }

    if (selectedIndex >= 0) {
        ui->cbComPort->setCurrentIndex(selectedIndex);
    } else if (ui->cbComPort->count() > 0) {
        ui->cbComPort->setCurrentIndex(0);
    }

    ui->statusbar->showMessage(QString("Found %1 available serial port(s).").arg(ports.size()), 3000);
}

void MainWindow::on_pbSend_clicked()
{
    QString text = ui->leInputData->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    QByteArray dataToSend;
    if (ui->rbASCII->isChecked()) {
        if (ui->cbSendWithNewline->isChecked()) {
            text += "\r\n";
        }
        dataToSend = text.toUtf8();
    } else {
        dataToSend = ByteUtils::parseHex(text);
        if (dataToSend.isEmpty() && !text.isEmpty()) {
            QMessageBox::warning(this, "Hex Format Error", "Invalid Hex string. Please provide valid hexadecimal bytes (e.g. '0A 1B FF').");
            return;
        }
    }

    m_serialCom->sendCommand(dataToSend);
    ui->leInputData->clear();
}

void MainWindow::on_pbClear_clicked()
{
    ui->pteReceivedData->clear();
}

void MainWindow::on_cbEnableLogging_toggled(bool checked)
{
    m_logger->setEnabled(checked);
    if (checked && m_serialCom->isConnected() && !m_logger->isSessionActive()) {
        m_logger->startSession("log_" + m_serialCom->portName());
    }
}

void MainWindow::on_pbBrowseLogDir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Log Directory", m_logger->logDirectory());
    if (!dir.isEmpty()) {
        m_logger->setLogDirectory(dir);
        ui->leLogPath->setText(dir);
    }
}

void MainWindow::on_pbOpenLogDir_clicked()
{
    if (!m_logger->openLogDirectoryInExplorer()) {
        QMessageBox::warning(this, "Open Directory", "Unable to open log directory.");
    }
}

void MainWindow::onResetCountersClicked()
{
    m_rxBytes = 0;
    m_txBytes = 0;
    updateCountersDisplay();
}

void MainWindow::onSerialConnected()
{
    updateUiForConnectionState(true);
    ui->statusbar->showMessage("Connected successfully.", 3000);
}

void MainWindow::onSerialDisconnected()
{
    updateUiForConnectionState(false);
    ui->statusbar->showMessage("Disconnected.", 3000);
}

void MainWindow::onSerialError(const QString &msg)
{
    ui->statusbar->showMessage("Serial Error: " + msg, 5000);
    m_logger->logError(msg);
    QMessageBox::warning(this, "Serial Port Error", msg);
}

void MainWindow::onSerialDataReceived(const QByteArray &data)
{
    m_rxBytes += static_cast<quint64>(data.size());
    updateCountersDisplay();

    // Log to file
    m_logger->logRx(data, ui->rbHex->isChecked());

    // Append to UI terminal
    QTextCursor cursor = ui->pteReceivedData->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->pteReceivedData->setTextCursor(cursor);

    if (ui->rbASCII->isChecked()) {
        ui->pteReceivedData->insertPlainText(QString::fromUtf8(data));
    } else {
        ui->pteReceivedData->insertPlainText(ByteUtils::formatHex(data) + " ");
    }

    if (ui->cbAutoScroll->isChecked()) {
        cursor.movePosition(QTextCursor::End);
        ui->pteReceivedData->setTextCursor(cursor);
    }
}

void MainWindow::onSerialDataSent(const QByteArray &data)
{
    m_txBytes += static_cast<quint64>(data.size());
    updateCountersDisplay();

    // Log to file
    m_logger->logTx(data, ui->rbHex->isChecked());
}
