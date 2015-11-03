
#include "include/cPepe.h"
#include "include/cScene.h"
#include "include/Globals.h"
#include <iostream>

cPepe::cPepe(void) {
	seq=0;
	delay=0;
}

cPepe::~cPepe(void){}

cPepe::cPepe(int posx,int posy,int width,int height) {
	x = posx;
	y = posy;
	w = width;
	h = height;
}

void cPepe::SetPosition(float posx, float posy) {
	x = posx;
	y = posy;
}

void cPepe::GetPosition(float *posx, float *posy) {
	*posx = x;
	*posy = y;
}

void cPepe::SetTile(int tx,int ty) {
	x = tx * TILE_SIZE;
	y = ty * TILE_SIZE;
}

void cPepe::GetTile(int *tx,int *ty) {
	*tx = x / TILE_SIZE;
	*ty = y / TILE_SIZE;
}

void cPepe::SetWidthHeight(int width,int height) {
	w = width;
	h = height;
}

void cPepe::GetWidthHeight(int *width,int *height) {
	*width = w;
	*height = h;
}

void cPepe::DrawRect(int tex_id,float xo,float yo,float xf,float yf) {
	float screen_x, screen_y;

	float vx = GAME_WIDTH / SCENE_WIDTH;
	float vy = GAME_HEIGHT / (SCENE_HEIGHT - 1);

	float hud_y = HUD_TILES * vx;

	screen_x = GAME_WIDTH / 2 + SCENE_Xo;
	screen_y = (((SCENE_HEIGHT - HUD_TILES) / 2) - 1)*vy + SCENE_Yo;

	if (x < SCENE_WIDTH / 2) screen_x = x*vx;
	else if (x > MAP_WIDTH - (SCENE_WIDTH / 2)) screen_x = GAME_WIDTH - (MAP_WIDTH - x)*vx;
	if (y <= (SCENE_HEIGHT - HUD_TILES) / 2) screen_y += ((SCENE_HEIGHT - HUD_TILES) / 2 - y)*vy;
	else if (y >= MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2) screen_y -= (y - (MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2))*vy;

	if (DEBUG_MODE) {
		glColor3f(1.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
			glVertex2i(screen_x, screen_y);
			glVertex2i(screen_x + (vx / TILE_SIZE)*w, screen_y);
			glVertex2i(screen_x + (vx / TILE_SIZE)*w, screen_y + (vy / TILE_SIZE)*h);
			glVertex2i(screen_x, screen_y + (vy / TILE_SIZE)*h);
		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
	}


	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, tex_id);
	glBegin(GL_QUADS);
		glTexCoord2f(xo, yo);	glVertex2i(screen_x, screen_y);
		glTexCoord2f(xf, yo);	glVertex2i(screen_x + (vx / TILE_SIZE)*w, screen_y);
		glTexCoord2f(xf, yf);	glVertex2i(screen_x + (vx / TILE_SIZE)*w, screen_y + (vy / TILE_SIZE)*h);
		glTexCoord2f(xo, yf);	glVertex2i(screen_x, screen_y + (vy / TILE_SIZE)*h);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void cPepe::MoveUp(int *map) {
	float yaux = y - STEP_LENGTH;

	if (checkCorrectMovement(x, yaux, map, STATE_WALKUP)) y = yaux;
		if (state != STATE_WALKUP) {
			state = STATE_WALKUP;
			seq = 1;
			delay = 0;
		}
}

void cPepe::MoveDown(int *map) {
	float yaux = y + STEP_LENGTH;

	if (checkCorrectMovement(x, yaux, map, STATE_WALKDOWN)) y = yaux;

		if (state != STATE_WALKDOWN) {
			state = STATE_WALKDOWN;
			seq = 1;
			delay = 0;
		}
}

void cPepe::MoveLeft(int *map) {
	float xaux = x - STEP_LENGTH;

	if (checkCorrectMovement(xaux, y, map, STATE_WALKLEFT)) x = xaux;
		if (state != STATE_WALKLEFT) {
			state = STATE_WALKLEFT;
			seq = 1;
			delay = 0;
		}
}

void cPepe::MoveRight(int *map) {
	float xaux = x + STEP_LENGTH;

	if (checkCorrectMovement(xaux, y, map, STATE_WALKRIGHT)) x = xaux;

		if (state != STATE_WALKRIGHT) {
			state = STATE_WALKRIGHT;
			seq = 1;
			delay = 0;
		}
}

void cPepe::Stop() {
	switch (state) {
		case STATE_WALKLEFT:	state = STATE_LOOKLEFT;		break;
		case STATE_WALKRIGHT:	state = STATE_LOOKRIGHT;	break;
		case STATE_WALKUP:		state = STATE_LOOKUP;		break;
		case STATE_WALKDOWN:	state = STATE_LOOKDOWN;		break;
	}
}

void cPepe::Logic(int *map) {

	// Invent a fucking logic man
	
}

void cPepe::NextFrame(int max) {
	delay++;
	if (delay == FRAME_DELAY) {
		seq++;
		seq%=max;
		delay = 0;
	}
}

int cPepe::GetFrame() {
	return seq;
}

int cPepe::GetState() {
	return state;
}

void cPepe::SetState(int s) {
	state = s;
}

bool cPepe::checkCorrectMovement(float & x, float & y, int *map, int movement) {

	int tile, tile2, newx, newy, newx2, newy2;

	// x, y point to the base left of the Pepe
	cRect * r = new cRect();

	if (DEBUG_MODE) std::cout << "I'm at x: " << x << " y: " << y << std::endl;

	switch (movement) {
	case STATE_WALKLEFT:
		// lower left corner
		newx = floor(x + 0.05f);
		newy = ceil(y - 0.05f);
		tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
		if (DEBUG_MODE)  std::cout << "L 1: " << tile << " x: " << newx << " y: " << newy << std::endl;
		if (!walkable(tile)) {
			if (diagonallyWalkableDownLeft(tile) || diagonallyWalkableDownRight(tile)) {
				y -= STEP_LENGTH;
				return true;
			}
			else if ((newy - y <= 0.55f && diagonallyWalkableUpLeft(tile)) || diagonallyWalkableUpRight(tile)) {
				y += STEP_LENGTH;
				return true;
			}
			return false;
		}

		// upper left corner / 2
		newx = floor(x + 0.05f);
		newy = ceil(y - 0.5f);
		tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
		if (DEBUG_MODE) std::cout << "L 2: " << tile << " x: " << newx << " y: " << newy << std::endl;
		if (!walkable(tile)) {
			if (diagonallyWalkableDownLeft(tile) || diagonallyWalkableDownRight(tile)) {
				y -= STEP_LENGTH;
				return true;
			}
			else if ((newy - y <= 0.55f && diagonallyWalkableUpLeft(tile)) || diagonallyWalkableUpRight(tile)) {
				y += STEP_LENGTH;
				return true;
			}
			return false;
		}

		break;

	case STATE_WALKRIGHT:
		// lower right corner
		newx = floor(x + 0.95f);
		newy = ceil(y - 0.05f);
		tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
		if (DEBUG_MODE) std::cout << "R 1: " << tile << " x: " << newx << " y: " << newy << std::endl;
		if (!walkable(tile)) {
			if ((diagonallyWalkableDownRight(tile)) || diagonallyWalkableDownLeft(tile)) {
				y -= STEP_LENGTH;
				return true;
			}
			else if ((newy - y <= 0.55f && diagonallyWalkableUpRight(tile)) || diagonallyWalkableUpLeft(tile)) {
				y += STEP_LENGTH;
				return true;
			}
			return false;
		}

		// upper right corner / 2
		newx = floor(x + 0.95f);
		newy = ceil(y - 0.5f);
		tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
		if (DEBUG_MODE) std::cout << "R 2: " << tile << " x: " << newx << " y: " << newy << std::endl;
		if (!walkable(tile)) {
			if ((diagonallyWalkableDownRight(tile)) || diagonallyWalkableDownLeft(tile)) {
				y -= STEP_LENGTH;
				return true;
			}
			else if ((newy - y <= 0.55f && diagonallyWalkableUpRight(tile)) || diagonallyWalkableUpLeft(tile)) {
				y += STEP_LENGTH;
				return true;
			}
			return false;
		}

		break;

	case STATE_WALKUP:
		// uper right corner / 2
		newx2 = floor(x + 1);
		newy2 = ceil(y - 0.5f);
		tile2 = map[(MAP_HEIGHT - newy2 - 1)*MAP_WIDTH + newx2];

		// upper left corner / 2
		newx = floor(x);
		newy = ceil(y - 0.5f);
		tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
		if (DEBUG_MODE) std::cout << "U 1: " << tile << " x: " << newx << " y: " << newy << std::endl;
		if (!walkable(tile)) {
			if (diagonallyWalkableUpLeft(tile) && !diagonallyWalkableUpRight(tile2)) {
				x += STEP_LENGTH;
				return true;
			}
			else if (diagonallyWalkableUpLeft(tile) && diagonallyWalkableUpRight(tile2) && newx2 - x > x - newx && y > newy) {
				x += STEP_LENGTH;
				return true;
			}
			else if (diagonallyWalkableUpLeft(tile) && diagonallyWalkableUpRight(tile2) && newx2 - x < x - newx && y > newy) {
				x -= STEP_LENGTH;
				return true;
			}
			else if (x - floor(x) >= 0.75) {
				// be careful to not go out of map!!!! (but beacuse of map form bounds it will never happen)
				tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx + 1];
				if (walkable(tile)) {
					x = newx + 1;
					return true;
				}
			}
			return false;
		}

		if (DEBUG_MODE) std::cout << "U 2: " << tile2 << " x: " << newx2 << " y: " << newy2 << std::endl;
		if (!walkable(tile2)) {
			if (diagonallyWalkableUpRight(tile2)) {
				x -= STEP_LENGTH;
				return true;
			}
			else if (x - (float)floor(x) <= 0.25f) {
				// be careful to not go out of map!!!! (but beacuse of map form bounds it will never happen)
				tile2 = map[(MAP_HEIGHT - newy2 - 1)*MAP_WIDTH + newx2 - 1];
				if (walkable(tile2)) {
					x = newx2 - 1;
					return true;
				}
			}
			return false;
		}

		break;

	case STATE_WALKDOWN:
		// lower right corner
		newx2 = floor(x + 1);
		newy2 = ceil(y - 0.05f);
		tile2 = map[(MAP_HEIGHT - newy2 - 1)*MAP_WIDTH + newx2];

		// lower left corner
		newx = floor(x);
		newy = ceil(y - 0.05f);
		tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
		if (DEBUG_MODE) std::cout << "D 1: " << tile << " x: " << newx << " y: " << newy << std::endl;
		if (!walkable(tile)) {
			if (diagonallyWalkableDownLeft(tile) && !diagonallyWalkableDownRight(tile2)) {
				x += STEP_LENGTH;
				return true;
			}
			else if (diagonallyWalkableDownLeft(tile) && diagonallyWalkableDownRight(tile2) && newx2 - x > x - newx && y < newy - 0.5f) {
				x += STEP_LENGTH;
				return true;
			}
			else if (diagonallyWalkableDownLeft(tile) && diagonallyWalkableDownRight(tile2) && newx2 - x < x - newx && y < newy - 0.5f) {
				x -= STEP_LENGTH;
				return true;
			}
			else if (x - (float)floor(x) >= 0.75f) {
				// be careful to not go out of map!!!! (but beacuse of map form bounds it will never happen)
				tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx + 1];
				if (walkable(tile)) {
					x = newx + 1;
					return true;
				}
			}
			return false;
		}


		if (DEBUG_MODE) std::cout << "D 2: " << tile2 << " x: " << newx2 << " y: " << newy2 << std::endl;
		if (!walkable(tile2)) {
			if (diagonallyWalkableDownRight(tile2)) {
				x -= STEP_LENGTH;
				return true;
			}
			else if (x - (float)floor(x) <= 0.25f) {
				// be careful to not go out of map!!!! (but beacuse of map form bounds it will never happen)
				tile2 = map[(MAP_HEIGHT - newy2 - 1)*MAP_WIDTH + newx2 - 1];
				if (walkable(tile2)) {
					x = newx2 - 1;
					return true;
				}
			}
			return false;
		}

		break;
	}

	return true;
}

