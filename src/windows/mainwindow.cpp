#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "subwindow.h"
#include "labeldialog.h"
#include "util.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    subWindow(new SubWindow(this, parent)),
    ui(new Ui::MainWindow),
    canvas(new RenderArea),
    area(new QScrollArea),
    dockStatus(new QDockWidget("Label Status")),
    status(new QListWidget),
    dockMagnifier(new QDockWidget("Magnifier")),
    magnifier(new QLabel)
{
    ui->setupUi(this);
    setCentralWidget(area);
    area->setBackgroundRole(QPalette::Dark);
    area->setAlignment(Qt::AlignCenter);
    area->setWidget(canvas);
    area->setMouseTracking(true);
    area->installEventFilter(this);
    canvas->setVisible(false);

    status->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    status->setMinimumWidth(150);
    magnifier->setAlignment(Qt::AlignCenter);
    magnifier->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    magnifier->setMinimumSize(200, 200);
    dockStatus->setWidget(status);
    dockMagnifier->setWidget(magnifier);
    for (auto* dock: {dockStatus, dockMagnifier}) {
        dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        addDockWidget(Qt::RightDockWidgetArea, dock);
        ui->menuView->addAction(dock->toggleViewAction());
        dock->close();
    }
    updateActions();

    connect(canvas, &RenderArea::painted, [=] () {
        QPoint pos = canvas->mapFromGlobal(QCursor::pos());
        if (canvas->rect().contains(pos))
            updateMagnifier(pos);
    });
    connect(canvas, &RenderArea::mouseMoved, [=] (const QPoint& pos) {
        ui->statusBar->showMessage(QString::asprintf("Cursor: (%d, %d)", pos.x(), pos.y()));
        updateMagnifier(pos);
    });
    connect(canvas, &RenderArea::selectedLabelChanged, ui->actRemove, &QAction::setEnabled);
    connect(canvas, &RenderArea::selectedLabelChanged, this, &MainWindow::updateStatus);
    connect(canvas, &RenderArea::labelChanged, this, &MainWindow::updateUndoList);
    connect(canvas, &RenderArea::labelUpdated, this, &MainWindow::updateActions);
}

MainWindow::~MainWindow() {
    delete subWindow;
    delete ui;
}

bool MainWindow::hasImage() const {
    return !canvas->pixmap()->isNull();
}

bool MainWindow::loadFile() {
    if (files.empty()) {
        closeFile();
        return false;
    }
    QImageReader reader(*files.it);
    reader.setAutoTransform(true);
    QPixmap pixmap = QPixmap::fromImageReader(&reader);
    if (pixmap.isNull()) {
        closeFile();
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(), QString("Cannot load %1: %2").arg(QDir::toNativeSeparators(*files.it), reader.errorString()));
        return false;
    }
    canvas->setPixmap(pixmap);
    canvas->adjustSize();
    canvas->setVisible(true);
    dockStatus->show();
    magnifier->setPixmap(QPixmap());
    undoList.clear();
    canvas->loadLabels(*files.it+".dat");
    return true;
}

