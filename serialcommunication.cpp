#include "serialcommunication.h"
#include <QDebug>

SerialCommunication :: SerialCommunication(QObject *parent)
    : QObject(parent)
{
}

void SerialCommunication :: connectPort(const SerialConfig &config)
{
    if(!serial.isOpen())
    {
        serial.setPortName(config.portName);
        serial.setBaudRate(config.baudeRate);
        serial.setDataBits(config.dataBits);
        serial.setParity(config.parity);
        serial.setStopBits(config.stopBits);

        if (serial.open(QIODevice::ReadWrite)) {
            emit connected();
            qDebug() << "Serial port connected to" << config.portName;
        }else {
            qDebug() << "Failed to connect to port:" << serial.errorString();
            emit error(serial.errorString());
        }
    }
}

void SerialCommunication::disconnectPort()
{
    if (serial.isOpen()){
        serial.close();
        qDebug() << "Serial port disconnected.";
        emit disconnected();
    }
}

bool SerialCommunication :: isConnected() const
{
    return serial. isOpen();
}

void SerialCommunication :: sendCommand(const QByteArray &cmd)
{
    if(!serial.isOpen())
    {
        emit error("Serial not connected!");
        return;
    }
    qDebug() << cmd;
    serial.write(cmd);
}
