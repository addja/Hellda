#include "include/cZone.h"

cZone::cZone(void) 
{
	current = 0;
}

cZone::~cZone(void) {}

void cZone::Reset() 
{
	for (int i = 0; i < current; ++i) {
		enemies[i].Reset();
	}
}

void cZone::Draw(float playerx, float playery) 
{
	for (int i = 0; i < current; ++i) {
		enemies[i].Draw(playerx, playery);
	}
}

void cZone::addEnemy(float x, float y, int type, bool thrower)
{
	enemies[current] = cEnemy(x,y,type,thrower);
	enemies[current].SetWidthHeight(TILE_SIZE, TILE_SIZE);
	++current;
}

//void addObject();

cEnemy* cZone::GetEnemies() {
	return enemies;
}

void cZone::Logic(int *map) {
	for (int i = 0; i < current; i++) enemies[i].Logic(map);
}