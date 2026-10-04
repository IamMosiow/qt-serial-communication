#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QGraphicsDropShadowEffect>

class StyleManager
{
public:
    /**
     * @brief Apply subtle drop-shadow effect to any container widget (e.g., QGroupBox).
     */
    static void applyShadowEffect(QWidget *widget, int blurRadius = 25, int xOffset = 0, int yOffset = 3, int alpha = 45);

    /**
     * @brief Apply style to the Connect/Disconnect toggle button based on connection state.
     */
    static void applyConnectButtonStyle(QPushButton *button, bool isConnected);

    /**
     * @brief Apply style to the Send button.
     */
    static void applySendButtonStyle(QPushButton *button);

    /**
     * @brief Apply style to the Connection Status badge label.
     */
    static void applyStatusBadgeStyle(QLabel *label, bool isConnected);

    /**
     * @brief Load and set application-wide stylesheet with fallback.
     */
    static bool loadApplicationStyle(const QString &path = ":/styles/Aqua.qss");
};

#endif // STYLEMANAGER_H
