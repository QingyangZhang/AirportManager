#include "radar.h"
#include <QPainter>
#include <QTimerEvent>
#include <QConicalGradient>
#include <qDebug>
#include <complex>
#include <mainwindow.h>



Radar::Radar(QWidget *parent) : QWidget(parent)
{
    //初始化
    m_pieRotate = 0;
    m_timerId = -1;
    m_nSpeed = 1000;
    m_points<<QPoint()<<QPoint()<<QPoint()<<QPoint()<<QPoint();
    m_pointsAlapha<<100<<100<<100<<100<<100;

    //启动定时器
    m_timerId = startTimer(m_nSpeed);
    m_pointTimerId = startTimer(1000);




}

void Radar::paintEvent(QPaintEvent *event)
{



    QPainter painter(this);
    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing);
    //背景
    //painter.fillRect(rect(),QColor(5,69,112,255));
    QPixmap bg;
    bg.load("C:\\Users\\ZQY\\Desktop\\google.png");
    QPixmap planePic0;
    planePic0.load("C:\\Users\\ZQY\\Desktop\\planePic0.png");
    QPixmap planePic1;
    planePic1.load("C:\\Users\\ZQY\\Desktop\\planePic1.png");
    QPixmap planePic2;
    planePic2.load("C:\\Users\\ZQY\\Desktop\\planePic2.png");
    QPixmap planePic3;
    planePic3.load("C:\\Users\\ZQY\\Desktop\\planePic3.png");
    QPixmap planePic4;
    planePic4.load("C:\\Users\\ZQY\\Desktop\\planePic4.png");
    QPixmap planePicd;
    planePicd.load("C:\\Users\\ZQY\\Desktop\\planePicd.png");

    painter.drawPixmap(rect(),bg);
    //边长
    int len = m_drawArea.width();
    //底盘(x轴、y轴和3个圆)
    painter.setPen(QPen(Qt::white,1));
    painter.drawLine(m_drawArea.topLeft() + QPoint(0,len/2),m_drawArea.topRight() + QPoint(0,len/2));
    painter.drawLine(m_drawArea.topLeft() + QPoint(len/2,0),m_drawArea.bottomLeft() + QPoint(len/2,0));
    int r=len/2;
    for(int i=1;i<=3;i++){
        painter.drawEllipse(m_drawArea.center(),r*i/3,r*i/3);
    }
    //转动部分
    qreal x = m_drawArea.center().x() + (qreal)len/2 * cos(-m_pieRotate*3.14159/180);
    qreal y = m_drawArea.center().y() + (qreal)len/2 * sin(-m_pieRotate*3.14159/180);
    painter.drawLine(m_drawArea.center(),QPointF(x,y));
    QConicalGradient gradient;
    gradient.setCenter(m_drawArea.center());
    gradient.setAngle(m_pieRotate + 180); //渐变与旋转方向恰好相反，以扇形相反的边作为渐变角度。
    gradient.setColorAt(0.4,QColor(2,255,2,100)); //从渐变角度开始0.5 - 0.75为扇形区域，由于Int类型计算不精确，将范围扩大到0.4-0.8
    gradient.setColorAt(0.8,QColor(2,255,2,0));
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawPie(m_drawArea,m_pieRotate*16,90*16);
    //更换画笔
    QPen mypen;
    mypen.setColor(Qt::yellow);
    painter.setPen(mypen);
    QFont f("微软雅黑",15);
    painter.setFont(f);



    //while(!l.empty()){
        list<Plane>::iterator itor;
        itor = l.begin();
        for(int i=0;itor!=l.end();i++,itor++){
            Plane p=*itor;
       // Plane p=l.front();//从列表中取出一架飞机
       // l.pop_front();
        //计算其显示在雷达上的坐标
        double x=m_drawArea.center().x()+m_drawArea.width()/2*p.getDistance()/1500*cos(p.getAngle()*3.14159/180);
        double y=m_drawArea.center().y()+m_drawArea.height()/2*p.getDistance()/1500*sin(p.getAngle()*3.14159/180);
        //根据飞机的航向对飞机素材图片进行旋转
        QMatrix matrix;
        matrix.rotate(p.getDirection());
        if(p.getEmergency()==1){
            //出现紧急情况的飞机使用红色绘制
            painter.drawPixmap(x,y,30,30,planePicd.transformed(matrix));
            painter.drawText(x,y,QString::fromStdString(p.getID()));
            continue;
        }
        switch(p.getStatus()){//根据飞行状态不同选择不同的素材绘制
        case 0:painter.drawPixmap(x,y,40,40,planePic0.transformed(matrix));break;
        case 1:painter.drawPixmap(x,y,30,30,planePic1.transformed(matrix));break;
        case 2:painter.drawPixmap(x,y,30,30,planePic2.transformed(matrix));break;
        case 3:painter.drawPixmap(x,y,30,30,planePic0.transformed(matrix));break;
        }
        //标注上飞机的ID
        painter.drawText(x,y,QString::fromStdString(p.getID()));
        }

//}


    //while(!l1.empty()){
        //Plane p=l1.front();
        //l1.pop_front();
        list<Plane>::iterator itor1;
        itor1 = l1.begin();
        for(int i=0;itor1!=l1.end();i++,itor1++){
        Plane p=*itor1;
        double x=m_drawArea.center().x()+m_drawArea.width()/2*p.getDistance()/1500*cos(p.getAngle()*3.14159/180);
        double y=m_drawArea.center().y()+m_drawArea.height()/2*p.getDistance()/1500*sin(p.getAngle()*3.14159/180);
        QMatrix matrix1;
        matrix1.rotate(p.getDirection());
        if(p.getEmergency()==1){
            painter.drawPixmap(x,y,30,30,planePicd.transformed(matrix1));
            continue;
        }
        switch(p.getStatus()){
        case 0:
            painter.drawPixmap(x,y,30,30,planePic0.transformed(matrix1));
            break;
        case 1:
            painter.drawPixmap(x,y,30,30,planePic1.transformed(matrix1));
            break;
         case 2:
            painter.drawPixmap(x,y,30,30,planePic2.transformed(matrix1));
            break;
        case 4:
             painter.drawPixmap(x,y,30,30,planePic4.transformed(matrix1));
            break;
        }

        painter.drawText(x,y,QString::fromStdString(p.getID()));
        }
    //更详细解释参考：http://fearlazy.com/index.php/post/31.html
//}


}

void Radar::resizeEvent(QResizeEvent *event)
{
    //以较短的边长作为绘制区域边长
    if(width() > height())
    {
        m_drawArea = QRect((width() - height())/2,0,height(),height());
    }
    else
    {
        m_drawArea = QRect(0,(height() - width())/2,width(),width());
    }

    m_drawArea.adjust(10,10,-10,-10);
}

void Radar::timerEvent(QTimerEvent *event)
{
    if(m_timerId == event->timerId())
    {
        m_pieRotate -= 10;
        update();
    }

}