void MainWindow::closeFile() {
    canvas->setVisible(false);
    canvas->setPixmap(QPixmap());
    magnifier->setPixmap(QPixmap());
    updateActions();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == area) {
        if (event->type() == QEvent::MouseMove)
            ui->statusBar->clearMessage();
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::updateStatus(Label* label) {
    while (auto* item = status->item(0))
        delete item;
    if (label) {
        status->addItem("Tag: "+label->tag);
        status->addItem("Color: "+label->brush.color().name().toUpper());
    }
}

void MainWindow::updateActions() {
    ui->actLoad->setEnabled(hasImage());
    ui->actSave->setEnabled(hasImage());
    ui->actSaveAs->setEnabled(hasImage());
    ui->actPrev->setEnabled(files.hasPrev());
    ui->actNext->setEnabled(files.hasNext());
    ui->actClose->setEnabled(hasImage());
    ui->actCloseAll->setEnabled(hasImage());
    ui->actNew->setEnabled(hasImage());
    ui->actRemoveAll->setEnabled(hasImage());
    ui->actUndo->setEnabled(undoList.hasPrev());
    ui->actRedo->setEnabled(undoList.hasNext());
}

void MainWindow::updateUndoList() {
    if (!undoList.atEnd())
        undoList.list.erase(undoList.next(), undoList.list.end());
    undoList.list << canvas->labelList();
    undoList.moveToLast();
    updateActions();
}

void MainWindow::updateMagnifier(const QPoint& pos) {
    QSize size = magnifier->size()/2;
    int maxWidth = canvas->size().width();
    int maxHeight = canvas->size().height();
    int w = qMin(maxWidth, size.width());
    int h = qMin(maxHeight, size.height());
    QPoint topLeft(qBound(0, pos.x() - w/2, maxWidth - w), qBound(0, pos.y() - h/2, maxHeight - h));
    QPixmap pixmap = canvas->grab(QRect(topLeft, QSize(w, h)));
    QPainter painter(&pixmap);
    painter.drawPixmap(pos - topLeft, QIcon(":/res/arrow.cur").pixmap(32));
    magnifier->setPixmap(pixmap.scaled(QSize(w, h)*2));
}

void MainWindow::on_actOpen_triggered() {
    QFileDialog dlg(this, "Open File");
    dlg.setFileMode(QFileDialog::ExistingFiles);
    dlg.setNameFilters({QString("All Supported Files (%1)").arg(imageFilters().join(' ')), "All Files (*)"});
    if (dlg.exec() == QDialog::Accepted) {
        files.list = dlg.selectedFiles();
        files.moveToBegin();
        loadFile();
    }
}

void MainWindow::on_actOpenFolder_triggered() {
    QFileDialog dlg(this, "Open Folder");
    dlg.setFileMode(QFileDialog::Directory);
    if (dlg.exec() == QDialog::Accepted) {
        QDir dir(dlg.selectedFiles().first());
        files.clear();
        for (const QString& fileName: dir.entryList(imageFilters()))
            files.list << dir.filePath(fileName);
        files.moveToBegin();
        loadFile();
    }
}

void MainWindow::on_actLoad_triggered() {
    QFileDialog dlg(this, "Load Label");
    dlg.setFileMode(QFileDialog::ExistingFile);
    if (dlg.exec() == QDialog::Accepted)
        canvas->loadLabels(dlg.selectedFiles().first());
}

void MainWindow::on_actSave_triggered() {
    canvas->saveLabels(*files.it+".dat");
}

void MainWindow::on_actSaveAs_triggered() {
    QFileDialog dlg(this, "Save Label As");
    if (dlg.exec() == QDialog::Accepted)
        canvas->saveLabels(dlg.selectedFiles().first());
}

void MainWindow::on_actPrev_triggered() {
    --files.it;
    loadFile();
}

void MainWindow::on_actNext_triggered() {
    ++files.it;
    loadFile();
}

void MainWindow::on_actClose_triggered() {
    files.it = files.list.erase(files.it);
    if (files.empty())
        closeFile();
    else {
        if (files.atEnd())
            --files.it;
        loadFile();
    }
}

void MainWindow::on_actCloseAll_triggered() {
    files.clear();
    closeFile();
    dockStatus->close();
    dockMagnifier->close();
}

void MainWindow::on_actNew_triggered() {
    LabelDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
        canvas->newLabel({dlg.text(), dlg.shape, dlg.hasBorder() ? Label::getPen(dlg.color) : QPen(Qt::NoPen), QBrush(dlg.color), QPainterPath()});
}

void MainWindow::on_actRemove_triggered() {
    canvas->removeSelectedLabel();
}

void MainWindow::on_actRemoveAll_triggered() {
    QDialog dlg(this);
    auto* edit = initInputDialog(&dlg, "Remove by Tag", "Tag Name:");
    if (dlg.exec() == QDialog::Accepted)
        canvas->remove(edit->text());
}

void MainWindow::on_actUndo_triggered(){
    canvas->setLabelList(*--undoList.it);
}

void MainWindow::on_actRedo_triggered(){
    canvas->setLabelList(*++undoList.it);
}

void MainWindow::on_actSwitch_triggered() {
    hide();
    subWindow->show();
}

QLineEdit* MainWindow::initInputDialog(QDialog* dlg, const QString& title, const QString& labelText) {
    dlg->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dlg->setWindowTitle(title);
    auto* label = new QLabel(labelText, dlg);
    auto* edit = new QLineEdit(dlg);
    auto* hLayout = new QHBoxLayout;
    hLayout->addWidget(label);
    hLayout->addWidget(edit);
    auto* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dlg);
    auto* vLayout = new QVBoxLayout;
    vLayout->addLayout(hLayout);
    vLayout->addWidget(btnBox);
    dlg->setLayout(vLayout);
    connect(btnBox, &QDialogButtonBox::accepted, dlg, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, dlg, &QDialog::reject);
    return edit;
}