bool cPepe::walkable(int tile) {
	return (tile == 2 || tile == 8 || tile == 14 || tile == 18 || tile == 24 || tile == 30 || tile == 58 ||
		tile == 64 || tile == 70 || tile == 75 || tile == 76 || tile == 77 || tile == 81 || tile == 82 ||
		tile == 83 || tile == 87 || tile == 88 || tile == 89 || tile == 93 || tile == 94 || tile == 95 ||
		tile == 99 || tile == 100 || tile == 101 || tile == 105 || tile == 106 || tile == 107 || tile == 111 ||
		tile == 112 || tile == 113 || tile == 117 || tile == 118 || tile == 119 || tile == 123 || tile == 124 ||
		tile == 125 || tile == 126 || tile == 127 || tile == 131 || tile == 132 ||
		tile == 133 || tile == 137 || tile == 138 || tile == 139 || tile == 140 ||
		tile == 141 || tile == 143);
}

// TODO: Tiles del arbol

bool cPepe::diagonallyWalkableUpRight(int tile) {
	return (tile == 54 || tile == 60 || tile == 66 || tile == 127 || tile == 133 || tile == 139 || tile == 108 ||
			tile == 114 || tile == 120);
}

bool cPepe::diagonallyWalkableUpLeft(int tile) {
	return (tile == 56 || tile == 62 || tile == 68 || tile == 126 || tile == 132 || tile == 138 || tile == 110 ||
			tile == 116 || tile == 122);
}

