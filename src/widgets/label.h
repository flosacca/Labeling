#ifndef LABEL_H
#define LABEL_H

#include <QtWidgets>

// Store label data

class Label {
public:
    enum Shape {Rect, Poly, Curve, Region};

public:
    void setColor(const QColor& color);

public:
    // All shapes use the same pen style
    // so just specify the color
    static QPen getPen(const QColor& color);

public:
    QString tag;
    int shape;
    QPen pen;
    QBrush brush;
    QPainterPath path;
};

QDataStream& operator<<(QDataStream& o, const Label& l);
QDataStream& operator>>(QDataStream& i, Label& l);

#endif // LABEL_H
