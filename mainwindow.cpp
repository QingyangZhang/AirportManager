#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cstdlib>
#include "QLabel"
#include <QDebug>
#include "radar.h"
#include <QStandardItemModel>
#include <QTime>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

Runway runway=Runway();
Runway runway1=Runway();
PlaneQueue q;
PlaneList l;
PlaneQueue q1;
PlaneList l1;
RecordList recordList;

static int timeSteps=0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
        //连接数据库
        QSqlDatabase database;
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("D:\planeDB.db");
        if (!database.open()){
            qDebug() << "Error: Failed to connect database." << database.lastError();
        }
        else{
            qDebug() << "Succeed to connect database." ;
        }
        //执行查询操作
        QSqlQuery sql_query;
        sql_query.exec("select * from PlaneInfo");
            if(!sql_query.exec()){
                qDebug()<<sql_query.lastError();
            }
            else
            {
                while(sql_query.next())
                {
                    QString name = sql_query.value(0).toString();
                    double speed=sql_query.value(1).toDouble();
                    double consumeRate=sql_query.value(2).toDouble();
                    double angle=sql_query.value(3).toDouble();
                    double fuel=sql_query.value(4).toDouble();
                    int in_out=sql_query.value(5).toInt();
                    double distance=sql_query.value(6).toDouble();
                    //为数据库中的航班创建实例
                    Plane p=Plane(name.toStdString(),speed,consumeRate,distance,angle,fuel,in_out);                    
                    if(in_out==1){//将航班加入起降队列中
                        q.push(p);
                    }else if (in_out==0) {
                        q1.push(p);
                    }
                }
            }
     //初始化跑道
    runway1=Runway(400);
    runway=Runway(400);
    //设置定时器，开始模拟
    this->timerId = startTimer(1000);
    this->setObjectName("mainWindow");
    this->setStyleSheet("#mainWindow{border-image:url(:/bg2.jpg);}");

    this->pTimer = new QTimer;
        //绑定定时器的信号

    connect(this->pTimer,SIGNAL(timeout()),this,SLOT(update()));
    //pTimer->start(1000);

}

