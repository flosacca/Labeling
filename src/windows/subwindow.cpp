#include "subwindow.h"
#include "ui_subwindow.h"
#include "mainwindow.h"
#include "cuboiddialog.h"
#include "util.h"

SubWindow::SubWindow(MainWindow* window, QWidget* parent) :
    QMainWindow(parent),
    mainWindow(window),
    ui(new Ui::SubWindow),
    imgTop(new RenderArea),
    imgLeft(new RenderArea),
    imgFront(new RenderArea),
    grpBox(new QGroupBox)
{
    ui->setupUi(this);
    auto* area1 = new QScrollArea(this);
    auto* area2 = new QScrollArea(this);
    auto* area3 = new QScrollArea(this);
    for (auto* area: {area1, area2, area3}) {
        area->setBackgroundRole(QPalette::Dark);
        area->setAlignment(Qt::AlignCenter);
    }
    area1->setWidget(imgTop);
    area2->setWidget(imgLeft);
    area3->setWidget(imgFront);
    auto* hLayout = new QHBoxLayout;
    hLayout->addWidget(area1);
    auto* vLayout = new QVBoxLayout;
    vLayout->addWidget(area2);
    vLayout->addWidget(area3);
    hLayout->addLayout(vLayout);
    grpBox->setLayout(hLayout);
    setCentralWidget(grpBox);

    for (auto* img: images()) {
        img->setVisible(false);
        connect(img, &RenderArea::mousePressed, this, &SubWindow::toggleActiveImage);
    }
    connect(imgTop, &RenderArea::mouseMoved, [&] (const QPoint& pos) {
        if (activeImg == imgTop) {
            cursor.x = pos.x();
            cursor.y = pos.y();
            refresh();
        }
    });
    connect(imgLeft, &RenderArea::mouseMoved, [&] (const QPoint& pos) {
        if (activeImg == imgLeft) {
            cursor.y = pos.x();
            cursor.z = pos.y();
            refresh();
        }
    });
    connect(imgFront, &RenderArea::mouseMoved, [&] (const QPoint& pos) {
        if (activeImg == imgFront) {
            cursor.x = pos.x();
            cursor.z = pos.y();
            refresh();
        }
    });
}

SubWindow::~SubWindow() {
    delete ui;
}

bool SubWindow::load(const QString& dirName) {
    QVector<QImage> imgs;
    QDir dir(dirName);
    for (const QString& fileName: dir.entryList(imageFilters())) {
        QString path = dir.filePath(fileName);
        QImageReader reader(path);
        reader.setAutoTransform(true);
        QImage img = reader.read();
        if (img.isNull()) {
            QMessageBox::information(this, QGuiApplication::applicationDisplayName(), QString("Cannot load %1: %2").arg(QDir::toNativeSeparators(path), reader.errorString()));
            return false;
        }
        imgs << img;
    }
    if (imgs.empty())
        return false;
    for (const QImage& img: imgs)
        if (img.size() != imgs.first().size())
            return false;

    imgSize = {imgs.first().height(), imgs.first().width(), imgs.size()};
    imgData.clear();
    for (QImage& img: imgs) {
        if (!QSet<QImage::Format>{QImage::Format_RGB32, QImage::Format_ARGB32}.contains(img.format()))
            img = img.convertToFormat(QImage::Format_ARGB32);
        imgData << img;
    }
    return true;
}

void SubWindow::refresh() {
    ui->statusBar->showMessage(QString::asprintf("Cursor: (%d, %d, %d)", cursor.x, cursor.y, cursor.z));
    setTop(cursor.z);
    setLeft(cursor.x);
    setFront(cursor.y);
    QList<Label> top;
    QList<Label> left;
    QList<Label> front;
    for (const auto& label: labels) {
        if (label.z1 <= cursor.z && cursor.z <= label.z2)
            top << label.top();
        if (label.x1 <= cursor.x && cursor.x <= label.x2)
            left << label.left();
        if (label.y1 <= cursor.y && cursor.y <= label.y2)
            front << label.front();
    }
    imgTop->setLabelList(top);
    imgLeft->setLabelList(left);
    imgFront->setLabelList(front);
}

void SubWindow::toggleActiveImage(RenderArea* img) {
    activeImg = activeImg == img ? nullptr : img;
    if (!activeImg)
        ui->statusBar->clearMessage();
}

void SubWindow::updateActions(bool open) {
    ui->actLoad->setEnabled(open);
    ui->actSave->setEnabled(open);
    ui->actClose->setEnabled(open);
    ui->actNew->setEnabled(open);
    ui->actRemove->setEnabled(open);
}

