#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

using namespace std;

class StudentWorld;

class Actor : public GraphObject
{
public:
	/*
	Actor(StudentWorld* w, int imageid, double x, double y, int dir, double size, int depth)
		: GraphObject(imageid, x, y, dir, size, depth) {
		world = w;
	};
	*/
	///
	Actor(StudentWorld* w, int imageid, double x, double y, int dir, double size, int depth,
		double verticalSpd, double horizontalSpd, int hp)
		: GraphObject(imageid, x, y, dir, size, depth) {
		world = w;
		verticalSpeed = verticalSpd;
		horizontalSpeed = horizontalSpd;
		maxHP = hp;
		hitPoints = maxHP;
	}
	virtual void doSomething() = 0;

	double getVerticalSpeed() { return verticalSpeed; }
	void changeVerticalSpeed(double delta) { verticalSpeed += delta; }

	bool getAliveStatus() { return isAlive; }

	int getHealth() { return hitPoints; }

	virtual bool getCollisionWorthiness() {
		return false;
	}

	virtual bool canBeHitByProjectile() { return false; }
	virtual void getHitByProjectile() {};

	bool doCommonMovement(); 

	bool doesOverlapWith(Actor* b) {
		Actor* a = this;
		double delta_x = abs(b->getX() - a->getX());
		double delta_y = abs(b->getY() - a->getY());

		double radius_sum = a->getRadius() + b->getRadius();	//both sprites are square, radius is SPRITE_WIDTH/2

		if (delta_x < radius_sum * 0.25 && delta_y < radius_sum * 0.6) {
			return true;
		}
		return false;
	}

	virtual void changeHealth(int delta) {
		hitPoints += delta;
		if (hitPoints <= 0) {
			isAlive = false;
		}
	}
private:
	double verticalSpeed, horizontalSpeed;
	int hitPoints, maxHP;
	bool isAlive = true;
	StudentWorld* world;
protected:
	StudentWorld* getWorld() { return world; }
	void setHealth(int h) { hitPoints = h; }
	void setAliveStatus(bool life) { isAlive = life; }
	void setHorizontalSpeed(double spd) { horizontalSpeed = spd; }
	double getHorizontalSpeed() { return horizontalSpeed; }
};

class SmartActor : public Actor
{
public:
	SmartActor(StudentWorld* w, int imageid, double x, double y, int dir, double size, int depth,
		double verticalSpd, double horizontalSpd, int hp)
		: Actor(w, imageid, x, y, dir, size, depth, verticalSpd, horizontalSpd, hp) {}
private:
	int movementPlanDistance = 0;
protected:
	virtual bool canBeHitByProjectile() { return true; }
	int getMovementPlanDistance() { return movementPlanDistance; }
	void setMovementPlanDistance(int n) { movementPlanDistance = n; }
	bool getCollisionWorthiness() { return true; }
};

class ZombiePed : public SmartActor
{
public:
	ZombiePed(StudentWorld* w, double x, double y)
		: SmartActor(w, IID_ZOMBIE_PED, x, y, 0, 3, 0, -4, 0, 2) {
	};
	virtual void doSomething();
private:
	int ticksUntilGrunt = 20;
	virtual void changeHealth(int delta);
	virtual void getHitByProjectile();
};
////////////////////////////////////////////////////////////////////////////////////////
class HumanPed : public SmartActor
{
public:
	HumanPed(StudentWorld* w, double x, double y)
		: SmartActor(w, IID_HUMAN_PED, x, y, 0, 2, 0, -4, 0, 2) {
	};
	virtual void doSomething();
private:
	virtual void changeHealth(int delta) {};
	virtual void getHitByProjectile();
};
////////////////////////////////////////////////////////////////////////////////////////
class ZombieCab : public SmartActor
{
public:
	ZombieCab(StudentWorld* w, double x, double y, double vspd)
		: SmartActor(w, IID_ZOMBIE_CAB, x, y, 90, 4, 0, vspd, 0, 3) {}
	virtual void doSomething();
private:
	virtual void getHitByProjectile();
	bool hasDamagedGhostRacer = false;
};
////////////////////////////////////////////////////////////////////////////////////////
class BorderLine : public Actor
{
public:
	BorderLine(StudentWorld* w, int imageid, double x, double y)
		: Actor(w, imageid, x, y, 0, 2.0, 2, -4, 0, 0) {};
	virtual void doSomething();
	virtual bool canBeHitByProjectile() { return false; }
};
////////////////////////////////////////////////////////////////////////////////////////
class GhostRacer : public Actor
{
public:
	GhostRacer(StudentWorld* w)
		: Actor(w, IID_GHOST_RACER, 128, 32, 90, 4.0, 0, 0, 0, 100) {};
	virtual void doSomething();
	void skid();
	int getHolyWaterSprays() { return holyWaterSprays; }
	void changeHolyWaterSprays(int delta) { holyWaterSprays += delta; }
private:
	void moveRacer();
	int holyWaterSprays = 10;
};
////////////////////////////////////////////////////////////////////////////////////////
class SoulGoodie : public Actor 
{
public:
	SoulGoodie(StudentWorld* w, double x, double y)
		: Actor(w, IID_SOUL_GOODIE, x, y, 0, 4.0, 2, -4, 0, 0) {};
	virtual void doSomething();
};
////////////////////////////////////////////////////////////////////////////////////////
class HolyWaterGoodie : public Actor
{
public:
	HolyWaterGoodie(StudentWorld* w, double x, double y)
		: Actor(w, IID_HOLY_WATER_GOODIE, x, y, 90, 2.0, 2, -4, 0, 0) {};
	virtual void doSomething();
};
////////////////////////////////////////////////////////////////////////////////////////
class HealingGoodie : public Actor
{
public:
	HealingGoodie(StudentWorld* w, double x, double y)
		: Actor(w, IID_HEAL_GOODIE, x, y, 0, 1.0, 2, -4, 0, 0) {};
	virtual void doSomething();
};
////////////////////////////////////////////////////////////////////////////////////////
class HolyWaterProjectile : public Actor
{
public:
	HolyWaterProjectile(StudentWorld* w, double x, double y, int dir)
		: Actor(w, IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1, 0, 0, 0) {};
	virtual void doSomething();
private:
	int maxTravelDistance = 160;
	int distanceTraveled = 0;
};
////////////////////////////////////////////////////////////////////////////////////////
class OilSlick : public Actor
{
public:
	OilSlick(StudentWorld* w, double x, double y)
		:Actor(w, IID_OIL_SLICK, x, y, 0, randInt(2, 5), 2, -4, 0, 0) {};
	virtual void doSomething();
};
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

#endif // ACTOR_H_
