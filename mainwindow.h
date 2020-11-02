#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGraphicsView>
#include "stdlib.h"
#include "radiolabel.h"
#include "airport.h"
#include "radar.h"
#include "dialog.h"

extern Airport airport;
extern Runway runway;
extern Runway runway1;
extern PlaneQueue q;
extern PlaneList l;
extern PlaneQueue q1;
extern PlaneList l1;
typedef struct{
    QString id;
    int time;
}Record;
typedef list<Record>RecordList ;
extern RecordList recordList;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void timerEvent();

protected:
     void timerEvent(QTimerEvent *event);    //定时器事件

private slots:
    void on_pushButton_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_tableView_2_doubleClicked(const QModelIndex &index);

    void on_pushButton_2_clicked();

    void on_pushButton_5_clicked();

    void update();

    void on_begin_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    int           timerId;       //定时器ID
    QTimer *pTimer;


};

#endif // MAINWINDOW_H
