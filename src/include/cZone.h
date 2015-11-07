#pragma once

#include "cTexture.h"
#include "Globals.h"
#include "cEnemy.h"

#define MAX_ENEMIES_PER_ZONE	10
#define MAX_OBJECTS_PER_ZONE	10

class cZone {
public:
	cZone(void);
	~cZone(void);

	void Reset();

	void Draw(float playerx, float playery);
	void Logic(int *map);

	void addEnemy(float x, float y, int type, bool thrower, int zone, bool* overworld);
	//void addObject();

	cEnemy *GetEnemies();
	//cObject *GetObjects();

	void SetData(cData *data);
	void SetOverworld(bool over);

private:
	int current;
	bool overworld;
	cData *Data;
	cEnemy enemies[MAX_ENEMIES_PER_ZONE];
	//cObject objects[MAX_OBJECTS_PER_ZONE];
};
