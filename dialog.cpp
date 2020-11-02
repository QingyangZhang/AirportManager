#include "dialog.h"
#include "ui_dialog.h"
#include <QStandardItemModel>
#include "QDebug"

Dialog::Dialog(QWidget *parent,QSqlQuery sql_query) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->label->setText("详细信息");
    QStandardItemModel *ItemModel = new QStandardItemModel(this);
    QStringList strList;
    //设置表头信息
    QStringList headerList;
    headerList<<" 航班号："<<" 机型："<<" 始发地："<<" 目的地："<<" 计划起飞时间："<<" 预计到达时间：";
    strList.append(sql_query.value(0).toString());
    //将查询结果分割并与对应的表头拼接
    for(int i=0;i<5;i++){
        strList.append(sql_query.value(7+i).toString());
    }
    //将查询结果加入model用于显示
    for(int i = 0; i < strList.size(); i++){
        QString string = static_cast<QString>(strList.at(i));
        QStandardItem *item = new QStandardItem(headerList.at(i)+string);
        ItemModel->appendRow(item);

     }
    this->ui->tableView->setModel(ItemModel);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->verticalHeader()->setDefaultSectionSize(80);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setStyleSheet("#Dialog{border-image:url(:/timg.jpg);}");


}

Dialog::~Dialog()
{
    delete ui;
}


