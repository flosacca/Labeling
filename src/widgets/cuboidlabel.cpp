#include "cuboidlabel.h"

QPainterPath CuboidLabel::rectPath(QRect rect) {
    QPainterPath path;
    path.addRect(rect);
    return path;
}

Label CuboidLabel::toLabel(QRect rect) const {
    return {tag, Label::Rect, pen, brush, rectPath(rect)};
}

Label CuboidLabel::top() const {
    return toLabel(QRect(QPoint(x1, y1), QPoint(x2, y2)));
}

Label CuboidLabel::left() const {
    return toLabel(QRect(QPoint(y1, z1), QPoint(y2, z2)));
}

Label CuboidLabel::front() const {
    return toLabel(QRect(QPoint(x1, z1), QPoint(x2, z2)));
}

bool CuboidLabel::contains(int x, int y, int z) const {
    return x1 <= x && x <= x2 && y1 <= y && y <= y2 && z1 <= z && z <= z2;
}

QDataStream& operator<<(QDataStream& o, const CuboidLabel& l) {
    return o << l.x1 << l.y1 << l.z1 << l.x2 << l.y2 << l.z2 << l.tag << l.pen << l.brush;
}

QDataStream& operator>>(QDataStream& i, CuboidLabel& l) {
    return i >> l.x1 >> l.y1 >> l.z1 >> l.x2 >> l.y2 >> l.z2 >> l.tag >> l.pen >> l.brush;
}
