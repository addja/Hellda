#pragma once

#include "cTexture.h"
#include "Globals.h"
#include "cEnemy.h"
#include "cOctoroct.h"
#include "cStalfos.h"
#include "cKeese.h"
#include "cBoss.h"
#include "cObject.h"

#define MAX_ENEMIES_PER_ZONE	10
#define MAX_OBJECTS_PER_ZONE	10

class cZone {
public:
	cZone(void);
	~cZone(void);

	void Reset();

	void Draw(float playerx, float playery);
	void Logic(int *map, float x, float y, int state, bool bullet, float bx, float by, bool & hit);

	void addEnemy(float x, float y, int type, bool thrower, bool dropper, int zone, bool* overworld);
	void addObject(float x, float y, int type, bool shown, int zone, bool* overworld);

	cOctoroct* GetOctoroc(int id);
	cStalfos* GetStalfos(int id);
	cKeese* GetKeese(int id);
	cObject* GetObj(int id);
	cBoss* GetBoss();

	void SetData(cData *data);
	void SetOverworld(bool over);
	int numOctorocs();
	int numStalfos();
	int numKeeses();
	int numObjects();
	bool isBossArea();
	bool allEnemiesDead();

private:
	int oc_current;
	int st_current;
	int ke_current;
	int obj_current;
	bool overworld;
	bool boss_area;
	cData *Data;
	cOctoroct * oc_enemies[MAX_ENEMIES_PER_ZONE];
	cStalfos * st_enemies[MAX_ENEMIES_PER_ZONE];
	cKeese * ke_enemies[MAX_ENEMIES_PER_ZONE];
	cBoss * boss;
	cObject * objects[MAX_OBJECTS_PER_ZONE];
};
