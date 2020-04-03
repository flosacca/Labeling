#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QtWidgets>
#include "label.h"
#include "listex.h"

// Widget to render an image and several labels
// Inherit from QLabel for convenience to render an image

class RenderArea : public QLabel {
    Q_OBJECT

public:
    RenderArea(QWidget* parent = nullptr);
    const QList<Label>& labelList() const;
    void setLabelList(const QList<Label>& labelList);
    void appendLabel(const Label& label);
    void setLabelVisible(bool visible);

    // Start drawing instead of appending a label immediately
    void newLabel(const Label& label);

    void remove(const QString& tag);
    void removeSelectedLabel();

public slots:
    void loadLabels(const QString& fileName);
    void saveLabels(const QString& fileName);

signals:
    void painted();

    // Implies painted()
    void labelUpdated();

    // Implies labelUpdated()
    void labelChanged();

    void selectedLabelChanged(Label* label);

    void mouseMoved(const QPoint& pos);
    void mousePressed(RenderArea* sender);

protected:
    void paintEvent(QPaintEvent* event);

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private:
    void setSelectedLabel(Label* label);

    QList<Label> labels;
    Label* selectedLabel = nullptr;

    // Whether the drawing of the new label is ongoing
    bool painting = false;

    bool labelVisible = true;

    // For multi-step drawing
    QPainterPath lastPath;
    QPainterPath currentPath;

    // For drawing a region
    QPoint lastPos;

    // Radius of pen to draw a region
    static const int Radius = 8;
};

#endif // RENDERAREA_H
