#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include <QtWidgets>
#include "windowfwd.h"
#include "renderarea.h"
#include "cuboidlabel.h"

namespace Ui {
class SubWindow;
}

// Main window for 3D images
// Provides file operations

class SubWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit SubWindow(MainWindow* window, QWidget* parent = nullptr);
    ~SubWindow();
    bool load(const QString& dirName);

private slots:
    // Update images in each views according to the current cursor
    void refresh();

    void toggleActiveImage(RenderArea* img);
    void updateActions(bool open);

    void on_actSwitch_triggered();
    void on_actOpen_triggered();
    void on_actLoad_triggered();
    void on_actSave_triggered();
    void on_actClose_triggered();
    void on_actNew_triggered();
    void on_actRemove_triggered();

private:
    QImage getTop(int k);
    QImage getLeft(int j);
    QImage getFront(int i);
    void setTop(int k);
    void setLeft(int j);
    void setFront(int i);

    // For convenience to set all views
    QList<RenderArea*> images();

private:
    MainWindow* mainWindow;
    Ui::SubWindow* ui;

    RenderArea* imgTop;
    RenderArea* imgLeft;
    RenderArea* imgFront;
    QGroupBox* grpBox;

    RenderArea* activeImg = nullptr;
    struct { int x, y, z; } cursor{0, 0, 0};
    struct { int h, w, d; } imgSize{0, 0, 0};

    // Store images directly to avoid a deep copy
    QVector<QImage> imgData;

    QList<CuboidLabel> labels;
};

#endif // SUBWINDOW_H
