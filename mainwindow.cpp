#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QPainter>
#include <QDateTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);    
    move((QApplication::desktop()->width()-width())/2,(QApplication::desktop()->height()-height())/2);    
    path = "";
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
        int pw=0,ph=0;
        for(int i=0;i<ui->listWidget->count();i++){
            QPixmap pixmapItem(ui->listWidgetIcon->item(i)->toolTip());
            if(ui->listWidget->flow() == QListView::TopToBottom){
                ph += pixmapItem.height();
            }
            if(ui->listWidget->flow() == QListView::LeftToRight){
                pw += pixmapItem.width();                
            }
        }
        QPixmap pixmapItem(ui->listWidgetIcon->item(0)->toolTip());
        if(ui->listWidget->flow() == QListView::TopToBottom){
            pw = pixmapItem.width();
        }
        if(ui->listWidget->flow() == QListView::LeftToRight){
            ph = pixmapItem.height();
        }
        qDebug() << pw << " X " << ph;
        QPixmap pixmap = QPixmap(pw,ph);
        pixmap.fill(Qt::white);
        QPainter painter(&pixmap);
        qDebug() << "count" << ui->listWidget->count();
        int x=0,y=0;
        for(int i=0;i<ui->listWidget->count();i++){
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
        ui->statusBar->showMessage("保存 " + path);
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰拼图 1.0\n一款基于Qt的拼图程序。\n作者：黄颖\nE-mail: sonichy@163.com\n主页：sonichy.96.lt");
    aboutMB.setIconPixmap(QPixmap(":/icon.png").scaled(200,200));
    aboutMB.exec();
}
