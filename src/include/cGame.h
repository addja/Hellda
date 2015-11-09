
#pragma once

#include "cOverworld.h"
#include "cDungeon.h"
#include "cPlayer.h"
#include "cData.h"
#include "cZone.h"
#include <set>

#define OVERWORLD_ENEMIES_MAP_WIDTH		OVERWORLD_MAP_WIDTH/SCENE_WIDTH
#define OVERWORLD_ENEMIES_MAP_HEIGHT	OVERWORLD_MAP_HEIGHT/SCENE_HEIGHT
#define DUNGEON_ENEMIES_MAP_WIDTH		DUNGEON_MAP_WIDTH/SCENE_WIDTH
#define DUNGEON_ENEMIES_MAP_HEIGHT		DUNGEON_MAP_HEIGHT/SCENE_HEIGHT

class cGame {
public:

	cGame(void);
	virtual ~cGame(void);

	bool Init();
	bool Loop();
	void Finalize();

	// Input
	void ReadKeyboard(unsigned char key, int x, int y, bool press);
	void ReadMouse(int button, int state, int x, int y);			// TODO: we don't use the mouse
	
	// Process
	bool Process();

	// Output
	void Render();

	void initializeEnemiesOverworld();
	void initializeEnemiesDungeons();

	void initializeObjectsOverworld();
	void initializeObjectsDungeons();
	int calcZone(float x, float y);

private:
	unsigned char keys[256];
	cOverworld Overworld;
	cDungeon Dungeon;
	cPlayer Player;
	cData Data;
	cZone ZonesOverworld[OVERWORLD_ENEMIES_MAP_WIDTH * OVERWORLD_ENEMIES_MAP_HEIGHT];
	cZone ZonesDungeon[DUNGEON_ENEMIES_MAP_WIDTH * DUNGEON_ENEMIES_MAP_HEIGHT];
	bool overworld;
	bool transition;
	std::set<int> zones;
};
