#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <iterator>
#include <list>

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void addActor(Actor* a);
    double changeCabSpeed(ZombieCab* c);
    Actor* tryHitWithProjectile(HolyWaterProjectile* p);
    void soulSaved();
    void hitPed() { hitHumanPed = true; }
    void tryHealingGoodieSpawn(double x, double y);
    void tryOilSlickSpawn(double x, double y);
    GhostRacer* getGhostRacer() { return racer; }

private:
    string formattedStatus();
    int laneActorIsIn(Actor* actor);
    void addNewActors();
    int randomXPosition();
    list<Actor*> tempActors;
    GhostRacer* racer;
    int soulsUntilNextLevel;
    int bonusRemaining;
    int ticks = 0;
    int lastBorderY;
    bool hitHumanPed = false;
};

#endif // STUDENTWORLD_H_