#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>

# define M_PI           3.14159265358979323846  /* pi */
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

bool Actor::doCommonMovement()
{
	double vert_speed = getVerticalSpeed() - getWorld()->getGhostRacer()->getVerticalSpeed();
	double horiz_speed = getHorizontalSpeed();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;
	moveTo(new_x, new_y);
	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
		setAliveStatus(false);
		return true;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////////////

/*
//Zombie Cab Class
*/

////////////////////////////////////////////////////////////////////////////////////////
void ZombieCab::doSomething() 
{
	if (!getAliveStatus())
	{
		return;
	}
	GhostRacer* racer = getWorld()->getGhostRacer();
	if (doesOverlapWith(racer))
	{
		if (!hasDamagedGhostRacer)
		{
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
			racer->changeHealth(-20);
			if (getX() <= racer->getX())
			{
				setHorizontalSpeed(-5);
				setDirection(120 + randInt(0, 19));
			}
			else {
				setHorizontalSpeed(5);
				setDirection(60 - randInt(0, 19));
			}
			hasDamagedGhostRacer = true;
		}
	}
	//now move
	bool outOfBounds = doCommonMovement();
	if (outOfBounds) {
		return;
	}
	//check for collision avoidance
	double changeSpeed = getWorld()->changeCabSpeed(this);
	if (changeSpeed != -1) {
		changeVerticalSpeed(changeSpeed);
		return;
	}
	setMovementPlanDistance(getMovementPlanDistance() - 1);
	if (getMovementPlanDistance() > 0) { return; }
	else 
	{
		setMovementPlanDistance(randInt(4, 32));
		changeVerticalSpeed(randInt(-2, 2));	//adds rand int from [-2,2] to current vspd
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void ZombieCab::getHitByProjectile() 
{
	changeHealth(-1);
	if (getHealth() <= 0) 
	{
		setAliveStatus(false);
		getWorld()->playSound(SOUND_VEHICLE_DIE);
		getWorld()->tryOilSlickSpawn(getX(), getY());
		getWorld()->increaseScore(200);
		return;
	}
	else
	{
		getWorld()->playSound(SOUND_VEHICLE_HURT);
	}
}
////////////////////////////////////////////////////////////////////////////////////////

/*
//Holy Water Projectile Class
*/

////////////////////////////////////////////////////////////////////////////////////////
void HolyWaterProjectile::doSomething() 
{
	if (!getAliveStatus()) {
		return;
	}
	Actor* hit = getWorld()->tryHitWithProjectile(this);
	if (hit != nullptr)
	{
		hit->getHitByProjectile();
		setAliveStatus(false);
		return;
	}
	//nothing to collide with, so continue moving
	moveForward(SPRITE_HEIGHT);
	distanceTraveled += SPRITE_HEIGHT;
	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
		setAliveStatus(false);
		return;
	}
	if (distanceTraveled >= maxTravelDistance) {
		setAliveStatus(false);
		return;
	}
}
////////////////////////////////////////////////////////////////////////////////////////

/*
//Oil Slick Class
*/

////////////////////////////////////////////////////////////////////////////////////////
void OilSlick::doSomething()
{
	bool outOfBounds = doCommonMovement();
	if (outOfBounds) {
		return;
	}
	if (doesOverlapWith(getWorld()->getGhostRacer()))
	{
		getWorld()->playSound(SOUND_OIL_SLICK);
		getWorld()->getGhostRacer()->skid();
	}
}
////////////////////////////////////////////////////////////////////////////////////////

/*
//Human Ped Class
*/

////////////////////////////////////////////////////////////////////////////////////////
void HumanPed::doSomething()
{
	if (!getAliveStatus())
	{
		return;
	}
	if (doesOverlapWith(getWorld()->getGhostRacer()))
	{
		getWorld()->hitPed();
		return;
	}
	bool outOfBounds = doCommonMovement();
	if (outOfBounds) {
		return;
	}
	setMovementPlanDistance(getMovementPlanDistance()-1);
	if (getMovementPlanDistance() > 0)
	{
		return;
	}
	else
	{
		setHorizontalSpeed(randInt(1, 3) * (randInt(0, 1) == 1 ? 1 : -1));
		setMovementPlanDistance(randInt(4, 32));
		if (getHorizontalSpeed() < 0)
		{
			setDirection(180);
		}
		else
		{
			setDirection(0);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void HumanPed::getHitByProjectile()
{
	setHorizontalSpeed(getHorizontalSpeed() * -1);
	if (getHorizontalSpeed() < 0)
	{
		setDirection(180);
	}
	else
	{
		setDirection(0);
	}
	getWorld()->playSound(SOUND_PED_HURT);
}
////////////////////////////////////////////////////////////////////////////////////////

/*
//Zombie Ped Class
*/

////////////////////////////////////////////////////////////////////////////////////////
void ZombiePed::doSomething() 
{
	if (!getAliveStatus()) {
		return;
	}
	GhostRacer* racer = getWorld()->getGhostRacer();
	if (doesOverlapWith(racer))
	{
		racer->changeHealth(-5);
		changeHealth(-2);
		return;
	}
	if (abs(getX() - racer->getX()) <= 30 && getY() > racer->getY())
	{
		setDirection(270);
		if (getX() > racer->getX()) {
			setHorizontalSpeed(-1);
		}
		else if (getX() < racer->getX()) {
			setHorizontalSpeed(1);
		}
		else {
			setHorizontalSpeed(0);
		}
		ticksUntilGrunt--;
		if (ticksUntilGrunt <= 0) {
			getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			ticksUntilGrunt = 20;
		}
	}
	//now, move
	bool outOfBounds = doCommonMovement();
	if (outOfBounds) {
		return;
	}
	if (getMovementPlanDistance() > 0) {
		setMovementPlanDistance(getMovementPlanDistance() - 1);
		return;
	}
	//set new movement plan
	int newSpd = randInt(1, 3) * (randInt(0, 1) == 1 ? 1 : -1);		//always evaluates to [-3, 0)U(0, 3]
	setHorizontalSpeed(newSpd);
	setMovementPlanDistance(randInt(4, 32));
	if (getHorizontalSpeed() < 0) {
		setDirection(180);
	}
	else {
		setDirection(0);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void ZombiePed::changeHealth(int delta) {
	setHealth(getHealth() + delta);
	if (getHealth() <= 0) {
		setAliveStatus(false);
		getWorld()->playSound(SOUND_PED_DIE);
		if (!doesOverlapWith(getWorld()->getGhostRacer())) {
			getWorld()->tryHealingGoodieSpawn(getX(), getY());
			getWorld()->increaseScore(150);
		}
	}
	else {
		getWorld()->playSound(SOUND_PED_HURT);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void ZombiePed::getHitByProjectile() {
	changeHealth(-1);
}
////////////////////////////////////////////////////////////////////////////////////////

/*
//Healing Goodie Class
*/

////////////////////////////////////////////////////////////////////////////////////////
void HealingGoodie::doSomething() {

	//location checking
	bool outOfBounds = doCommonMovement();
	if (outOfBounds) {
		return;
	}

	if (doesOverlapWith(getWorld()->getGhostRacer())) {
		getWorld()->getGhostRacer()->changeHealth(10);
		setAliveStatus(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(250);
	}

}
////////////////////////////////////////////////////////////////////////////////////////

/*
//Holy Water Goodie Class
*/

////////////////////////////////////////////////////////////////////////////////////////
void HolyWaterGoodie::doSomething() {

	//location checking
	bool outOfBounds = doCommonMovement();
	if (outOfBounds) {
		return;
	}

	if (doesOverlapWith(getWorld()->getGhostRacer())) {
		getWorld()->getGhostRacer()->changeHolyWaterSprays(10);
		setAliveStatus(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(50);
	}

}
////////////////////////////////////////////////////////////////////////////////////////

/*
//Soul Goodie Class
*/

////////////////////////////////////////////////////////////////////////////////////////
void SoulGoodie::doSomething() {
	//movement
	bool outOfBounds = doCommonMovement();
	if (outOfBounds) {
		return;
	}

	//racer overlap checking
	if (doesOverlapWith(getWorld()->getGhostRacer())) {
		getWorld()->soulSaved();
		setAliveStatus(false);
	}

	setDirection(getDirection() + 10);
}
////////////////////////////////////////////////////////////////////////////////////////

/*
//BorderLine Class
*/

////////////////////////////////////////////////////////////////////////////////////////
void BorderLine::doSomething() {
	bool outOfBounds = doCommonMovement();
	if (outOfBounds) {
		return;
	}
}
////////////////////////////////////////////////////////////////////////////////////////

/*
// GhostRacer class
*/

////////////////////////////////////////////////////////////////////////////////////////
void GhostRacer::doSomething() {
	//1
	if (getAliveStatus() == false) { return; }
	//2
	if (getX() <= (ROAD_CENTER - (ROAD_WIDTH / 2))) {
		if (getDirection() > 90) {
			changeHealth(-10);
		}
		setDirection(82);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		moveRacer();
	}
	//3
	if (getX() >= (ROAD_CENTER + (ROAD_WIDTH / 2))) {
		if (getDirection() < 90) {
			changeHealth(-10);
		}
		setDirection(98);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		moveRacer();
	}
	//4
	int key;
	if (getWorld()->getKey(key)) {
		switch (key) {
		case KEY_PRESS_SPACE:
			if (getHolyWaterSprays() >= 1) {
				//spawn holy water
				StudentWorld* world = getWorld();
				getWorld()->addActor(new HolyWaterProjectile(world, getX(), getY(), getDirection()));
				getWorld()->playSound(SOUND_PLAYER_SPRAY);
				changeHolyWaterSprays(-1);
			}
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() < 114) {
				setDirection(getDirection() + 8);
			}
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() > 66) {
				setDirection(getDirection() - 8);
			}
			break;
		case KEY_PRESS_UP:
			if (getVerticalSpeed() < 5) {
				changeVerticalSpeed(1);
			}
			break;
		case KEY_PRESS_DOWN:
			if (getVerticalSpeed() > -1) {
				changeVerticalSpeed(-1);
			}
			break;
		}
		
	}
	moveRacer();
}
////////////////////////////////////////////////////////////////////////////////////////
void GhostRacer::moveRacer() {
	double max_shift_per_tick = 4.0;
	double direction = getDirection();
	double delta_x = cos(direction * M_PI / 180) * max_shift_per_tick;
	double cur_x = getX();
	double cur_y = getY();
	moveTo(cur_x + delta_x, cur_y);
}
////////////////////////////////////////////////////////////////////////////////////////
void GhostRacer::skid()
{
	setDirection(getDirection() + (randInt(5, 20) * (randInt(0, 1) == 1 ? 1 : -1)));	//always changes direction by [-20,-5]U[5,20]
}
