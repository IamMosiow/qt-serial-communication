#ifndef BYTEUTILS_H
#define BYTEUTILS_H

#include <QByteArray>
#include <QString>
#include <QtGlobal>
#include <cstring>

namespace ByteUtils {

/**
 * @brief Format a QByteArray into an uppercase space-separated Hex string.
 */
inline QString formatHex(const QByteArray &data, char separator = ' ')
{
    return QString::fromUtf8(data.toHex(separator)).toUpper();
}

/**
 * @brief Parse a Hex string (with or without spaces) into a QByteArray.
 */
inline QByteArray parseHex(const QString &hexString)
{
    QString cleaned = hexString;
    cleaned.remove(' ').remove(':').remove('-');
    return QByteArray::fromHex(cleaned.toUtf8());
}

/**
 * @brief Read a 32-bit float in Little-Endian format from buffer at offset.
 */
inline float readFloatLE(const QByteArray &data, int offset)
{
    if (offset + static_cast<int>(sizeof(float)) > data.size()) {
        return 0.0f;
    }
    float result = 0.0f;
    std::memcpy(&result, data.constData() + offset, sizeof(float));
    return result;
}

/**
 * @brief Read a 16-bit unsigned integer in Little-Endian format.
 */
inline quint16 readUInt16LE(const QByteArray &data, int offset)
{
    if (offset + static_cast<int>(sizeof(quint16)) > data.size()) {
        return 0;
    }
    const auto *ptr = reinterpret_cast<const quint8*>(data.constData() + offset);
    return static_cast<quint16>(ptr[0] | (ptr[1] << 8));
}

/**
 * @brief Read a 32-bit unsigned integer in Little-Endian format.
 */
inline quint32 readUInt32LE(const QByteArray &data, int offset)
{
    if (offset + static_cast<int>(sizeof(quint32)) > data.size()) {
        return 0;
    }
    const auto *ptr = reinterpret_cast<const quint8*>(data.constData() + offset);
    return static_cast<quint32>(ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24));
}

/**
 * @brief Simple 8-bit XOR checksum calculation.
 */
inline quint8 calculateXorChecksum(const QByteArray &data)
{
    quint8 checksum = 0;
    for (int i = 0; i < data.size(); ++i) {
        checksum ^= static_cast<quint8>(data.at(i));
    }
    return checksum;
}

} // namespace ByteUtils

#endif // BYTEUTILS_H
