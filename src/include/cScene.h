#pragma once

#include "cTexture.h"

#define SCENE_Xo		0
#define SCENE_Yo		0
#define SCENE_WIDTH		16
#define SCENE_HEIGHT	11

#define FILENAME		"Hellda-map"
#define FILENAME_EXT	".csv"
#define MAP_WIDTH		4096/16
#define MAP_HEIGHT		1408/16

#define TILE_SIZE		16
#define BLOCK_SIZE		16
#define TILE_MAP_WIDTH  288
#define TILE_MAP_HEIGHT 128


class cScene {
public:
	cScene(void);
	virtual ~cScene(void);

	bool LoadLevel(int level);
	void Draw(int tex_id);
	int *GetMap();

private:
	int map[MAP_WIDTH * MAP_HEIGHT];		//scene
	int id_DL;								//actual level display list
};