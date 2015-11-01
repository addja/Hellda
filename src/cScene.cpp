#include "include/cScene.h"
#include "include/Globals.h"

cScene::cScene(void) {}

cScene::~cScene(void) {}

bool cScene::LoadLevel(int level) {
	bool res;
	FILE *fd;
	char file[16];
	int i,j;
	std::string tile;
	char c;

	res=true;

	/*if (level<10) 	sprintf(file,"%s0%d%s",(char *)FILENAME,level,(char *)FILENAME_EXT);
	else		 	sprintf(file,"%s%d%s",(char *)FILENAME,level,(char *)FILENAME_EXT);*/

	sprintf(file, "%s%s", (char *)FILENAME,(char *)FILENAME_EXT);

	fd=fopen(file,"r");
	if (fd==NULL) return false;

	for(j=MAP_HEIGHT-1;j>=0;j--) {
		for(i=0;i<MAP_WIDTH;i++) {
			tile = "";
			fscanf(fd, "%c", &c);
			while (c != ',') {
				tile += c;
				fscanf(fd, "%c", &c);
			}

			int itile = atoi(tile.c_str());
			map[(j*MAP_WIDTH) + i] = itile;
		}
		fscanf(fd,"%c",&c); //pass enter
	}


	fclose(fd);

	return res;
}

void cScene::Draw(int tex_id, float playerx, float playery) {
	int i, j;
	float backgroundx, backgroundy;
	float coordx_tile, coordy_tile;
	/*float playerx = 120.0f;
	float playery = 88.0f;*/
	// Test player position in the scene
	if (playerx < SCENE_WIDTH / 2) backgroundx = 0.0f;
	else if (playerx > MAP_WIDTH - (SCENE_WIDTH / 2)) backgroundx = MAP_WIDTH - SCENE_WIDTH;
	else backgroundx = playerx - (SCENE_WIDTH / 2);
	if (playery < SCENE_HEIGHT / 2) backgroundy = MAP_HEIGHT + 4;
	else if (playery > MAP_HEIGHT - (SCENE_HEIGHT / 2) + 4) backgroundy = SCENE_HEIGHT;
	else backgroundy = MAP_HEIGHT - playery + (SCENE_HEIGHT / 2) + 4;

	float offset_x = (float)16 / (float)TILE_MAP_WIDTH;
	float offset_y = (float)16 / (float)TILE_MAP_HEIGHT;
	//float statusbar_y = 4 * (GAME_HEIGHT / (SCENE_HEIGHT - 1 + 4));
	float vx = GAME_WIDTH / SCENE_WIDTH;
	float vy = GAME_HEIGHT / (SCENE_HEIGHT - 1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,tex_id);
	
	glBegin(GL_QUADS);

	for (j = backgroundy;j >= backgroundy - SCENE_HEIGHT - 1;j--) {
		for (i = backgroundx;i<backgroundx + SCENE_WIDTH;i++) {
			int itile = map[(j*MAP_WIDTH) + i];

			coordy_tile = itile / (TILE_MAP_WIDTH / TILE_SIZE);
			coordx_tile = itile % (TILE_MAP_WIDTH / TILE_SIZE);

			// BLOCK_SIZE = 16, FILE_WIDTH = 288, FILE_HEIGTH = 128
			// 16 / 288 = 0.05
			// 16 / 128 = 0.125
			float ni = i - backgroundx;
			float nj = j - backgroundy + SCENE_HEIGHT;
			glTexCoord2f(coordx_tile*offset_x, offset_y*(coordy_tile + 1));	glVertex2i(ni*vx, nj*vy);
			glTexCoord2f(offset_x*(coordx_tile + 1), offset_y*(coordy_tile + 1));	glVertex2i((ni + 1)*vx, nj*vy);
			glTexCoord2f(offset_x*(coordx_tile + 1), coordy_tile*offset_y);	glVertex2i((ni + 1)*vx, (nj + 1)*vy);
			glTexCoord2f(coordx_tile*offset_x, coordy_tile*offset_y);	glVertex2i(ni*vx, (nj + 1)*vy);
			/*glTexCoord2f(coordx_tile*offset_x, offset_y*(coordy_tile + 1));	glVertex2i(ni, nj);
			glTexCoord2f(offset_x*(coordx_tile + 1), offset_y*(coordy_tile + 1));	glVertex2i(ni + TILE_SIZE, nj);
			glTexCoord2f(offset_x*(coordx_tile + 1), coordy_tile*offset_y);	glVertex2i(ni + TILE_SIZE, nj + TILE_SIZE);
			glTexCoord2f(coordx_tile*offset_x, coordy_tile*offset_y);	glVertex2i(ni, nj + TILE_SIZE);*/
		}
	}

	glEnd();

	glDisable(GL_TEXTURE_2D);
}

int* cScene::GetMap() {
	return map;
}