#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init()
{
    racer = new GhostRacer(this);
    int n = (VIEW_HEIGHT / SPRITE_HEIGHT);
    double LEFT_EDGE = ROAD_CENTER - (ROAD_WIDTH / 2);
    double RIGHT_EDGE = ROAD_CENTER + (ROAD_WIDTH / 2);

    //create edge lines
    BorderLine* line;
    for (int j = 0; j < n; j++) {

        //left edge
        line = new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, j * SPRITE_HEIGHT);
        tempActors.push_back(line);

        //right edge
        line = new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, j * SPRITE_HEIGHT);
        tempActors.push_back(line);
    }

    //create middle lines
    int m = (VIEW_HEIGHT / (4 * SPRITE_HEIGHT));
    for (int j = 0; j < m; j++) {

        //left mid line
        line = new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + (ROAD_WIDTH / 3), j * (4*SPRITE_HEIGHT));
        tempActors.push_back(line);

        //right mid line
        line = new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - (ROAD_WIDTH / 3), j * (4*SPRITE_HEIGHT));
        tempActors.push_back(line);
    }
    lastBorderY = VIEW_HEIGHT - SPRITE_HEIGHT;

    bonusRemaining = 5000;
    soulsUntilNextLevel = 2 * getLevel() + 5;

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (!racer->getAliveStatus() || hitHumanPed) {
        if(!racer->getAliveStatus()) { playSound(SOUND_PLAYER_DIE); }
        //racer is dead, decLives and return correct status
        decLives();
        hitHumanPed = false;
        return GWSTATUS_PLAYER_DIED;
    }

    //try creating new actors
    addNewActors();

    racer->doSomething();

    list<Actor*>::iterator itr = tempActors.begin();

    while (itr != tempActors.end()) {
        Actor* actor = *itr;

        if (actor != nullptr) {
            if (actor->getAliveStatus() == false) {
                tempActors.erase(itr++);
                delete actor;
            }
            else {
                actor->doSomething();
                ++itr;
            }
        }
    }

    double speed = getGhostRacer()->getVerticalSpeed();
    lastBorderY += (-4 - speed); //change lastBorderY by the speed of the borders

    double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    double delta_y = new_border_y - lastBorderY;

    if (delta_y >= SPRITE_HEIGHT) {
        //add yellow lines
        double LEFT_EDGE = ROAD_CENTER - (ROAD_WIDTH / 2);
        double RIGHT_EDGE = ROAD_CENTER + (ROAD_WIDTH / 2);
        BorderLine* line = new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y);
        tempActors.push_back(line);
        line = new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y);
        tempActors.push_back(line);
    }
    if (delta_y >= 4*SPRITE_HEIGHT) {
        //add white lines
        double LEFT_EDGE = ROAD_CENTER - (ROAD_WIDTH / 2);
        double RIGHT_EDGE = ROAD_CENTER + (ROAD_WIDTH / 2);
        BorderLine* line;
        line = new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + (ROAD_WIDTH / 3), new_border_y);
        tempActors.push_back(line);
        line = new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - (ROAD_WIDTH / 3), new_border_y);
        tempActors.push_back(line);
        lastBorderY = new_border_y;
    }
    ticks++;

    ostringstream oss;
    oss << "Score: " << getScore() << "  ";
    oss << "Lvl: " << getLevel() << "  ";
    oss << "Souls2Save: " << soulsUntilNextLevel << "  ";
    oss << "Lives: " << getLives() << "  ";
    oss << "Health: " << getGhostRacer()->getHealth() << "  ";
    oss << "Sprays: " << getGhostRacer()->getHolyWaterSprays() << "  ";
    oss << "Bonus: " << bonusRemaining;

    setGameStatText(oss.str());

    if (soulsUntilNextLevel == 0) {
        //move on to next level
        increaseScore(bonusRemaining);
        return GWSTATUS_FINISHED_LEVEL;
    }

    bonusRemaining--;

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete racer;
    list<Actor*>::iterator itr = tempActors.begin();
    while (itr != tempActors.end()) {
        Actor* actor = *itr;
        tempActors.erase(itr++);
        delete actor;
    }
}

