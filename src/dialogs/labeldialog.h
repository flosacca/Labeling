#ifndef LABELDIALOG_H
#define LABELDIALOG_H

#include <QtWidgets>
#include "label.h"

namespace Ui {
class LabelDialog;
}

// Dialog for setting styles of a label

class LabelDialog : public QDialog {
    Q_OBJECT

public:
    explicit LabelDialog(QWidget* parent = nullptr);
    ~LabelDialog();
    QString text() const;
    bool hasBorder() const;

public:
    QColor color;
    int shape = 0;

private slots:
    void on_cmbShape_currentIndexChanged(int index);
    void on_chkBorder_toggled(bool checked);
    void on_btnColor_clicked();

private:
    Ui::LabelDialog* ui;
};

#endif // LABELDIALOG_H
