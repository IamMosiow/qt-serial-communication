#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QByteArray>
#include <QList>

struct SerialConfig
{
    QString portName;
    qint32 baudeRate = 115200;
    QSerialPort::DataBits dataBits = QSerialPort::Data8;
    QSerialPort::Parity parity = QSerialPort::NoParity;
    QSerialPort::StopBits stopBits = QSerialPort::OneStop;
    QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
};

class SerialCommunication : public QObject
{
    Q_OBJECT
public:
    explicit SerialCommunication(QObject *parent = nullptr);
    ~SerialCommunication() override;

    bool connectPort(const SerialConfig &config);
    void disconnectPort();
    bool isConnected() const;
    qint64 sendCommand(const QByteArray &cmd);

    SerialConfig currentConfig() const;
    QString portName() const;

    static QList<QSerialPortInfo> availablePorts();

signals:
    void connected();
    void disconnected();
    void error(const QString &msg);
    void dataReceived(const QByteArray &data);
    void dataSent(const QByteArray &data);

private slots:
    void onReadyRead();
    void handleSerialError(QSerialPort::SerialPortError portError);

private:
    QSerialPort m_serial;
    SerialConfig m_config;
};

#endif // SERIALCOMMUNICATION_H