void StudentWorld::addNewActors() {
    int chanceOfLostSoul = 100;
    if (randInt(0, chanceOfLostSoul-1) == 0) {
        SoulGoodie* soul = new SoulGoodie(this, randomXPosition(), VIEW_HEIGHT);
        tempActors.push_back(soul);
    }

    int chanceOfHolyWater = 100 + 10*getLevel();
    if (randInt(0, chanceOfHolyWater-1) == 0) {
        HolyWaterGoodie* goodie = new HolyWaterGoodie(this, randomXPosition(), VIEW_HEIGHT);
        tempActors.push_back(goodie);
    }
    
    int chanceOfZombiePed = max(100 - getLevel()*10, 20);
    if (randInt(0, chanceOfZombiePed-1) == 0) {
        ZombiePed* zomb = new ZombiePed(this, randInt(1, VIEW_WIDTH-1), VIEW_HEIGHT);
        tempActors.push_back(zomb);
    }

    int chanceOfOilSlick = max(150 - getLevel() * 10, 40);
    if (randInt(0, chanceOfOilSlick - 1) == 0) {
        OilSlick* oil = new OilSlick(this, randomXPosition(), VIEW_HEIGHT);
        tempActors.push_back(oil);
    }

    int chanceOfHumanPed = max(200 - getLevel() * 10, 30);
    if (randInt(0, chanceOfHumanPed - 1) == 0) {
        HumanPed* human = new HumanPed(this, randInt(1, VIEW_WIDTH-1), VIEW_HEIGHT);
        tempActors.push_back(human);
    }
    
    int chanceOfVehicle = max(100 - getLevel() * 10, 20);
    if (randInt(0, chanceOfVehicle - 1) == 0)
    {
        int lane = randInt(0, 2);
        double closestCollisionActorY = VIEW_HEIGHT + 1;
        int destLane = -1;
        double destY, destVspd;
        for (int i = 0; i < 3; i++) {
            lane = (lane + i) % 3;
            list<Actor*>::iterator itr = tempActors.begin();
            while (itr != tempActors.end()) {
                Actor* actor = *itr;
                if (actor->getCollisionWorthiness() && laneActorIsIn(actor) == lane && actor->getY() < closestCollisionActorY)
                {
                    closestCollisionActorY = actor->getY();
                }
                itr++;
            }
            if (laneActorIsIn(getGhostRacer()) == lane && getGhostRacer()->getY() < closestCollisionActorY)
            {
                closestCollisionActorY = getGhostRacer()->getY();
            }
            if (closestCollisionActorY > (VIEW_HEIGHT / 3))
            {
                destLane = lane;
                destY = SPRITE_HEIGHT / 2;
                destVspd = getGhostRacer()->getVerticalSpeed() + randInt(2, 4);
                i = 4;  //break
            }
            else 
            {
                closestCollisionActorY = -1;
                list<Actor*>::iterator itr = tempActors.begin();
                while (itr != tempActors.end()) {
                    Actor* actor = *itr;
                    if (actor->getCollisionWorthiness() && laneActorIsIn(actor) == lane && actor->getY() > closestCollisionActorY)
                    {
                        closestCollisionActorY = actor->getY();
                    }
                    itr++;
                }
                if (laneActorIsIn(getGhostRacer()) == lane && getGhostRacer()->getY() > closestCollisionActorY)
                {
                    closestCollisionActorY = getGhostRacer()->getY();
                }
                if (closestCollisionActorY < ((VIEW_HEIGHT*2) / 3))
                {
                    destLane = lane;
                    destY = VIEW_HEIGHT - (SPRITE_HEIGHT / 2);
                    destVspd = getGhostRacer()->getVerticalSpeed() - randInt(2, 4);
                    i = 4;  //break
                }
            }
        }
        //step 3
        if (destLane == -1)
        {
            //no good spot found, return
            return;
        }
        else
        {
            int destX = (ROAD_CENTER - (ROAD_WIDTH / 3)) + (destLane * (ROAD_WIDTH / 3));
            tempActors.push_back(new ZombieCab(this, destX, destY, destVspd));
        }
        //step 4
    }
}