bool cPepe::diagonallyWalkableDownRight(int tile) {
	return (tile == 36 || tile == 42 || tile == 48 || tile == 129 || tile == 135 || tile == 141 || tile == 72 ||
		tile == 78 || tile == 84 || tile == 39 || tile == 45 || tile == 51);
}

bool cPepe::diagonallyWalkableDownLeft(int tile) {
	return (tile == 38 || tile == 44 || tile == 50 || tile == 128 || tile == 134 || tile == 140 || tile == 74 ||
		tile == 80 || tile == 86 || tile == 41 || tile == 47 || tile == 53);
}

bool cPepe::dungeonTransitions(int tile) {
	return (tile == 0 || tile == 6 || tile == 12 || tile == 22 || tile == 28 || tile == 34);
}

void cPepe::SetSeqNDelay(int s, int d) {
	seq = s;
	delay = d;
}

// draws a weapon or object having as a constraint that it occupies one tile given an offset from the Pepe
void cPepe::DrawWeapon(int tex_id, float xo, float yo, float xf, float yf, float offsetx, float offsety) {
	float screen_x, screen_y;

	float vx = GAME_WIDTH / SCENE_WIDTH;
	float vy = GAME_HEIGHT / (SCENE_HEIGHT - 1);

	float hud_y = HUD_TILES * vx;

	screen_x = GAME_WIDTH / 2 + SCENE_Xo;
	screen_y = (((SCENE_HEIGHT - HUD_TILES) / 2) - 1)*vy + SCENE_Yo;

	if (x < SCENE_WIDTH / 2) screen_x = x*vx;
	else if (x > MAP_WIDTH - (SCENE_WIDTH / 2)) screen_x = GAME_WIDTH - (MAP_WIDTH - x)*vx;
	if (y <= (SCENE_HEIGHT - HUD_TILES) / 2) screen_y += ((SCENE_HEIGHT - HUD_TILES) / 2 - y)*vy;
	else if (y >= MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2) screen_y -= (y - (MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2))*vy;

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, tex_id);
	glBegin(GL_QUADS);
	glTexCoord2f(xo, yo);	glVertex2i(screen_x + vx*offsetx, screen_y + vy*offsety);
	glTexCoord2f(xf, yo);	glVertex2i(screen_x + (vx / TILE_SIZE)*w + vx*offsetx,
										screen_y + vy*offsety);
	glTexCoord2f(xf, yf);	glVertex2i(screen_x + (vx / TILE_SIZE)*w + vx*offsetx,
										screen_y + (vy / TILE_SIZE)*h + vy*offsety);
	glTexCoord2f(xo, yf);	glVertex2i(screen_x + vx*offsetx,
										screen_y + (vy / TILE_SIZE)*h + vy*offsety);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}