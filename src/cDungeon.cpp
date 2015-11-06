#include "include/cDungeon.h"
#include "include/Globals.h"

cDungeon::cDungeon(void) {}

cDungeon::~cDungeon(void) {}

bool cDungeon::LoadLevel(int level) {
	bool res;
	FILE *fd;
	char file[16];
	int i,j;
	std::string tile;
	char c;

	res=true;

	/*if (level<10) 	sprintf(file,"%s0%d%s",(char *)FILENAME,level,(char *)FILENAME_EXT);
	else		 	sprintf(file,"%s%d%s",(char *)FILENAME,level,(char *)FILENAME_EXT);*/

	sprintf(file, "%s%d%s", (char *)DUNGEON_FILENAME, level,(char *)DUNGEON_FILENAME_EXT);

	fd=fopen(file,"r");
	if (fd==NULL) return false;

	for(j=DUNGEON_MAP_HEIGHT -1;j>=0;j--) {
		for(i=0;i<DUNGEON_MAP_WIDTH;i++) {
			tile = "";
			fscanf(fd, "%c", &c);
			while (c != ',') {
				tile += c;
				fscanf(fd, "%c", &c);
			}

			int itile = atoi(tile.c_str());
			map[(j*DUNGEON_MAP_WIDTH) + i] = itile;
		}
		fscanf(fd,"%c",&c); //pass enter
	}


	fclose(fd);

	return res;
}

void cDungeon::Draw(int tex_id, int zone) {
	int i, j;
	float coordx_tile, coordy_tile;

	int zonex = (zone % (DUNGEON_MAP_WIDTH/ZONE_WIDTH)) * ZONE_WIDTH;
	int zoney = DUNGEON_MAP_HEIGHT - floor(zone / (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_HEIGHT + ZONE_HEIGHT/2;

	float offset_x = (float)16 / (float)DUNGEON_TILE_MAP_WIDTH;
	float offset_y = (float)16 / (float)DUNGEON_TILE_MAP_HEIGHT;
	float vx = GAME_WIDTH / SCENE_WIDTH;
	float vy = GAME_HEIGHT / (SCENE_HEIGHT - 1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,tex_id);
	
	glBegin(GL_QUADS);

	for (j = zoney;j >= zoney - SCENE_HEIGHT - 1 - HUD_TILES;j--) {
		for (i = zonex;i<zonex + SCENE_WIDTH;i++) {
			int itile = map[(j*DUNGEON_MAP_WIDTH) + i];

			coordy_tile = itile / (DUNGEON_TILE_MAP_WIDTH / TILE_SIZE);
			coordx_tile = itile % (DUNGEON_TILE_MAP_WIDTH / TILE_SIZE);

			float ni = i - zonex;
			float nj = j - zoney + SCENE_HEIGHT;
			glTexCoord2f(coordx_tile*offset_x, offset_y*(coordy_tile + 1));	glVertex2i(ni*vx, nj*vy);
			glTexCoord2f(offset_x*(coordx_tile + 1), offset_y*(coordy_tile + 1));	glVertex2i((ni + 1)*vx, nj*vy);
			glTexCoord2f(offset_x*(coordx_tile + 1), coordy_tile*offset_y);	glVertex2i((ni + 1)*vx, (nj + 1)*vy);
			glTexCoord2f(coordx_tile*offset_x, coordy_tile*offset_y);	glVertex2i(ni*vx, (nj + 1)*vy);
		}
	}

	glEnd();

	glDisable(GL_TEXTURE_2D);
}

int* cDungeon::GetMap() {
	return map;
}