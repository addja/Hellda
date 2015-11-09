#pragma once

#include "cTexture.h"

#define DUNGEON_FILENAME		"Dungeon"
#define DUNGEON_FILENAME_EXT	".csv"

#define DUNGEON_TILE_MAP_WIDTH  288
#define DUNGEON_TILE_MAP_HEIGHT 176

#define LEFT_BORDER		0
#define RIGHT_BORDER	1
#define TOP_BORDER		2
#define BOTTOM_BORDER	3

class cDungeon {
public:
	cDungeon(void);
	virtual ~cDungeon(void);

	bool LoadLevel(int level);
	void Draw(int tex_id, int zone);
	void DrawBorder(int tex_id, int type);
	void DrawTile(int tex_id, int tile, float x, float y, float offset_x, float offset_y);
	int *GetMap();

private:
	int map[DUNGEON_MAP_WIDTH * DUNGEON_MAP_HEIGHT];		// scene
};