#ifndef AIRPORT_H
#define AIRPORT_H

#include <vector>
#include <queue>
#include<list>
#include <string>
using namespace std;


//enum Status { FINE, , WHITE, BLACK };

class Plane {
public:

    string getID();
    int getEmergency();
    int getStatus();
    double getFuel();
    double getDistance();
    double getAngle();
    double getDirection();
    int getInOut();

    void setStatus(int s);
    void setEmergency(int e);

    void checkFuel();
    void checkEmergency();
    void checkDistance();
    void checkAll();
    Plane() {}
    Plane(string id,double initSpeed,double initConsumeRate,double initDistance,double initAngle,double initFuel,int in)//构造函数
    {
        ID = id;
        speed = initSpeed;
        in_out=in;
        if(in_out){
            direction=initAngle+180;
            status=0;
        }
        else {
            direction=initAngle;
            status=4;
        }
        consumeRate = initConsumeRate;
        fuel = initFuel;
        distance = initDistance;
        angle = initAngle;

        emergency = 0;
    }

private:
    string ID;
    //Type type;
    //Company company;
    //Size size;
    double speed;
    double direction;
    double consumeRate;
    double fuel;
    //Location location;
    double distance;
    double angle;
    int in_out;
    int status;//0:normal,1:circling,2:landing,3:shutdown
    int emergency;//0:normal,1:lock fuel,2:danger
};

class Runway {
public:
    double length;
    Plane landingPlane;
    bool busy;
    Runway(){}
    Runway(double l) {
        length = l;
        //landingPlane = p;
        busy = false;
    }

};


/*bool operator<(Plane a, Plane b) {
    return a.getEmergency() < b.getEmergency();
}*/    //自定义排序规则

struct cmp //重写仿函数
{
    bool operator() (Plane a, Plane b)
    {
        if (!(a.getEmergency() == b.getEmergency()))
            return a.getEmergency() < b.getEmergency(); //大顶堆
        else return a.getFuel() > b.getFuel();
    }
};

typedef priority_queue<Plane, vector<Plane>, cmp> PlaneQueue;
typedef list<Plane> PlaneList;

class Airport{
public:
    PlaneQueue planeQueue;
    Runway runway;
};

#endif // AIRPORT_H
