#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QDragEnterEvent>
#include <QComboBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private:
    Ui::MainWindow *ui;
    QString path;
    QDialog *dialog;
    QSpinBox *spinc,*spinr,*spinMargin;
    QComboBox *comboBox_flow_type, *comboBox_zoom_type;
    bool isArray;
    enum Zoom_Type  {
        ORIGINAL,
        FIT_SMALL,
        FIT_BIG
    };
    enum Flow_Type{
        TopToBottom,
        LeftToRight
    };
    void add(QString spath);

private slots:
    void on_actionNew_triggered();
    void on_actionAdd_triggered();
    void on_actionArray_triggered();
    void on_actionZoomFit_triggered();
    void on_actionZoomOriginal_triggered();
    void on_actionSave_triggered();
    void on_actionAbout_triggered();
    void redraw();
    void customContextMenu(const QPoint &pos);

};

#endif // MAINWINDOW_H