#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QPainter>
#include <QDateTime>
#include <QMessageBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QMimeData>
#include <QMimeDatabase>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menuBar->hide();
    move((QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2 );
    path = "";
    isArray = false;

    comboBox_flow_type = new QComboBox;
    comboBox_flow_type->addItem(QIcon(":/HL.png"), "竖排", TopToBottom);
    comboBox_flow_type->addItem(QIcon(":/VL.png"), "横排", LeftToRight);
    ui->mainToolBar->insertWidget(ui->actionZoomOriginal, comboBox_flow_type);
    connect(comboBox_flow_type, SIGNAL(currentIndexChanged(int)), this, SLOT(redraw()));

    comboBox_zoom_type = new QComboBox;
    comboBox_zoom_type->addItem("原始大小", ORIGINAL);
    comboBox_zoom_type->addItem("适应小图", FIT_SMALL);
    comboBox_zoom_type->addItem("适应大图", FIT_BIG);
    ui->mainToolBar->insertWidget(ui->actionZoomOriginal, comboBox_zoom_type);
    connect(comboBox_zoom_type, SIGNAL(currentIndexChanged(int)), this, SLOT(redraw()));

    connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));
    connect(ui->listWidget->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(redraw()));

    dialog = new QDialog(this);
    dialog->setWindowTitle("阵列");
    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *hbox = new QHBoxLayout;
    QLabel *label = new QLabel("列：");
    spinc = new QSpinBox;
    spinc->setRange(1, 10);
    spinc->setValue(4);
    hbox->addWidget(label, 0, Qt::AlignCenter);
    hbox->addWidget(spinc);
    vbox->addLayout(hbox);
    label = new QLabel("行：");
    spinr = new QSpinBox;
    spinr->setRange(1, 10);
    spinr->setValue(2);
    hbox = new QHBoxLayout;
    hbox->addWidget(label, 0, Qt::AlignCenter);
    hbox->addWidget(spinr);
    vbox->addLayout(hbox);
    label = new QLabel("间隙：");
    spinMargin = new QSpinBox;
    spinMargin->setRange(-100, 100);
    spinMargin->setValue(40);
    hbox = new QHBoxLayout;
    hbox->addWidget(label, 0, Qt::AlignCenter);
    hbox->addWidget(spinMargin);
    vbox->addLayout(hbox);
    QPushButton *pushButton_confirm = new QPushButton("确定");
    QPushButton *pushButton_cancel = new QPushButton("取消");
    hbox = new QHBoxLayout;
    hbox->addWidget(pushButton_confirm);
    hbox->addWidget(pushButton_cancel);
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    connect(pushButton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(pushButton_cancel, SIGNAL(clicked()), dialog, SLOT(reject()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    ui->listWidget->clear();
    ui->label->setPixmap(QPixmap(""));
}

void MainWindow::on_actionAdd_triggered()
{
    isArray = false;
    if (path == "")
        path = ".";
    QStringList SL_path = QFileDialog::getOpenFileNames(this, "打开图片", path, "图片文件(*.jpg *.jpeg *.png *.bmp)");
    SL_path.sort();
    qDebug() << SL_path;
    for(int i=0; i<SL_path.length(); i++){
        QString filepath = SL_path.at(i);
        qDebug() << filepath;
        QString MIME = QMimeDatabase().mimeTypeForFile(filepath).name();
        QString filetype = MIME.left(MIME.indexOf("/"));
        qDebug() << MIME << filetype;
        if (filetype == "image") {
            add(filepath);
            path = filepath;
        }
    }
    redraw();
}

void MainWindow::add(QString spath)
{
    QListWidgetItem *LWI = new QListWidgetItem(QIcon(spath), "");
    LWI->setToolTip(spath);
    ui->listWidget->insertItem(ui->listWidget->count() + 1, LWI);
}

void MainWindow::on_actionZoomFit_triggered()
{
}

void MainWindow::on_actionZoomOriginal_triggered()
{
}

void MainWindow::on_actionSave_triggered()
{
    QDateTime time = QDateTime::currentDateTime();
    QString filename = time.toString("yyyyMMddhhmmss") + ".jpg";
    path = QFileInfo(path).absolutePath() + "/" + filename;
    path = QFileDialog::getSaveFileName(this, "保存图片", path,"图片文件(*.jpg *.png *.bmp)");
    if (path.length() != 0) {
        const QPixmap *pixmap = ui->label->pixmap();
        pixmap->save(path, nullptr, 100);
        ui->statusBar->showMessage("保存 " + path);
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰拼图 2.4\n一款基于 Qt 的拼图程序，支持横排、竖排、阵列。\n作者：海天鹰\nE-mail: sonichy@163.com\n主页：https://github.com/sonichy\nDragDropMode(QAbstractItemView::InternalMove) + ViewMode::ListMode(default)可以排序");
    aboutMB.setIconPixmap(QPixmap(":/HTYPinTu.png").scaled(200,200));
    aboutMB.exec();
}

void MainWindow::on_actionArray_triggered()
{
    if (dialog->exec() == QDialog::Accepted) {
        if (ui->listWidget->count() < 1) {
            ui->label->setPixmap(QPixmap(""));
            return;
        }
        isArray = true;
        QPixmap pixmap(ui->listWidget->item(0)->toolTip());
        QPixmap pixmap_array = QPixmap((pixmap.width() + spinMargin->value()) * spinc->value() + spinMargin->value(), (pixmap.height() + spinMargin->value()) * spinr->value() + spinMargin->value());
        pixmap_array.fill(Qt::white);
        QPainter painter(&pixmap_array);
        for (int r=0; r< spinr->value(); r++) {
            for (int c=0; c<spinc->value(); c++) {
                painter.drawPixmap((pixmap.width() + spinMargin->value()) * c + spinMargin->value(), (pixmap.height() + spinMargin->value()) * r + spinMargin->value(), pixmap);
            }
        }
        ui->label->setPixmap(pixmap_array);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    qDebug() << e->mimeData()->formats().at(0);
    //if(e->mimeData()->hasFormat("text/uri-list")) //只能打开文本列表
    e->acceptProposedAction(); //可以在这个窗口部件上拖放对象
}

void MainWindow::dropEvent(QDropEvent *e) //释放对方时，执行的操作
{
    QList<QUrl> urls = e->mimeData()->urls();
    if(urls.isEmpty())
        return;

    foreach (QUrl url, urls) {
        qDebug() << url.toString();
        QString filepath = url.toLocalFile();
        qDebug() << filepath;
        QString MIME = QMimeDatabase().mimeTypeForFile(filepath).name();
        QString filetype = MIME.left(MIME.indexOf("/"));
        qDebug() << MIME << filetype;
        if (filetype == "image") {
            add(filepath);
            path = filepath;
        }
    }
    redraw();
}

void MainWindow::redraw()
{
    if (ui->listWidget->count() < 1) {
        ui->label->setPixmap(QPixmap(""));
        return;
    }
    qDebug() << "redraw";
    Flow_Type flow_type = static_cast<Flow_Type>(comboBox_flow_type->currentIndex());
    Zoom_Type zoom_type = static_cast<Zoom_Type>(comboBox_zoom_type->currentIndex());
    int pw=0, ph=0;
    QList<int> list_width, list_height;
    for(int i=0; i<ui->listWidget->count(); i++){
        QPixmap pixmap(ui->listWidget->item(i)->toolTip());
        list_width << pixmap.width();
        list_height << pixmap.height();
    }
    std::sort(list_width.begin(), list_width.end());
    std::sort(list_height.begin(), list_height.end());

    if (flow_type == TopToBottom) {
        // 取竖排宽度
        if (zoom_type == FIT_SMALL) {
            pw = list_width.first();
        } else {
            pw = list_width.last();
        }
    } else {
        // 取横排高度
        if (zoom_type == FIT_SMALL) {
            ph = list_height.first();
        } else {
            ph = list_height.last();
        }
    }

    // 竖排计算总高，横排计算总宽
    for (int i=0; i<ui->listWidget->count(); i++) {
        QPixmap pixmap(ui->listWidget->item(i)->toolTip());
        if (flow_type == TopToBottom) { //竖排
            if (zoom_type == FIT_SMALL) {
                if (pixmap.width() > pw)
                    pixmap = pixmap.scaledToWidth(pw);
            }
            ph += pixmap.height();
        } else { // 横排
            if (zoom_type == FIT_SMALL) {
                pixmap = pixmap.scaledToHeight(ph);
            }
            pw += pixmap.width();
        }
    }

    qDebug() << pw << " X " << ph;
    QPixmap pixmap = QPixmap(pw,ph);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    qDebug() << "count" << ui->listWidget->count();
    int x=0, y=0;
    for (int i=0; i<ui->listWidget->count(); i++) {
        QPixmap pixmap_item(ui->listWidget->item(i)->toolTip());
        if (flow_type == TopToBottom) { //竖排
            if (i>0) {
                QPixmap pixmap_prev(ui->listWidget->item(i-1)->toolTip());
                if(zoom_type == FIT_SMALL){
                    pixmap_prev = pixmap_prev.scaledToWidth(pw);
                }
                y += pixmap_prev.height();
            }
            if (zoom_type == FIT_SMALL) {
                pixmap_item = pixmap_item.scaledToWidth(pw, Qt::SmoothTransformation);
            }
        } else { // 横排
            if (i>0) {
                QPixmap pixmap_prev(ui->listWidget->item(i-1)->toolTip());
                if(zoom_type == FIT_SMALL){
                    pixmap_prev = pixmap_prev.scaledToHeight(ph);
                }
                x += pixmap_prev.width();
            }
            if (zoom_type == FIT_SMALL) {
                pixmap_item = pixmap_item.scaledToHeight(ph, Qt::SmoothTransformation);
            }
        }
        painter.drawPixmap(x, y, pixmap_item);
    }
    ui->label->setPixmap(pixmap);

}


void MainWindow::customContextMenu(const QPoint &pos)
{
    QAction *action_delete = nullptr;
    QList<QAction*> actions;
    QModelIndex index = ui->listWidget->indexAt(pos);
    if (index.isValid()) {
        action_delete = new QAction(this);
        action_delete->setText("删除");
        action_delete->setIcon(QIcon::fromTheme("edit-delete"));
        actions.append(action_delete);
    }
    QAction *result_action = QMenu::exec(actions, ui->listWidget->mapToGlobal(pos));
    if (result_action == action_delete) {
        for (int i=0; i<ui->listWidget->selectedItems().length(); i++) {
            QListWidgetItem *LWI = ui->listWidget->selectedItems().at(i);
            ui->listWidget->removeItemWidget(LWI);
            delete LWI;
        }
        redraw();
        return;
    }
}