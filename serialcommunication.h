#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QObject>
#include <QSerialPort>

struct SerialConfig
{
    QString portName;
    qint32 baudeRate;
    QSerialPort :: DataBits dataBits;
    QSerialPort :: Parity parity;
    QSerialPort :: StopBits stopBits;
};

class SerialCommunication
{
public:
    explicit SerialCommunication(QObject *parent = nullptr);

    void connectPort(const SerialConfig &config);
    void disconnectPort();
    bool isConnected() const;
    void sendCommand(const QByteArray &cmd) ;

signals:

    void connected();
    void disconnected();
    void error(const QString &msg);
private:

    QSerialPort serial;
    void onReadyRead(); //must be added
};

#endif // SERIALCOMMUNICATION_H
