#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include "windowfwd.h"
#include "renderarea.h"
#include "listex.h"

namespace Ui {
class MainWindow;
}

// Main window for 2D images
// Provides file operations

class MainWindow : public QMainWindow {
    Q_OBJECT

    friend class SubWindow;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    bool hasFile() const;
    bool hasImage() const;
    bool loadFile();

public slots:
    void closeFile();

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private slots:
    // Enable or disable actions according to the current status
    void updateActions();

    void updateUndoList();
    void updateStatus(Label* label);
    void updateMagnifier(const QPoint& pos);

    void on_actOpen_triggered();
    void on_actOpenFolder_triggered();
    void on_actLoad_triggered();
    void on_actSave_triggered();
    void on_actSaveAs_triggered();
    void on_actPrev_triggered();
    void on_actNext_triggered();
    void on_actClose_triggered();
    void on_actCloseAll_triggered();
    void on_actNew_triggered();
    void on_actRemove_triggered();
    void on_actRemoveAll_triggered();
    void on_actUndo_triggered();
    void on_actRedo_triggered();
    void on_actSwitch_triggered();

private:
    // The layout of the standard QInputDialog looks awful because the label is above the edit.
    // It seems there's way to put the label on the left of the edit but to set all widgets manually.
    static QLineEdit* initInputDialog(QDialog* dlg, const QString& title, const QString& labelText);

private:
    SubWindow* subWindow;
    Ui::MainWindow* ui;

    RenderArea* canvas;

    // Central widget
    QScrollArea* area;

    // It seems hard to edit a dock widget in UI editor so set it in manually
    QDockWidget* dockStatus;
    QListWidget* status;
    QDockWidget* dockMagnifier;
    QLabel* magnifier;

    ListEx<QString> files;

    // Save the whole label list whenever it changes
    // It's efficient because of the implicit sharing
    ListEx<QList<Label>> undoList;
};

#endif // MAINWINDOW_H