void SubWindow::on_actSwitch_triggered() {
    hide();
    mainWindow->show();
}

void SubWindow::on_actOpen_triggered() {
    QFileDialog dlg(this, "Open Folder");
    dlg.setFileMode(QFileDialog::Directory);
    if (dlg.exec() == QDialog::Accepted)
        if (load(dlg.selectedFiles().first())) {
            activeImg = nullptr;
            cursor = {0, 0, 0};
            labels.clear();
            refresh();
            for (auto* img: images())
                img->setVisible(true);
            updateActions(true);
        }
}

void SubWindow::on_actLoad_triggered() {
    QFileDialog dlg(this, "Load Label");
    dlg.setFileMode(QFileDialog::ExistingFile);
    if (dlg.exec() == QDialog::Accepted) {
        labels.clear();
        QFile file(dlg.selectedFiles().first());
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream istream(&file);
            istream >> labels;
        }
        refresh();
    }
}

void SubWindow::on_actSave_triggered() {
    QFileDialog dlg(this, "Save Label As");
    if (dlg.exec() == QDialog::Accepted) {
        QFile file(dlg.selectedFiles().first());
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream ostream(&file);
            ostream << labels;
        }
    }
}

void SubWindow::on_actClose_triggered() {
    for (auto* img: images())
        img->setVisible(false);
    updateActions(false);
}

void SubWindow::on_actNew_triggered() {
    CuboidDialog dlg(imgSize.h, imgSize.w, imgSize.d, this);
    if (dlg.exec() == QDialog::Accepted) {
        labels << CuboidLabel{dlg.x1(), dlg.y1(), dlg.z1(), dlg.x2(), dlg.y2(), dlg.z2(), dlg.text(), dlg.hasBorder() ? Label::getPen(dlg.color) : QPen(Qt::NoPen), QBrush(dlg.color)};
        refresh();
    }
}

void SubWindow::on_actRemove_triggered() {
    QInputDialog dlg(this, Qt::WindowCloseButtonHint);
    dlg.setWindowTitle("Remove by Tag");
    dlg.setLabelText("Tag Name:");
    dlg.adjustSize();
    if (dlg.exec() == QDialog::Accepted) {
        QString tag = dlg.textValue();
        bool changed = false;
        QList<CuboidLabel> rested;
        for (const auto& label: labels) {
            if (label.tag != tag)
                rested << label;
            else
                changed = true;
        }
        if (changed) {
            labels = rested;
            for (auto* img: images())
                img->remove(tag);
        }
    }
}

QImage SubWindow::getTop(int k) {
    QImage img(imgSize.w, imgSize.h, QImage::Format_ARGB32);
    for (int i = 0; i < img.height(); ++i) {
        QRgb* line = reinterpret_cast<QRgb*>(img.scanLine(i));
        for (int j = 0; j < img.width(); ++j)
            line[j] = reinterpret_cast<const QRgb*>(imgData[k].constBits())[i*imgSize.w + j];
    }
    return img;
}

QImage SubWindow::getLeft(int j) {
    QImage img(imgSize.h, imgSize.d, QImage::Format_ARGB32);
    for (int k = 0; k < img.height(); ++k) {
        QRgb* line = reinterpret_cast<QRgb*>(img.scanLine(k));
        for (int i = 0; i < img.width(); ++i)
            line[i] = reinterpret_cast<const QRgb*>(imgData[k].constBits())[i*imgSize.w + j];
    }
    return img;
}

QImage SubWindow::getFront(int i) {
    QImage img(imgSize.w, imgSize.d, QImage::Format_ARGB32);
    for (int k = 0; k < img.height(); ++k) {
        QRgb* line = reinterpret_cast<QRgb*>(img.scanLine(k));
        for (int j = 0; j < img.width(); ++j)
            line[j] = reinterpret_cast<const QRgb*>(imgData[k].constBits())[i*imgSize.w + j];
    }
    return img;
}

void SubWindow::setTop(int k) {
    imgTop->setPixmap(QPixmap::fromImage(getTop(k)));
    imgTop->adjustSize();
}

void SubWindow::setLeft(int j) {
    imgLeft->setPixmap(QPixmap::fromImage(getLeft(j)));
    imgLeft->adjustSize();
}

void SubWindow::setFront(int i) {
    imgFront->setPixmap(QPixmap::fromImage(getFront(i)));
    imgFront->adjustSize();
}

QList<RenderArea*> SubWindow::images() {
    return {imgTop, imgLeft, imgFront};
}
