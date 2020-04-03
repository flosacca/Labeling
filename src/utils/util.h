#ifndef UTIL_H
#define UTIL_H

#include <QtGui>

inline QColor randomColor(int alpha = 0xFF) {
    QRgb rgb = QRandomGenerator::global()->bounded(0xFFFFFFu);
    return QColor(qRed(rgb), qGreen(rgb), qBlue(rgb), alpha);
}

inline QStringList imageFilters() {
    QStringList filters;
    for(const QByteArray& format: QImageReader::supportedImageFormats())
        filters << "*."+format;
    return filters;
}

#endif // UTIL_H
