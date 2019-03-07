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
    move((QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2 );
    path = "";
    isArray = false;

    comboBox_zoom_type = new QComboBox;
    comboBox_zoom_type->addItem("原始大小",ORIGINAL);
    comboBox_zoom_type->addItem("适应小图",FIT_SMALL);
    comboBox_zoom_type->addItem("适应大图",FIT_BIG);
    ui->mainToolBar->addWidget(comboBox_zoom_type);

    connect(ui->listWidgetIcon->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(reorder()));

    dialog = new QDialog(this);
    dialog->setWindowTitle("阵列");
    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *hbox = new QHBoxLayout;
    QLabel *label = new QLabel("列：");
    spinc = new QSpinBox;
    spinc->setRange(1,10);
    spinc->setValue(4);
    hbox->addWidget(label,0,Qt::AlignCenter);
    hbox->addWidget(spinc);
    vbox->addLayout(hbox);
    label = new QLabel("行：");
    spinr = new QSpinBox;
    spinr->setRange(1,10);
    spinr->setValue(2);
    hbox = new QHBoxLayout;
    hbox->addWidget(label,0,Qt::AlignCenter);
    hbox->addWidget(spinr);
    vbox->addLayout(hbox);
    label = new QLabel("间隙：");
    spinMargin = new QSpinBox;
    spinMargin->setRange(0,200);
    spinMargin->setValue(40);
    hbox = new QHBoxLayout;
    hbox->addWidget(label,0,Qt::AlignCenter);
    hbox->addWidget(spinMargin);
    vbox->addLayout(hbox);
    QPushButton *pushButtonConfirm = new QPushButton("确定");
    QPushButton *pushButtonCancel = new QPushButton("取消");
    hbox = new QHBoxLayout;
    hbox->addWidget(pushButtonConfirm);
    hbox->addWidget(pushButtonCancel);
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    connect(pushButtonConfirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(pushButtonCancel, SIGNAL(clicked()), dialog, SLOT(reject()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    ui->listWidgetIcon->clear();
    ui->listWidget->clear();
}

void MainWindow::on_actionAdd_triggered()
{
    isArray = false;
    if (path == "") path = ".";
    QStringList SL_path = QFileDialog::getOpenFileNames(this, "打开图片", path, "图片文件(*.jpg *.jpeg *.png *.bmp)");
    qDebug() << SL_path;
    //    if(path.length() != 0){
    //        add(path);
    //    }
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
}

void MainWindow::add(QString spath)
{
    QListWidgetItem *LWIicon, *LWI;
    LWIicon = new QListWidgetItem(QIcon(spath), "");
    LWIicon->setToolTip(spath);
    ui->listWidgetIcon->insertItem(ui->listWidgetIcon->count() + 1, LWIicon);
    ui->listWidget->setIconSize(ui->listWidget->size());
    LWI = new QListWidgetItem(QIcon(spath), "");
    //LWI->setTextAlignment(Qt::AlignCenter);
    ui->listWidget->insertItem(ui->listWidget->count()+1, LWI);
}

void MainWindow::on_actionVertical_triggered()
{
    ui->listWidget->setFlow(QListView::TopToBottom);
}

void MainWindow::on_actionHorizontal_triggered()
{
    ui->listWidget->setFlow(QListView::LeftToRight);
}

void MainWindow::on_actionZoomFit_triggered()
{
    ui->listWidget->setIconSize(ui->listWidget->size()/ui->listWidget->count());
    //ui->listWidget->setIconSize(QSize(ui->listWidget->width()/ui->listWidget->count(),ui->listWidget->height()/ui->listWidget->count()));
}

void MainWindow::on_actionZoomOriginal_triggered()
{
    ui->listWidget->setIconSize(ui->listWidget->size());
}

void MainWindow::on_actionSave_triggered()
{
    QDateTime time = QDateTime::currentDateTime();
    QString filename = time.toString("yyyyMMddhhmmss") + ".jpg";
    path = QFileInfo(path).absolutePath() + "/" + filename;
    path = QFileDialog::getSaveFileName(this, "保存图片", path,"图片文件(*.jpg *.png *.bmp)");
    if(path.length() != 0){
        if(isArray){
            imageArray.save(path,0,100);
        }else{
            Zoom_Type zoom_type = (Zoom_Type)(comboBox_zoom_type->currentIndex());
            int pw=0, ph=0;
            QList<int> list_width, list_height;
            for(int i=0; i<ui->listWidgetIcon->count(); i++){
                QPixmap pixmap(ui->listWidgetIcon->item(i)->toolTip());
                list_width << pixmap.width();
                list_height << pixmap.height();
            }
            std::sort(list_width.begin(), list_width.end());
            std::sort(list_height.begin(), list_height.end());

            // 取竖排宽度
            if(ui->listWidget->flow() == QListView::TopToBottom){
                if(zoom_type == FIT_SMALL){
                    pw = list_width.first();
                }else{
                    pw = list_width.last();
                }
            }

            // 取横排高度
            if(ui->listWidget->flow() == QListView::LeftToRight){
                if(zoom_type == FIT_SMALL){
                    ph = list_height.first();
                }else{
                    ph = list_height.last();
                }

            }

            // 竖排计算总高，横排计算总宽
            for(int i=0; i<ui->listWidget->count(); i++){
                QPixmap pixmap(ui->listWidgetIcon->item(i)->toolTip());
                if(ui->listWidget->flow() == QListView::TopToBottom){
                    if(zoom_type == FIT_SMALL){
                        if(pixmap.width() > pw)
                            pixmap = pixmap.scaledToWidth(pw);
                    }
                    ph += pixmap.height();
                }
                if(ui->listWidget->flow() == QListView::LeftToRight){
                    if(zoom_type == FIT_SMALL){
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
            for(int i=0; i<ui->listWidget->count(); i++){
                QPixmap pixmapItem(ui->listWidgetIcon->item(i)->toolTip());
                if(ui->listWidget->flow() == QListView::TopToBottom){
                    if(i>0){
                        QPixmap pixmapPrev(ui->listWidgetIcon->item(i-1)->toolTip());
                        if(zoom_type == FIT_SMALL){
                            pixmapPrev = pixmapPrev.scaledToWidth(pw);
                        }
                        y += pixmapPrev.height();
                    }
                    if(zoom_type == FIT_SMALL){
                        pixmapItem = pixmapItem.scaledToWidth(pw, Qt::SmoothTransformation);
                    }
                }
                if(ui->listWidget->flow() == QListView::LeftToRight){
                    if(i>0){
                        QPixmap pixmapPrev(ui->listWidgetIcon->item(i-1)->toolTip());
                        if(zoom_type == FIT_SMALL){
                            pixmapPrev = pixmapPrev.scaledToHeight(ph);
                        }
                        x += pixmapPrev.width();
                    }
                    if(zoom_type == FIT_SMALL){
                        pixmapItem = pixmapItem.scaledToHeight(ph, Qt::SmoothTransformation);
                    }
                }
                painter.drawPixmap(x,y,pixmapItem);
            }
            pixmap.save(path,0,100);
        }
        ui->statusBar->showMessage("保存 " + path);
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰拼图 2.3\n一款基于 Qt 的拼图程序，支持横排、竖排、阵列。\n作者：黄颖\nE-mail: sonichy@163.com\n主页：https://github.com/sonichy\nDragDropMode(QAbstractItemView::InternalMove) + ViewMode::ListMode(default)可以排序");
    aboutMB.setIconPixmap(QPixmap(":/icon.png").scaled(200,200));
    aboutMB.exec();
}

void MainWindow::on_actionArray_triggered()
{
    if(dialog->exec()==QDialog::Accepted){
        isArray = true;
        ui->listWidget->clear();
        QImage image( ui->listWidgetIcon->item(0)->toolTip() );
        imageArray = QImage( (image.width() + spinMargin->value())*spinc->value() + spinMargin->value(), (image.height() + spinMargin->value())*spinr->value()+ spinMargin->value(), QImage::Format_RGB32 );
        imageArray.fill(Qt::white);
        QPainter painter(&imageArray);
        for(int r=0; r< spinr->value(); r++){
            for(int c=0; c<spinc->value(); c++){
                painter.drawImage((image.width() + spinMargin->value())*c + spinMargin->value(), (image.height() + spinMargin->value())*r + spinMargin->value(), image);
            }
        }
        ui->listWidget->addItem(new QListWidgetItem(QIcon(QPixmap::fromImage(imageArray)),""));
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
}

void MainWindow::reorder()
{
    qDebug() << "reorder";
    ui->listWidget->clear();
    for (int i=0; i<ui->listWidgetIcon->count(); i++) {
        QListWidgetItem *LWI = new QListWidgetItem(QIcon(ui->listWidgetIcon->item(i)->toolTip()), "");
        ui->listWidget->insertItem(i, LWI);
    }
}