#ifndef CUBOIDDIALOG_H
#define CUBOIDDIALOG_H

#include <QtWidgets>

namespace Ui {
class CuboidDialog;
}

// Dialog for setting styles of a cuboid label

class CuboidDialog : public QDialog {
    Q_OBJECT

public:
    explicit CuboidDialog(int h, int w, int d, QWidget* parent = nullptr);
    ~CuboidDialog();
    int x1() const;
    int y1() const;
    int z1() const;
    int x2() const;
    int y2() const;
    int z2() const;
    QString text() const;
    bool hasBorder() const;

public:
    QColor color;

private slots:
    void on_chkBorder_stateChanged(int state);
    void on_btnColor_clicked();

private:
    Ui::CuboidDialog* ui;
};

#endif // CUBOIDDIALOG_H
