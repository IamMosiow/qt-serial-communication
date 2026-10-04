#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QByteArray>
#include <QDir>

class Logger : public QObject
{
    Q_OBJECT
public:
    enum class Direction {
        TX,
        RX,
        Event,
        Error
    };

    explicit Logger(QObject *parent = nullptr);
    ~Logger() override;

    bool startSession(const QString &sessionPrefix = "serial_log");
    void stopSession();
    bool isSessionActive() const;

    void setEnabled(bool enabled);
    bool isEnabled() const;

    void setLogDirectory(const QString &dirPath);
    QString logDirectory() const;
    QString currentFilePath() const;

    void logTx(const QByteArray &data, bool isHex = false);
    void logRx(const QByteArray &data, bool isHex = false);
    void logEvent(const QString &message);
    void logError(const QString &errorMessage);

    bool openLogDirectoryInExplorer() const;

    quint64 totalBytesLogged() const;
    quint64 totalEntriesLogged() const;

signals:
    void logMessageAdded(const QString &formattedLine);
    void loggerError(const QString &error);
    void sessionStarted(const QString &filePath);
    void sessionStopped();

private:
    void writeEntry(Direction direction, const QString &payload);
    QString directionToString(Direction dir) const;

    bool m_enabled = true;
    QString m_logDirectory;
    QString m_currentFilePath;
    QFile m_file;
    QTextStream m_stream;
    quint64 m_totalBytesLogged = 0;
    quint64 m_totalEntriesLogged = 0;
};

#endif // LOGGER_H