void MainWindow::update()
{
    timeSteps++;

    if(1){
        PlaneQueue temp;
        PlaneQueue temp1;

        while(!l.empty())l.pop_front();
        while(!l1.empty())l1.pop_front();

        if (runway.busy) {//更新降落跑道飞机状态
                    runway.landingPlane.checkAll();
                    if (runway.landingPlane.getDistance() < 0) {//如果已完成降落
                        runway.landingPlane.setStatus(3);//设置状态为：到达
                        //将该航班的id和到达时间保存至记录中
                        Record r={QString::fromStdString(runway.landingPlane.getID()),timeSteps};
                        recordList.push_back(r);
                        //清空跑道
                        runway.busy = !runway.busy;
                    }
                    l.push_front(runway.landingPlane);
                }
        if(runway1.busy){//更新起飞跑道飞机状态
                runway1.landingPlane.checkAll();
                if(runway1.landingPlane.getDistance()>4){//已经完成起飞的
                    runway1.landingPlane.setStatus(0);//设置状态为：爬升
                    runway1.busy=!runway1.busy;
                }
                l1.push_front(runway1.landingPlane);
        }
                //更新降落飞机队列中飞机的状态
                while (!q.empty()) {
                    Plane p = q.top();
                    q.pop();
                    l.push_front(p);
                    p.checkAll();
                    if (p.getDistance() < 400 && p.getStatus() == 0) {//对于进入10km范围内的飞机
                        p.setStatus(1);//设置其状态为：盘旋
                    }
                    temp.push(p);
                }
                q = temp;
                //更新起飞飞机队列中飞机的状态
                while(!q1.empty()){
                    Plane p=q1.top();
                    q1.pop();
                    l1.push_front(p);
                    p.checkAll();//更新飞机飞行状态
                    if(p.getDistance()>10){
                        p.setStatus(0);//对于已经飞离10km范围内的飞机，更新其状态为：巡航
                    }
                    if(p.getDistance()<1500){//飞离1500km范围内的飞机，不再进行追踪
                        temp1.push(p);
                    }
                    if(p.getDistance()>1500){
                        Record r={QString::fromStdString(p.getID()),timeSteps};
                        recordList.push_back(r);
                    }
                }
                q1=temp1;
                //如果跑道空闲，且有飞机处于等待中，允许其降落
                if (!runway.busy && !q.empty()) {
                    Plane p = q.top();//取出队列最前端的飞机
                    if (p.getStatus() == 1) {
                        q.pop();
                        p.setStatus(2);//更新飞机状态为下降
                        runway.landingPlane = p;//降落
                        runway.busy = !runway.busy;//设置跑道状态为繁忙
                    }
                }

                QStandardItemModel* model = new QStandardItemModel(this);
                model->setColumnCount(3);
                //设置表头数据
                model->setHeaderData(0,Qt::Horizontal, "进港航班");
                model->setHeaderData(1,Qt::Horizontal, "距离(km)");
                model->setHeaderData(2,Qt::Horizontal, "状态");
                model->setRowCount(l.size());
                //声明迭代器
                list<Plane>::iterator itor;

                itor = l.begin();
                for(int i=0;itor!=l.end();i++,itor++)
                {

                            //修改model中的数据
                           model->setItem(i, 0,new QStandardItem(QString::fromStdString((*itor).getID())));
                           model->setItem(i, 1,new QStandardItem(QString::number((*itor).getDistance())));
                           switch ((*itor).getStatus()) {
                                case 0:model->setItem(i, 2,new QStandardItem("巡航"));break;
                                case 1:model->setItem(i, 2,new QStandardItem("盘旋"));break;
                                case 2:model->setItem(i, 2,new QStandardItem("下降"));break;
                                case 3:model->setItem(i, 2,new QStandardItem("着陆"));break;
                                case 4:model->setItem(i, 2,new QStandardItem("爬升"));break;
                           }
                 }
                //设置前端tableView组件显示更新后的model数据
                ui->tableView->setModel(model);

                QStandardItemModel* model1 = new QStandardItemModel(this);
                model1->setColumnCount(3);
                model1->setHeaderData(0,Qt::Horizontal, "离港航班");
                model1->setHeaderData(1,Qt::Horizontal, "距离(km)");
                model1->setHeaderData(2,Qt::Horizontal, "状态");


                model1->setRowCount(l1.size());

                list<Plane>::iterator itor1;
                itor1 = l1.begin();

                for(int i=0;itor1!=l1.end();i++,itor1++)
                {
                      // model->setHeaderData(i,Qt::Vertical, i);


                           model1->setItem(i, 0,new QStandardItem(QString::fromStdString((*itor1).getID())));
                           model1->setItem(i, 1,new QStandardItem(QString::number((*itor1).getDistance())));
                           //model1->setItem(i, 2,new QStandardItem(QString::number((*itor1).getStatus())));
                           switch ((*itor1).getStatus()) {
                                case 0:model1->setItem(i, 2,new QStandardItem("巡航"));break;
                                case 1:model1->setItem(i, 2,new QStandardItem("盘旋"));break;
                                case 2:model1->setItem(i, 2,new QStandardItem("下降"));break;
                                case 3:model1->setItem(i, 2,new QStandardItem("着陆"));break;
                                case 4:model1->setItem(i, 2,new QStandardItem("爬升"));break;
                           }

                 }

                ui->tableView->setModel(model);
                ui->tableView->verticalHeader()->hide();
                ui->tableView->verticalHeader()->setDefaultSectionSize(80);
                ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                ui->tableView_2->setModel(model1);
                ui->tableView_2->verticalHeader()->hide();
                ui->tableView_2->verticalHeader()->setDefaultSectionSize(80);
                ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选中模式为选中行
                ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
                ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选中模式为选中行
                ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
                //ui->tableView->setSelectionMode( QAbstractItemView::SingleSelection);//设置选中单个
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    exit(0);
}

void MainWindow::timerEvent(QTimerEvent *event){
    /*
    if(timerId == event->timerId()){
        PlaneQueue temp;
        PlaneQueue temp1;
        //l.clear();
        if (runway.busy) {//更新降落跑道飞机状态
                    runway.landingPlane.checkAll();
                    if (runway.landingPlane.getDistance() < 0) {//如果已完成降落
                        runway.landingPlane.setStatus(3);//设置状态为：到达
                        //将该航班的id和到达时间保存至记录中
                        Record r={QString::fromStdString(runway.landingPlane.getID()),1};
                        recordList.push_back(r);
                        //清空跑道
                        runway.busy = !runway.busy;
                    }
                    l.push_front(runway.landingPlane);
                }
        if(runway1.busy){//更新起飞跑道飞机状态
                runway1.landingPlane.checkAll();
                if(runway1.landingPlane.getDistance()>4){//已经完成起飞的
                    runway1.landingPlane.setStatus(0);//设置状态为：爬升
                    runway1.busy=!runway1.busy;
                }
                l1.push_front(runway1.landingPlane);
        }
                //更新降落飞机队列中飞机的状态
                while (!q.empty()) {
                    Plane p = q.top();
                    q.pop();
                    l.push_front(p);
                    p.checkAll();
                    if (p.getDistance() < 400 && p.getStatus() == 0) {//对于进入10km范围内的飞机
                        p.setStatus(1);//设置其状态为：盘旋
                    }
                    temp.push(p);
                }
                q = temp;
                //更新起飞飞机队列中飞机的状态
                while(!q1.empty()){
                    Plane p=q1.top();
                    q1.pop();
                    l1.push_front(p);
                    p.checkAll();//更新飞机飞行状态
                    if(p.getDistance()>10){
                        p.setStatus(0);//对于已经飞离10km范围内的飞机，更新其状态为：巡航
                    }
                    if(p.getDistance()<1500){//飞离1500km范围内的飞机，不再进行追踪
                        temp1.push(p);
                    }
                }
                q1=temp1;
                //如果跑道空闲，且有飞机处于等待中，允许其降落
                if (!runway.busy && !q.empty()) {
                    Plane p = q.top();//取出队列最前端的飞机
                    if (p.getStatus() == 1) {
                        q.pop();
                        p.setStatus(2);//更新飞机状态为下降
                        runway.landingPlane = p;//降落
                        runway.busy = !runway.busy;//设置跑道状态为繁忙
                    }
                }

                QStandardItemModel* model = new QStandardItemModel(this);
                model->setColumnCount(3);
                //设置表头数据
                model->setHeaderData(0,Qt::Horizontal, "进港航班");
                model->setHeaderData(1,Qt::Horizontal, "距离(km)");
                model->setHeaderData(2,Qt::Horizontal, "状态");
                model->setRowCount(l.size());
                //声明迭代器
                list<Plane>::iterator itor;
                itor = l.begin();
                for(int i=0;itor!=l.end();i++,itor++)
                {
                            //修改model中的数据
                           model->setItem(i, 0,new QStandardItem(QString::fromStdString((*itor).getID())));
                           model->setItem(i, 1,new QStandardItem(QString::number((*itor).getDistance())));
                           switch ((*itor).getStatus()) {
                                case 0:model->setItem(i, 2,new QStandardItem("巡航"));break;
                                case 1:model->setItem(i, 2,new QStandardItem("盘旋"));break;
                                case 2:model->setItem(i, 2,new QStandardItem("下降"));break;
                                case 3:model->setItem(i, 2,new QStandardItem("着陆"));break;
                                case 4:model->setItem(i, 2,new QStandardItem("爬升"));break;
                           }
                 }
                //设置前端tableView组件显示更新后的model数据
                ui->tableView->setModel(model);

                QStandardItemModel* model1 = new QStandardItemModel(this);
                model1->setColumnCount(3);
                model1->setHeaderData(0,Qt::Horizontal, "离港航班");
                model1->setHeaderData(1,Qt::Horizontal, "距离(km)");
                model1->setHeaderData(2,Qt::Horizontal, "状态");


                model1->setRowCount(l1.size());

                list<Plane>::iterator itor1;
                itor1 = l1.begin();

                for(int i=0;itor1!=l1.end();i++,itor1++)
                {
                      // model->setHeaderData(i,Qt::Vertical, i);


                           model1->setItem(i, 0,new QStandardItem(QString::fromStdString((*itor1).getID())));
                           model1->setItem(i, 1,new QStandardItem(QString::number((*itor1).getDistance())));
                           //model1->setItem(i, 2,new QStandardItem(QString::number((*itor1).getStatus())));
                           switch ((*itor1).getStatus()) {
                                case 0:model1->setItem(i, 2,new QStandardItem("巡航"));break;
                                case 1:model1->setItem(i, 2,new QStandardItem("盘旋"));break;
                                case 2:model1->setItem(i, 2,new QStandardItem("下降"));break;
                                case 3:model1->setItem(i, 2,new QStandardItem("着陆"));break;
                                case 4:model1->setItem(i, 2,new QStandardItem("爬升"));break;
                           }

                 }

                ui->tableView->setModel(model);
                ui->tableView->verticalHeader()->hide();
                ui->tableView->verticalHeader()->setDefaultSectionSize(80);
                ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                ui->tableView_2->setModel(model1);
                ui->tableView_2->verticalHeader()->hide();
                ui->tableView_2->verticalHeader()->setDefaultSectionSize(80);
                ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选中模式为选中行
                ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
                ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选中模式为选中行
                ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
                //ui->tableView->setSelectionMode( QAbstractItemView::SingleSelection);//设置选中单个
*/
                QDateTime current_date_time =QDateTime::currentDateTime();
                QString str = current_date_time.toString("yyyy-MM-dd");
                QString week=current_date_time.toString("ddd");
                QString timestr=current_date_time.toString("hh:mm:ss");
                ui->lcdNumber->display(str);
                ui->lcdNumber_4->display(timestr);
                ui->label_7->setText(week);

    //}



}













void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    //获取被双击的行号
    int row = index.row();
    //获取该行中航班的ID，转为Qstring类型用于查询
    QString str=this->ui->tableView->model()->data(this->ui->tableView->model()->index(row,0)).toString();
    //在数据库中查询该航班的详细信息
    QSqlQuery sql_query;
    sql_query.exec(QString("select * from PlaneInfo where ID='%1' ").arg(str));
        if(!sql_query.exec()) {
            qDebug()<<sql_query.lastError();//错误处理
        }
        else{
            while(sql_query.next()){
                //创建对话框，显示查询结果
                Dialog*d=new Dialog(this,sql_query);
                d->show();
            }
        }
}

