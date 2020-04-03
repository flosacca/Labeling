#include "renderarea.h"
#include "util.h"

RenderArea::RenderArea(QWidget* parent) :
    QLabel(parent)
{
    setMouseTracking(true);
    setBackgroundRole(QPalette::Base);
    setPixmap(QPixmap());
    connect(this, &RenderArea::painted, this, QOverload<>::of(&RenderArea::update));
    connect(this, &RenderArea::labelUpdated, this, &RenderArea::painted);
    connect(this, &RenderArea::labelUpdated, std::bind(&RenderArea::setSelectedLabel, this, nullptr));
    connect(this, &RenderArea::labelChanged, this, &RenderArea::labelUpdated);
}

const QList<Label>& RenderArea::labelList() const {
    return labels;
}

void RenderArea::setLabelList(const QList<Label>& labelList) {
    labels = labelList;
    emit labelUpdated();
}

void RenderArea::appendLabel(const Label& label) {
    labels << label;
    emit labelUpdated();
}

void RenderArea::setLabelVisible(bool visible) {
    labelVisible = visible;
    emit painted();
}

void RenderArea::newLabel(const Label& label) {
    labels << label;
    painting = true;
    emit painted();
}

void RenderArea::remove(const QString& tag) {
    bool changed = false;
    QList<Label> rested;
    for (const Label& label: labels) {
        if (label.tag != tag)
            rested << label;
        else
            changed = true;
    }
    if (changed) {
        labels = rested;
        emit labelChanged();
    }
}

void RenderArea::removeSelectedLabel() {
    for (auto it = labels.begin(); it != labels.end(); ++it)
        if (&*it == selectedLabel) {
            labels.erase(it);
            setSelectedLabel(nullptr);
            emit labelChanged();
            break;
        }
}

void RenderArea::loadLabels(const QString& fileName) {
    labels.clear();
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream istream(&file);
        istream >> labels;
    }
    // Randomly reset color
    /*
    for (auto& label: labels) {
        QColor color = randomColor();
        if (label.pen != Qt::NoPen)
            label.pen = Label::getPen(color);
        color.setAlpha(label.brush.color().alpha());
        label.brush.setColor(color);
    }
    */
    emit labelChanged();
}

void RenderArea::saveLabels(const QString& fileName) {
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream ostream(&file);
        ostream << labels;
    }
}

void RenderArea::paintEvent(QPaintEvent* event) {
    QLabel::paintEvent(event);
    if (!labelVisible)
        return;
    QPainter painter(this);
    for (const Label& label: labels) {
        QPen pen = label.pen;
        // Always show border when drawing a polygon or a closed curve
        /*
        if (painting && &label == &labels.last()) {
            switch (label.shape) {
            case Label::Poly:
            case Label::Curve:
                pen = Label::getPen(label.brush.color());
            }
        }
        */
        painter.setPen(pen);
        painter.setBrush(label.brush);
        painter.drawPath(label.path);
    }

    // Draw an extra pen when drawing a region
    if (painting && labels.last().shape == Label::Region)
        painter.drawEllipse(mapFromGlobal(QCursor::pos()), Radius, Radius);
}

void RenderArea::mousePressEvent(QMouseEvent* event) {
    emit mousePressed(this);

    // Set selected label
    if (!painting) {
        Label* ptr = nullptr;
        for (Label& label: labels)
            if (label.path.contains(event->pos()))
                ptr = &label;
        setSelectedLabel(ptr);
        return;
    }

    auto& path = labels.last().path;
    switch(labels.last().shape) {
    case Label::Rect:
        if (event->button() == Qt::LeftButton) {
            path.moveTo(event->pos());
        }
        break;

    case Label::Region:
        if (event->button() == Qt::LeftButton) {
            QPainterPath circ;
            circ.addEllipse(event->pos(), Radius, Radius);
            path |= circ;
            lastPos = event->pos();
            emit painted();
        }
        if (event->button() == Qt::RightButton) {
            painting = false;
            emit labelChanged();
        }
        break;

    case Label::Poly:
        if (event->button() == Qt::LeftButton) {
            if (!path.elementCount()) {
                path.moveTo(event->pos());
                lastPath = path;
            } else {
                lastPath.lineTo(event->pos());
                path = lastPath;
                emit painted();
            }
        }
        if (event->button() == Qt::RightButton) {
            lastPath.closeSubpath();
            path = lastPath;
            painting = false;
            emit labelChanged();
        }
        break;

    case Label::Curve:
        if (event->button() == Qt::LeftButton) {
            if (!path.elementCount()) {
                path.moveTo(event->pos());
                lastPath = path;
                currentPath = path;
            } else {
                if (lastPath == currentPath) {
                    currentPath.lineTo(event->pos());
                } else {
                    QPointF endPt = currentPath.currentPosition();
                    currentPath = lastPath;
                    currentPath.quadTo(event->pos(), endPt);
                }
                path = currentPath;
                emit painted();
            }
        }
        if (event->button() == Qt::RightButton) {
            if (path.isEmpty()) {
                labels.removeLast();
                painting = false;
                break;
            }
            QPointF startPt = currentPath.elementAt(0);
            if (lastPath == currentPath) {
                currentPath.lineTo(startPt);
                path = currentPath;
                emit painted();
            } else {
                if (currentPath.currentPosition() == startPt) {
                    painting = false;
                    emit labelChanged();
                    break;
                }
                lastPath = currentPath;
            }
        }
        break;
    }
}

void RenderArea::mouseReleaseEvent(QMouseEvent* event) {
    if (!painting)
        return;
    if (labels.last().shape == Label::Rect) {
        if (event->button() == Qt::LeftButton) {
            painting = false;
            emit labelChanged();
        }
    }
}

void RenderArea::mouseMoveEvent(QMouseEvent* event) {
    emit mouseMoved(event->pos());
    if (!painting)
        return;

    auto& path = labels.last().path;
    switch (labels.last().shape) {
    case Label::Rect:
        if (event->buttons() & Qt::LeftButton) {
            QPointF p1 = path.currentPosition();
            QPointF p2 = event->pos();
            path = QPainterPath();
            path.addRect(QRectF(QPointF(qMin(p1.x(), p2.x()), qMin(p1.y(), p2.y())), QPointF(qMax(p1.x(), p2.x()), qMax(p1.y(), p2.y()))));
            path.moveTo(p1);
            emit painted();
        }
        break;

    case Label::Region:
        // To draw a "line with width", draw a rotated rectangle
        if (event->buttons() & Qt::LeftButton) {
            qreal dx = event->x() - lastPos.x();
            qreal dy = event->y() - lastPos.y();
            QMatrix trans;
            trans.rotate(qAtan2(dy, dx) / M_PI * 180);
            QPainterPath rect;
            rect.addPolygon(trans.map(QPolygonF(QRectF(0, -Radius, qSqrt(dx*dx + dy*dy), Radius*2))));
            rect.translate(lastPos);
            QPainterPath circ;
            circ.addEllipse(event->pos(), Radius, Radius);
            path |= rect | circ;
            lastPos = event->pos();
        }
        emit painted();
        break;

    case Label::Poly:
        if (!path.elementCount())
            break;
        path = lastPath;
        path.lineTo(event->pos());
        emit painted();
        break;

    case Label::Curve:
        if (!path.elementCount())
            break;
        if (lastPath == currentPath) {
            path = lastPath;
            path.lineTo(event->pos());
            emit painted();
        }
        break;
    }
}

void RenderArea::setSelectedLabel(Label* label) {
    if (label != selectedLabel) {
        selectedLabel = label;
        emit selectedLabelChanged(label);
    }
}
