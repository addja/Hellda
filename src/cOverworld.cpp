#include "include/cOverworld.h"
#include "include/Globals.h"

cOverworld::cOverworld(void) {}

cOverworld::~cOverworld(void) {}

bool cOverworld::Load() {
	bool res;
	FILE *fd;
	char file[16];
	int i,j;
	std::string tile;
	char c;

	res=true;

	sprintf(file, "%s%s", (char *)OVERWORLD_FILENAME,(char *)OVERWORLD_FILENAME_EXT);

	fd=fopen(file,"r");
	if (fd==NULL) return false;

	for(j= OVERWORLD_MAP_HEIGHT-1;j>=0;j--) {
		for(i=0;i<OVERWORLD_MAP_WIDTH;i++) {
			tile = "";
			fscanf(fd, "%c", &c);
			while (c != ',') {
				tile += c;
				fscanf(fd, "%c", &c);
			}

			int itile = atoi(tile.c_str());
			map[(j*OVERWORLD_MAP_WIDTH) + i] = itile;
		}
		fscanf(fd,"%c",&c); //pass enter
	}


	fclose(fd);

	return res;
}

void cOverworld::Draw(int tex_id, float playerx, float playery) {
	int i, j;
	float backgroundx, backgroundy;
	float coordx_tile, coordy_tile;

	// Test player position in the scene
	if (playerx < SCENE_WIDTH / 2) backgroundx = 0.0f;
	else if (playerx > OVERWORLD_MAP_WIDTH - (SCENE_WIDTH / 2)) backgroundx = OVERWORLD_MAP_WIDTH - SCENE_WIDTH;
	else backgroundx = playerx - (SCENE_WIDTH / 2);
	if (playery <= (SCENE_HEIGHT - HUD_TILES) / 2) backgroundy = OVERWORLD_MAP_HEIGHT + HUD_TILES;
	else if (playery >= OVERWORLD_MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2) backgroundy = SCENE_HEIGHT;
	else backgroundy = OVERWORLD_MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2 - playery + SCENE_HEIGHT;

	float offset_x = (float)16 / (float)OVERWORLD_TILE_MAP_WIDTH;
	float offset_y = (float)16 / (float)OVERWORLD_TILE_MAP_HEIGHT;
	float vx = GAME_WIDTH / SCENE_WIDTH;
	float vy = GAME_HEIGHT / (SCENE_HEIGHT - 1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,tex_id);
	
	glBegin(GL_QUADS);

	for (j = backgroundy;j >= backgroundy - SCENE_HEIGHT - 1;j--) {
		for (i = backgroundx;i<backgroundx + SCENE_WIDTH;i++) {
			int itile = map[(j*OVERWORLD_MAP_WIDTH) + i];

			coordy_tile = itile / (OVERWORLD_TILE_MAP_WIDTH / TILE_SIZE);
			coordx_tile = itile % (OVERWORLD_TILE_MAP_WIDTH / TILE_SIZE);

			float ni = i - backgroundx;
			float nj = j - backgroundy + SCENE_HEIGHT;
			glTexCoord2f(coordx_tile*offset_x, offset_y*(coordy_tile + 1));	glVertex2i(ni*vx, nj*vy);
			glTexCoord2f(offset_x*(coordx_tile + 1), offset_y*(coordy_tile + 1));	glVertex2i((ni + 1)*vx, nj*vy);
			glTexCoord2f(offset_x*(coordx_tile + 1), coordy_tile*offset_y);	glVertex2i((ni + 1)*vx, (nj + 1)*vy);
			glTexCoord2f(coordx_tile*offset_x, coordy_tile*offset_y);	glVertex2i(ni*vx, (nj + 1)*vy);
		}
	}

	glEnd();

	glDisable(GL_TEXTURE_2D);
}

int* cOverworld::GetMap() {
	return map;
}