#include "cuboiddialog.h"
#include "ui_cuboiddialog.h"
#include "util.h"

CuboidDialog::CuboidDialog(int h, int w, int d, QWidget* parent) :
    QDialog(parent, Qt::WindowCloseButtonHint),
    color(randomColor(0)),
    ui(new Ui::CuboidDialog)
{
    ui->setupUi(this);
    ui->x1->setMaximum(w);
    ui->y1->setMaximum(h);
    ui->z1->setMaximum(d);
    ui->x2->setMaximum(w);
    ui->y2->setMaximum(h);
    ui->z2->setMaximum(d);
    ui->x2->setValue(w);
    ui->y2->setValue(h);
    ui->z2->setValue(d);
}

CuboidDialog::~CuboidDialog() {
    delete ui;
}

int CuboidDialog::x1() const {
    return qMin(ui->x1->value(), ui->x2->value());
}

int CuboidDialog::y1() const {
    return qMin(ui->y1->value(), ui->y2->value());
}

int CuboidDialog::z1() const {
    return qMin(ui->z1->value(), ui->z2->value());
}

int CuboidDialog::x2() const {
    return qMax(ui->x1->value(), ui->x2->value());
}

int CuboidDialog::y2() const {
    return qMax(ui->y1->value(), ui->y2->value());
}

int CuboidDialog::z2() const {
    return qMax(ui->z1->value(), ui->z2->value());
}

QString CuboidDialog::text() const {
    return ui->tag->text();
}

bool CuboidDialog::hasBorder() const {
    return ui->chkBorder->isChecked();
}

// Automatically change the opacity
void CuboidDialog::on_chkBorder_stateChanged(int state) {
    if (state == Qt::Checked)
        color.setAlpha(0);
    if (state == Qt::Unchecked)
        color.setAlphaF(0.5);
}

void CuboidDialog::on_btnColor_clicked() {
    QColorDialog colorDlg(this);
    colorDlg.setOption(QColorDialog::ShowAlphaChannel);
    colorDlg.setCurrentColor(color);
    colorDlg.adjustSize();
    if (colorDlg.exec() == QDialog::Accepted)
        color = colorDlg.selectedColor();
}
