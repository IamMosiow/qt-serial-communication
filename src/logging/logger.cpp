#include "logger.h"
#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

Logger::Logger(QObject *parent)
    : QObject(parent)
{
    // Default log directory: <appDir>/logs
    m_logDirectory = QCoreApplication::applicationDirPath() + "/logs";
}

Logger::~Logger()
{
    stopSession();
}

bool Logger::startSession(const QString &sessionPrefix)
{
    if (m_file.isOpen()) {
        stopSession();
    }

    if (!m_enabled) {
        return false;
    }

    QDir dir(m_logDirectory);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            QString err = QString("Failed to create log directory: %1").arg(m_logDirectory);
            qWarning() << err;
            emit loggerError(err);
            return false;
        }
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    QString fileName = QString("%1_%2.txt").arg(sessionPrefix, timestamp);
    m_currentFilePath = dir.filePath(fileName);

    m_file.setFileName(m_currentFilePath);
    if (!m_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QString err = QString("Failed to open log file %1: %2").arg(m_currentFilePath, m_file.errorString());
        qWarning() << err;
        emit loggerError(err);
        return false;
    }

    m_stream.setDevice(&m_file);
    m_stream << "# ====================================================\n";
    m_stream << "# Serial Communication Log - Session Started\n";
    m_stream << "# Start Time: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz") << "\n";
    m_stream << "# ====================================================\n\n";
    m_stream.flush();

    emit sessionStarted(m_currentFilePath);
    qDebug() << "Log session started:" << m_currentFilePath;
    return true;
}

void Logger::stopSession()
{
    if (m_file.isOpen()) {
        m_stream << "\n# ====================================================\n";
        m_stream << "# Session Ended: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz") << "\n";
        m_stream << "# Total Bytes Logged: " << m_totalBytesLogged << "\n";
        m_stream << "# Total Entries: " << m_totalEntriesLogged << "\n";
        m_stream << "# ====================================================\n";
        m_stream.flush();
        m_file.close();
        emit sessionStopped();
        qDebug() << "Log session stopped.";
    }
}

bool Logger::isSessionActive() const
{
    return m_file.isOpen();
}

void Logger::setEnabled(bool enabled)
{
    m_enabled = enabled;
    if (!enabled && isSessionActive()) {
        stopSession();
    }
}

bool Logger::isEnabled() const
{
    return m_enabled;
}

void Logger::setLogDirectory(const QString &dirPath)
{
    if (dirPath.isEmpty() || dirPath == m_logDirectory) {
        return;
    }

    bool wasActive = isSessionActive();
    if (wasActive) {
        stopSession();
    }

    m_logDirectory = dirPath;

    if (wasActive) {
        startSession();
    }
}

QString Logger::logDirectory() const
{
    return m_logDirectory;
}

QString Logger::currentFilePath() const
{
    return m_currentFilePath;
}

void Logger::logTx(const QByteArray &data, bool isHex)
{
    if (data.isEmpty()) return;

    QString content;
    if (isHex) {
        content = "[HEX] " + QString::fromUtf8(data.toHex(' ')).toUpper();
    } else {
        content = QString::fromUtf8(data);
    }
    writeEntry(Direction::TX, content);
}

void Logger::logRx(const QByteArray &data, bool isHex)
{
    if (data.isEmpty()) return;

    QString content;
    if (isHex) {
        content = "[HEX] " + QString::fromUtf8(data.toHex(' ')).toUpper();
    } else {
        content = QString::fromUtf8(data);
    }
    writeEntry(Direction::RX, content);
}

void Logger::logEvent(const QString &message)
{
    writeEntry(Direction::Event, message);
}

void Logger::logError(const QString &errorMessage)
{
    writeEntry(Direction::Error, errorMessage);
}

bool Logger::openLogDirectoryInExplorer() const
{
    QDir dir(m_logDirectory);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return QDesktopServices::openUrl(QUrl::fromLocalFile(m_logDirectory));
}

quint64 Logger::totalBytesLogged() const
{
    return m_totalBytesLogged;
}

quint64 Logger::totalEntriesLogged() const
{
    return m_totalEntriesLogged;
}

QString Logger::directionToString(Direction dir) const
{
    switch (dir) {
    case Direction::TX:    return "TX";
    case Direction::RX:    return "RX";
    case Direction::Event: return "EVENT";
    case Direction::Error: return "ERROR";
    }
    return "UNKNOWN";
}

void Logger::writeEntry(Direction direction, const QString &payload)
{
    if (!m_enabled) {
        return;
    }

    if (!m_file.isOpen()) {
        if (!startSession()) {
            return;
        }
    }

    QString timeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString line = QString("[%1] [%2] %3").arg(timeStamp, directionToString(direction), payload);

    m_stream << line << "\n";
    m_stream.flush();

    m_totalBytesLogged += static_cast<quint64>(payload.toUtf8().size());
    m_totalEntriesLogged++;

    emit logMessageAdded(line);
}