double StudentWorld::changeCabSpeed(ZombieCab* c)
{
    if (c->getVerticalSpeed() > racer->getVerticalSpeed())
    {
        list<Actor*>::iterator itr = tempActors.begin();
        while (itr != tempActors.end()) {
            Actor* actor = *itr;
            if (actor->getX() == c->getX() && actor->getCollisionWorthiness() && actor->getY() < (c->getY() + 96) && actor->getY() > c->getY())
            {
                return -0.5;
            }
            itr++;
        }
    }
    if (c->getVerticalSpeed() <= racer->getVerticalSpeed())
    {
        list<Actor*>::iterator itr = tempActors.begin();
        while (itr != tempActors.end()) {
            Actor* actor = *itr;
            if (actor->getX() == c->getX() && actor->getCollisionWorthiness() && actor->getY() > (c->getY() - 96) && actor->getY() < c->getY())
            {
                c->changeVerticalSpeed(0.5);
                return 0.5;
            }
            itr++;
        }
    }
    return -1;
}

Actor* StudentWorld::tryHitWithProjectile(HolyWaterProjectile* p)
{
    list<Actor*>::iterator itr = tempActors.begin();

    while (itr != tempActors.end()) {
        Actor* actor = *itr;
        if (p->doesOverlapWith(actor) && actor->canBeHitByProjectile())
        {
            return actor;
        }
        itr++;
    }
    return nullptr;
}

void StudentWorld::addActor(Actor* a)
{
    tempActors.push_back(a);
}

int StudentWorld::randomXPosition() 
{
    int maxX = ROAD_CENTER + (ROAD_WIDTH / 2);
    int minX = ROAD_CENTER - (ROAD_WIDTH / 2);
    return randInt(minX, maxX);
}

void StudentWorld::soulSaved() 
{
    soulsUntilNextLevel--;
    increaseScore(100);
    playSound(SOUND_GOT_SOUL);
}

void StudentWorld::tryHealingGoodieSpawn(double x, double y) 
{
    int r = randInt(0, 4);
    if (r == 0) {
        HealingGoodie* goodie = new HealingGoodie(this, x, y);
        tempActors.push_back(goodie);
    }
}

void StudentWorld::tryOilSlickSpawn(double x, double y)
{
}

string StudentWorld::formattedStatus() {
    string status = "";
    
    status += "Score: " + getScore();
    status += "  ";
    status += "Lvl: " + getLevel();
    status += "  ";
    status += "Souls2Save: " + soulsUntilNextLevel;
    status += "  ";
    status += "Lives: " + getLives();
    status += "  ";
    status += "Health: " + getGhostRacer()->getHealth();
    status += "  ";
    status += "Sprays: " + getGhostRacer()->getHolyWaterSprays();
    status += "  ";
    status += "Bonus: " + bonusRemaining;

    return status;
}

//returns int corresponding to the lane the actor is currently in [0, 2]
int StudentWorld::laneActorIsIn(Actor* actor)
{
    for (int i = 0; i < 3; i++)
    {
        int minLaneX = (ROAD_CENTER - ROAD_WIDTH / 2) + (ROAD_WIDTH / 3) * i;
        int maxLaneX = (ROAD_CENTER - ROAD_WIDTH / 2) + (ROAD_WIDTH / 3) * (i + 1);
        if (actor->getX() >= minLaneX && actor->getX() < maxLaneX)
        {
            return i;
        }
    }
    return -1;
}
