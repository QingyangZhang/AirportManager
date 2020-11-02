#include "airport.h"
#include "QDebug"

string Plane::getID()
{
    return this->ID;
}

int Plane::getEmergency()
{
    return this->emergency;
}

int Plane::getStatus()
{
    return this->status;
}

double Plane::getFuel()
{
    return this->fuel;
}

double Plane::getDistance()
{
    return this->distance;
}

int Plane::getInOut()
{
    return this->in_out;
}

double Plane::getAngle()
{
    return angle;
}

double Plane::getDirection()
{
    return direction;
}

void Plane::setStatus(int s)
{
    this->status = s;
}

void Plane::setEmergency(int e)
{
    this->emergency = e;
}

void Plane::checkFuel()
{
    this->fuel -= consumeRate;
}

void Plane::checkEmergency()
{

}

void Plane::checkDistance()
{
    if (this->getStatus() == 0)//flying
    {
        if(this->getInOut()){
        distance -= speed;
        }else{
        distance+=speed;

        }
        //qDebug()<<"! "<<in_out<<"! ";
    }
   else if (this->getStatus() == 1){//cylcing
        this->angle += speed/(400*3.14159*2)*360;
        this->direction=angle+90;
    }
    else if (this->getStatus() == 2){//landing
        this->direction=angle+180;
        this->distance -= speed;
    }
    else if(this->getStatus()==4){//taking off
        this->distance+=speed;
    }
}

void Plane::checkAll()
{
    checkFuel();
    checkDistance();
}
