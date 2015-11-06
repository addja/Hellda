#pragma once

#include "cTexture.h"

#define OVERWORLD_FILENAME		"Hellda-map"
#define OVERWORLD_FILENAME_EXT	".csv"

#define OVERWORLD_TILE_MAP_WIDTH  288
#define OVERWORLD_TILE_MAP_HEIGHT 128


class cOverworld {
public:
	cOverworld(void);
	virtual ~cOverworld(void);

	bool Load();
	void Draw(int tex_id, float playerx, float playery);
	int *GetMap();

private:
	int map[OVERWORLD_MAP_WIDTH * OVERWORLD_MAP_HEIGHT];		// scene
};