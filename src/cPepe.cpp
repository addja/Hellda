
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

	float hud_y = 4 * (GAME_HEIGHT / (SCENE_HEIGHT - 1));

	screen_x = GAME_WIDTH / 2 + SCENE_Xo;
	screen_y = (GAME_HEIGHT / 2) - hud_y + SCENE_Yo;

	float vx = GAME_WIDTH / SCENE_WIDTH;
	float vy = GAME_HEIGHT / (SCENE_HEIGHT - 1);

	if (x < SCENE_WIDTH / 2) screen_x = x*vx;
	else if (x > MAP_WIDTH - (SCENE_WIDTH / 2)) screen_x = GAME_WIDTH - (MAP_WIDTH - x)*vx;
	if (y < SCENE_HEIGHT / 2) screen_y += ((SCENE_HEIGHT / 2) - y)*vy;
	else if (y > MAP_HEIGHT - (SCENE_HEIGHT / 2) + HUD_TILES) screen_y -= (y - (MAP_HEIGHT - (SCENE_HEIGHT / 2) + HUD_TILES))*vy;

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

	int tile, newx, newy;

	// x, y point to the base left of the Pepe
	cRect * r = new cRect();

	if (DEBUG_MODE) std::cout << "I'm at x: " << x << " y: " << y << std::endl;

	switch (movement) {
		case STATE_WALKLEFT:
			// lower left corner
			newx = floor(x);
			newy = round(y);
			tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
			if (DEBUG_MODE)  std::cout << "L 1: " << tile << " x: " << newx << " y: " << newy << std::endl;
			if (!walkable(tile)) return false;
			
			// upper left corner / 2
			newx = floor(x);
			newy = round(y - 0.5f);
			tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
			if (DEBUG_MODE) std::cout << "L 2: " << tile << " x: " << newx << " y: " << newy << std::endl;
			if (!walkable(tile)) return false;
			
			break;
		
		case STATE_WALKRIGHT:
			// lower right corner
			newx = floor(x + 1);
			newy = round(y);
			tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
			if (DEBUG_MODE) std::cout << "R 1: " << tile << " x: " << newx << " y: " << newy << std::endl;
			if (!walkable(tile)) return false;
			
			// upper right corner / 2
			newx = floor(x + 1);
			newy = round(y - 0.5f);
			tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
			if (DEBUG_MODE) std::cout << "R 2: " << tile << " x: " << newx << " y: " << newy << std::endl;
			if (!walkable(tile)) return false;
			
			break;

		case STATE_WALKUP:
			// upper left corner / 2
			newx = floor(x);
			newy = round(y - 0.5f);
			tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
			if (DEBUG_MODE) std::cout << "U 1: " << tile << " x: " << newx << " y: " << newy <<std::endl;
			if (!walkable(tile)) {
				if (x - newx > 0.9) {
					// be careful to not go out of map!!!! (but beacuse of map form bounds it will never happen)
					tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx+1];
					if (walkable(tile)) {
						x = newx + 1;
						return true;
					}
				}
				return false;
			}
			
			// uper right corner / 2
			newx = floor(x + 1);
			newy = round(y - 0.5f);
			tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
			if (DEBUG_MODE) std::cout << "U 2: " << tile << " x: " << newx << " y: " << newy << std::endl;
			if (!walkable(tile)) {
				if (x - newx < 0.1) {
					// be careful to not go out of map!!!! (but beacuse of map form bounds it will never happen)
					tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx -1];
					if (walkable(tile)) {
						x = newx - 1;
						return true;
					}
				}
				return false;
			}
			
			break;

		case STATE_WALKDOWN:
			// lower left corner
			newx = floor(x);
			newy = round(y);
			tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
			if (DEBUG_MODE) std::cout << "D 1: " << tile << " x: " << newx << " y: " << newy << std::endl;
			if (!walkable(tile)) {
				if (x - newx > 0.9) {
					// be careful to not go out of map!!!! (but beacuse of map form bounds it will never happen)
					tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx + 1];
					if (walkable(tile)) {
						x = newx + 1;
						return true;
					}
				}
				return false;
			}
			
			// lower right corner
			newx = floor(x + 1);
			newy = round(y);
			tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx];
			if (DEBUG_MODE) std::cout << "D 2: " << tile << " x: " << newx << " y: " << newy << std::endl;
			if (!walkable(tile)) {
				if (x - newx < 0.1) {
					// be careful to not go out of map!!!! (but beacuse of map form bounds it will never happen)
					tile = map[(MAP_HEIGHT - newy - 1)*MAP_WIDTH + newx - 1];
					if (walkable(tile)) {
						x = newx - 1;
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
	return (tile == 2 || tile == 8 || tile == 14);
}

void cPepe::SetSeqNDelay(int s, int d) {
	seq = s;
	delay = d;
}

// draws a weapon or object having as a constraint that it occupies one tile given an offset from the Pepe
void cPepe::DrawWeapon(int tex_id, float xo, float yo, float xf, float yf, float offsetx, float offsety) {
	float screen_x, screen_y;

	float hud_y = 4 * (GAME_HEIGHT / (SCENE_HEIGHT - 1));

	screen_x = GAME_WIDTH / 2 + SCENE_Xo;
	screen_y = (GAME_HEIGHT / 2) - hud_y + SCENE_Yo;

	float vx = GAME_WIDTH / SCENE_WIDTH;
	float vy = GAME_HEIGHT / (SCENE_HEIGHT - 1);

	if (x < SCENE_WIDTH / 2) screen_x = x*vx;
	else if (x > MAP_WIDTH - (SCENE_WIDTH / 2)) screen_x = GAME_WIDTH - (MAP_WIDTH - x)*vx;
	if (y < SCENE_HEIGHT / 2) screen_y += ((SCENE_HEIGHT / 2) - y)*vy;
	else if (y > MAP_HEIGHT - (SCENE_HEIGHT / 2) + HUD_TILES) screen_y -= (y - (MAP_HEIGHT - (SCENE_HEIGHT / 2) + HUD_TILES))*vy;

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