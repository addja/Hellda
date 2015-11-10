#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gl/glut.h>
#include <string>
#include <iostream>
#include <vector>

// 1 -> on | 0 -> off
#define DEBUG_MODE 1

#define GAME_WIDTH	512
#define GAME_HEIGHT 512

#define SCENE_Xo		0
#define SCENE_Yo		0
#define SCENE_WIDTH		16
#define SCENE_HEIGHT	16

#define TILE_SIZE		16
#define BLOCK_SIZE		16

#define OVERWORLD_MAP_WIDTH		4096/TILE_SIZE
#define OVERWORLD_MAP_HEIGHT	1408/TILE_SIZE

#define DUNGEON_MAP_WIDTH	1536/TILE_SIZE
#define DUNGEON_MAP_HEIGHT	1056/TILE_SIZE

#define HUD_TILES	4

#define ZONE_WIDTH	16
#define ZONE_HEIGHT	11

#define INITIAL_POS_LINKX 120.0f
#define INITIAL_POS_LINKY 82.0f