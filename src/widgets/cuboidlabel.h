#ifndef CUBOIDLABEL_H
#define CUBOIDLABEL_H

#include <QtWidgets>
#include "label.h"

// Store cuboid label data

class CuboidLabel {
public:
    static QPainterPath rectPath(QRect rect);

    // Ignore cuboid information
    Label toLabel(QRect rect) const;

    Label top() const;
    Label left() const;
    Label front() const;

    bool contains(int x, int y, int z) const;

public:
    int x1, y1, z1, x2, y2, z2;
    QString tag;
    QPen pen;
    QBrush brush;
};

QDataStream& operator<<(QDataStream& o, const CuboidLabel& l);
QDataStream& operator>>(QDataStream& i, CuboidLabel& l);

#endif // CUBOIDLABEL_H
