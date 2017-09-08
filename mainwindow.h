#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString path;

private slots:
    void on_actionNew_triggered();
    void on_actionAdd_triggered();
    void on_actionVertical_triggered();
    void on_actionHorizontal_triggered();
    void on_actionZoomFit_triggered();
    void on_actionZoomOriginal_triggered();
    void on_actionSave_triggered();
    void on_actionAbout_triggered();
};

#endif // MAINWINDOW_H
