#include "label.h"

void Label::setColor(const QColor& color) {
    pen.setColor(color.rgb());
    brush = QBrush(color);
}

QPen Label::getPen(const QColor& color) {
    return QPen(QBrush(color.rgb()), 4);
}

QDataStream& operator<<(QDataStream& o, const Label& l) {
    return o << l.tag << l.shape << l.pen << l.brush << l.path;
}

QDataStream& operator>>(QDataStream& i, Label& l) {
    return i >> l.tag >> l.shape >> l.pen >> l.brush >> l.path;
}
