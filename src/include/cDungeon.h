#pragma once

#include "cTexture.h"

#define DUNGEON_FILENAME		"Dungeon"
#define DUNGEON_FILENAME_EXT	".csv"

#define DUNGEON_TILE_MAP_WIDTH  288
#define DUNGEON_TILE_MAP_HEIGHT 176


class cDungeon {
public:
	cDungeon(void);
	virtual ~cDungeon(void);

	bool LoadLevel(int level);
	void Draw(int tex_id, int zone);
	int *GetMap();

private:
	int map[DUNGEON_MAP_WIDTH * DUNGEON_MAP_HEIGHT];		// scene
};