void MainWindow::on_tableView_2_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    QString str=this->ui->tableView_2->model()->data(this->ui->tableView_2->model()->index(row,0)).toString();
    QSqlQuery sql_query;
    sql_query.exec(QString("select * from PlaneInfo where ID='%1' ").arg(str));
        if(!sql_query.exec())
        {
            qDebug()<<sql_query.lastError();
        }
        else
        {
            while(sql_query.next())
            {
                QString name = sql_query.value(0).toString();
                double speed=sql_query.value(1).toDouble();
                double consumeRate=sql_query.value(2).toDouble();
                double angle=sql_query.value(3).toDouble();
                double fuel=sql_query.value(4).toDouble();
                int in_out=sql_query.value(5).toInt();
                //qDebug()<<name<<speed;
                Dialog*d=new Dialog(this,sql_query);
                d->show();

            }
            }
}

//开始
void MainWindow::on_pushButton_2_clicked()
{
    pTimer->start(1000);
    this->ui->pushButton_2->setText("继续");
}

//暂停
void MainWindow::on_pushButton_5_clicked()
{
    this->pTimer->stop();
}

void MainWindow::on_begin_clicked()
{
    pTimer->start(1000);
}



//保存
void MainWindow::on_pushButton_4_clicked()
{
    list<Record>::iterator itor;
    itor = recordList.begin();
    QSqlDatabase database;
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("D:\planeDB.db");
    if (!database.open()){
        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    else{
        qDebug() << "Succeed to connect database." ;
    }
    //执行查询操作
    QSqlQuery sql_query;
    for(int i=0;itor!=recordList.end();i++,itor++){
        Record r=*itor;
        sql_query.exec(QString("insert into Output values('%1', %2)").arg(r.id).arg(r.time));
    }
    if(!sql_query.exec()){
        qDebug()<<sql_query.lastError();
    }

}
