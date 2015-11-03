#pragma once

#include "cTexture.h"
#include "Globals.h"

#define MAX_ENEMIES_PER_ZONE	10
#define MAX_OBJECTS_PER_ZONE	10

class cZone
{
public:
	cZone(void);
	~cZone(void);

	void Reset();

	void Draw(float playerx, float playery);

	void addEnemy(float x, float y, int type, bool thrower);
	//void addObject();

	cEnemy *GetEnemies();
	//cObject *GetObjects();

private:
	int current;
	cEnemy enemies[MAX_ENEMIES_PER_ZONE];
	//cObject objects[MAX_OBJECTS_PER_ZONE];
};
