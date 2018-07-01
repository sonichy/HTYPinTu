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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    move( (QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2 );
    path = "";
    isArray = false;

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
    if(path==""){
        path = QFileDialog::getOpenFileName(this,"打开图片", ".", "图片文件(*.jpg *.jpeg *.png *.bmp)");
    }else{
        path = QFileDialog::getOpenFileName(this,"打开图片", path, "图片文件(*.jpg *.jpeg *.png *.bmp)");
    }
    qDebug() << path;
    if(path.length() != 0){
        QListWidgetItem *LWIicon,*LWI;
        LWIicon = new QListWidgetItem(QIcon(path),"");
        LWIicon->setToolTip(path);
        ui->listWidgetIcon->insertItem(ui->listWidgetIcon->count()+1,LWIicon);
        ui->listWidget->setIconSize(ui->listWidget->size());
        LWI = new QListWidgetItem(QIcon(path),"");
        //LWI->setTextAlignment(Qt::AlignCenter);
        ui->listWidget->insertItem(ui->listWidget->count()+1,LWI);
    }
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
    if(path==""){
        path = QFileDialog::getSaveFileName(this,"保存图片","./未命名.jpg","图片文件(*.jpg *.png *.bmp)");
    }else{
        QDateTime time = QDateTime::currentDateTime();
        path = QFileInfo(path).absolutePath() + "/" + time.toString("yyyyMMddhhmmss") + ".jpg";
        path = QFileDialog::getSaveFileName(this,"保存图片",path,"图片文件(*.jpg *.png *.bmp)");
    }
    if(path.length() != 0){
        if(isArray){
            imageArray.save(path,0,100);
        }else{
            int pw=0, ph=0;
            // 竖排计算总高，横排计算总宽
            for(int i=0; i<ui->listWidget->count(); i++){
                QPixmap pixmapItem(ui->listWidgetIcon->item(i)->toolTip());
                if(ui->listWidget->flow() == QListView::TopToBottom){
                    ph += pixmapItem.height();
                }
                if(ui->listWidget->flow() == QListView::LeftToRight){
                    pw += pixmapItem.width();
                }
            }

            // 竖排取最大宽，横排取最大高
            if(ui->listWidget->flow() == QListView::TopToBottom){
                for(int i=0; i<ui->listWidget->count()-1; i++){
                    QPixmap pixmapItem1(ui->listWidgetIcon->item(i)->toolTip());
                    QPixmap pixmapItem2(ui->listWidgetIcon->item(i+1)->toolTip());
                    pw = qMax(pixmapItem1.width(),pixmapItem2.width());
                }
            }
            if(ui->listWidget->flow() == QListView::LeftToRight){
                for(int i=0; i<ui->listWidget->count()-1; i++){
                    QPixmap pixmapItem1(ui->listWidgetIcon->item(i)->toolTip());
                    QPixmap pixmapItem2(ui->listWidgetIcon->item(i+1)->toolTip());
                    ph = qMax(pixmapItem1.height(),pixmapItem2.height());
                }
            }

            qDebug() << pw << " X " << ph;
            QPixmap pixmap = QPixmap(pw,ph);
            pixmap.fill(Qt::white);
            QPainter painter(&pixmap);
            qDebug() << "count" << ui->listWidget->count();
            int x=0,y=0;
            for(int i=0; i<ui->listWidget->count(); i++){
                QPixmap pixmapItem(ui->listWidgetIcon->item(i)->toolTip());
                if(ui->listWidget->flow() == QListView::TopToBottom){
                    if(i>0){
                        QPixmap pixmapPrev(ui->listWidgetIcon->item(i-1)->toolTip());
                        y += pixmapPrev.height();
                    }
                }
                if(ui->listWidget->flow() == QListView::LeftToRight){
                    if(i>0){
                        QPixmap pixmapPrev(ui->listWidgetIcon->item(i-1)->toolTip());
                        x += pixmapPrev.width();
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
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰拼图 2.1\n一款基于Qt的拼图程序，支持横排、竖排、阵列。\n作者：黄颖\nE-mail: sonichy@163.com\n主页：sonichy.96.lt");
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
