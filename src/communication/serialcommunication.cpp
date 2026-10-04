#include "serialcommunication.h"
#include <QDebug>

SerialCommunication::SerialCommunication(QObject *parent)
    : QObject(parent)
{
    connect(&m_serial, &QSerialPort::readyRead, this, &SerialCommunication::onReadyRead);
    connect(&m_serial, &QSerialPort::errorOccurred, this, &SerialCommunication::handleSerialError);
}

SerialCommunication::~SerialCommunication()
{
    if (m_serial.isOpen()) {
        m_serial.close();
    }
}

bool SerialCommunication::connectPort(const SerialConfig &config)
{
    if (m_serial.isOpen()) {
        disconnectPort();
    }

    m_config = config;
    m_serial.setPortName(config.portName);
    m_serial.setBaudRate(config.baudeRate);
    m_serial.setDataBits(config.dataBits);
    m_serial.setParity(config.parity);
    m_serial.setStopBits(config.stopBits);
    m_serial.setFlowControl(config.flowControl);

    if (m_serial.open(QIODevice::ReadWrite)) {
        qDebug() << "Serial port connected:" << config.portName << "@" << config.baudeRate;
        emit connected();
        return true;
    } else {
        QString err = m_serial.errorString();
        qWarning() << "Failed to open port:" << config.portName << "-" << err;
        emit error(QString("Failed to open %1: %2").arg(config.portName, err));
        return false;
    }
}

void SerialCommunication::disconnectPort()
{
    if (m_serial.isOpen()) {
        m_serial.close();
        qDebug() << "Serial port disconnected:" << m_config.portName;
        emit disconnected();
    }
}

bool SerialCommunication::isConnected() const
{
    return m_serial.isOpen();
}

qint64 SerialCommunication::sendCommand(const QByteArray &cmd)
{
    if (!m_serial.isOpen()) {
        emit error("Cannot send data: Serial port is not open.");
        return -1;
    }

    qint64 bytesWritten = m_serial.write(cmd);
    if (bytesWritten > 0) {
        emit dataSent(cmd);
    } else if (bytesWritten < 0) {
        emit error(QString("Write error: %1").arg(m_serial.errorString()));
    }
    return bytesWritten;
}

SerialConfig SerialCommunication::currentConfig() const
{
    return m_config;
}

QString SerialCommunication::portName() const
{
    return m_serial.portName();
}

QList<QSerialPortInfo> SerialCommunication::availablePorts()
{
    return QSerialPortInfo::availablePorts();
}

void SerialCommunication::onReadyRead()
{
    QByteArray data = m_serial.readAll();
    if (!data.isEmpty()) {
        emit dataReceived(data);
    }
}

void SerialCommunication::handleSerialError(QSerialPort::SerialPortError portError)
{
    if (portError == QSerialPort::NoError) {
        return;
    }

    qWarning() << "Serial port error occurred:" << portError << m_serial.errorString();

    if (portError == QSerialPort::ResourceError ||
        portError == QSerialPort::DeviceNotFoundError ||
        portError == QSerialPort::PermissionError)
    {
        qWarning() << "Critical serial error - closing port";
        if (m_serial.isOpen()) {
            m_serial.close();
            emit error(QString("Connection lost: %1").arg(m_serial.errorString()));
            emit disconnected();
        }
    } else {
        emit error(m_serial.errorString());
    }
}
