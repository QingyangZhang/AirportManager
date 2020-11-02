#include "radiolabel.h"

RadioLabel::RadioLabel(QString path,QWidget *parent):QLabel(parent)
{
    image= new QImage(path);
}

RadioLabel::~RadioLabel(){
    delete image;
}

void RadioLabel::paintEvent(QPaintEvent* event){

    QPainter painter(this);
    painter.drawImage(rect(),*image);
    painter.setPen(Qt::green);
    double x=250;double y=250;double r=200;int n=5;
    painter.drawLine(x-r, y, x+r, y);
    painter.drawLine(x, y-r, x, y+r);

    //painter.drawRect(10, 10, 100, 400);
    //painter.setPen(QPen(Qt::green, 5));
    //painter.setBrush(Qt::blue);
    //painter.drawEllipse(x-r, y-r, 2*r, 2*r);
    for(int i=1;i<=n;i++){
     painter.drawEllipse(x-r*i/n, y-r*i/n, 2*r*i/n, 2*r*i/n);
    }

}
