#include "stylemanager.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QColor>
#include <QDebug>

void StyleManager::applyShadowEffect(QWidget *widget, int blurRadius, int xOffset, int yOffset, int alpha)
{
    if (!widget) return;
    auto *shadow = new QGraphicsDropShadowEffect(widget);
    shadow->setBlurRadius(blurRadius);
    shadow->setXOffset(xOffset);
    shadow->setYOffset(yOffset);
    shadow->setColor(QColor(0, 0, 0, alpha));
    widget->setGraphicsEffect(shadow);
}

void StyleManager::applyConnectButtonStyle(QPushButton *button, bool isConnected)
{
    if (!button) return;

    if (isConnected) {
        // Disconnect mode (Warm Red / Danger)
        button->setStyleSheet(
            "QPushButton {"
            "    background-color: #D9534F;"
            "    color: white;"
            "    font-weight: bold;"
            "    border: 1px solid #C9302C;"
            "    border-radius: 4px;"
            "    padding: 5px 12px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #C9302C;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #AC2925;"
            "}"
        );
    } else {
        // Connect mode (Emerald Green / Primary)
        button->setStyleSheet(
            "QPushButton {"
            "    background-color: #2ECC71;"
            "    color: white;"
            "    font-weight: bold;"
            "    border: 1px solid #27AE60;"
            "    border-radius: 4px;"
            "    padding: 5px 12px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #27AE60;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #1E8449;"
            "}"
        );
    }
}

void StyleManager::applySendButtonStyle(QPushButton *button)
{
    if (!button) return;
    button->setStyleSheet(
        "QPushButton:enabled {"
        "    background-color: #3498DB;"
        "    color: white;"
        "    font-weight: bold;"
        "    border: 1px solid #2980B9;"
        "    border-radius: 4px;"
        "    padding: 5px 15px;"
        "}"
        "QPushButton:hover:enabled {"
        "    background-color: #2980B9;"
        "}"
        "QPushButton:pressed:enabled {"
        "    background-color: #21618C;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #BDC3C7;"
        "    color: #7F8C8D;"
        "    border: 1px solid #BDC3C7;"
        "    border-radius: 4px;"
        "    padding: 5px 15px;"
        "}"
    );
}

void StyleManager::applyStatusBadgeStyle(QLabel *label, bool isConnected)
{
    if (!label) return;
    if (isConnected) {
        label->setText(" CONNECTED ");
        label->setStyleSheet(
            "QLabel {"
            "    background-color: #27AE60;"
            "    color: white;"
            "    font-weight: bold;"
            "    font-size: 11px;"
            "    border-radius: 3px;"
            "    padding: 2px 6px;"
            "}"
        );
    } else {
        label->setText(" DISCONNECTED ");
        label->setStyleSheet(
            "QLabel {"
            "    background-color: #7F8C8D;"
            "    color: white;"
            "    font-weight: bold;"
            "    font-size: 11px;"
            "    border-radius: 3px;"
            "    padding: 2px 6px;"
            "}"
        );
    }
}

bool StyleManager::loadApplicationStyle(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        // Fallback to local disk file if resource path fails
        QFile localFile("resources/styles/Aqua.qss");
        if (!localFile.exists()) {
            localFile.setFileName("styles/Aqua.qss");
        }
        if (localFile.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream stream(&localFile);
            qApp->setStyleSheet(stream.readAll());
            localFile.close();
            return true;
        }
        qWarning() << "Could not open stylesheet file:" << path;
        return false;
    }

    QTextStream stream(&file);
    qApp->setStyleSheet(stream.readAll());
    file.close();
    return true;
}
