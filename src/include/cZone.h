#pragma once

#include "cTexture.h"
#include "Globals.h"
#include "cEnemy.h"
#include "cOctoroct.h"
#include "cStalfos.h"
#include "cKeese.h"
#include "cBoss.h"

#define MAX_ENEMIES_PER_ZONE	10
#define MAX_OBJECTS_PER_ZONE	10

class cZone {
public:
	cZone(void);
	~cZone(void);

	void Reset();

	void Draw(float playerx, float playery);
	void Logic(int *map, float x, float y, int state);

	void addEnemy(float x, float y, int type, bool thrower, int zone, bool* overworld);
	//void addObject();

	cOctoroct* GetOctoroc(int id);
	cStalfos* GetStalfos(int id);
	cKeese* GetKeese(int id);
	cBoss* GetBoss();
	//cObject *GetObjects();

	void SetData(cData *data);
	void SetOverworld(bool over);
	int numOctorocs();
	int numStalfos();
	int numKeeses();
	bool isBossArea();

private:
	int oc_current;
	int st_current;
	int ke_current;
	bool overworld;
	bool boss_area;
	cData *Data;
	cOctoroct * oc_enemies[MAX_ENEMIES_PER_ZONE];
	cStalfos * st_enemies[MAX_ENEMIES_PER_ZONE];
	cKeese * ke_enemies[MAX_ENEMIES_PER_ZONE];
	cBoss * boss;
	//cObject objects[MAX_OBJECTS_PER_ZONE];
};
