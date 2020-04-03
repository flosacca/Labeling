#include "labeldialog.h"
#include "ui_labeldialog.h"
#include "util.h"

LabelDialog::LabelDialog(QWidget* parent) :
    QDialog(parent, Qt::WindowCloseButtonHint),
    color(randomColor(0)),
    ui(new Ui::LabelDialog)
{
    ui->setupUi(this);
    ui->tag->selectAll();
}

LabelDialog::~LabelDialog() {
    delete ui;
}

QString LabelDialog::text() const {
    return ui->tag->text();
}

bool LabelDialog::hasBorder() const {
    return ui->chkBorder->isChecked();
}

void LabelDialog::on_cmbShape_currentIndexChanged(int index) {
    shape = static_cast<Label::Shape>(index);
    ui->chkBorder->setChecked(shape == Label::Rect);
}

// Automatically change the opacity
void LabelDialog::on_chkBorder_toggled(bool checked) {
    if (checked)
        color.setAlpha(0);
    else
        color.setAlphaF(0.5);
}

void LabelDialog::on_btnColor_clicked() {
    QColorDialog colorDlg(this);
    colorDlg.setOption(QColorDialog::ShowAlphaChannel);
    colorDlg.setCurrentColor(color);
    colorDlg.adjustSize();
    if (colorDlg.exec() == QDialog::Accepted)
        color = colorDlg.selectedColor();
}
