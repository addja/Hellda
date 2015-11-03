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

void Draw(float playerx, float playery) 
{
	for (int i = 0; i < current; ++i) {
		enemies[i].Draw(playerx, playery);
	}
}

void addEnemy(float x, float y, int type, bool thrower)
{
	enemies[current] = cEnemy(x,y,type,thrower);
	++current;
}

//void addObject();

cEnemy *GetEnemies() {
	return enemies;
}