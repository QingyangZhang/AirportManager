#ifndef RADIOLABEL_H
#define RADIOLABEL_H

#endif // RADIOLABEL_H
#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QString>
#include <QDebug>
#include <QObject>
#include <QPaintEvent>

class RadioLabel:public QLabel
{
    Q_OBJECT
public:
    explicit RadioLabel(QString path,QWidget *parent = 0);
    ~RadioLabel();
    void paintEvent(QPaintEvent* event);

private:
    QImage* image;

};


