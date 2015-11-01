#include "include/cPepe.h"
#include "include/cScene.h"
#include "include/Globals.h"
#include <iostream>

cPepe::cPepe(void) {
	seq=0;
	delay=0;

	jumping = false;
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

bool cPepe::Collides(cRect *rc) {
	return ((x>rc->left) && (x+w<rc->right) && (y>rc->bottom) && (y+h<rc->top));
}

bool cPepe::CollidesMapWall(int *map,bool right) {
	int tile_x,tile_y;
	int j;
	int width_tiles,height_tiles;

	tile_x = x / TILE_SIZE;
	tile_y = y / TILE_SIZE;
	width_tiles  = w / TILE_SIZE;
	height_tiles = h / TILE_SIZE;

	if (right)	tile_x += width_tiles;
	
	for (j=0;j<height_tiles;j++) {
		if (map[ tile_x + ((tile_y+j)*SCENE_WIDTH) ] != 0)	return true;
	}
	
	return false;
}

bool cPepe::CollidesMapFloor(int *map) {
	int tile_x,tile_y;
	int width_tiles;
	bool on_base;
	int i;

	tile_x = x / TILE_SIZE;
	tile_y = y / TILE_SIZE;

	width_tiles = w / TILE_SIZE;
	//if ( (x % TILE_SIZE) != 0) width_tiles++;

	on_base = false;
	/*i=0;
	while ((i<width_tiles) && !on_base) {
		if ( (y % TILE_SIZE) == 0 ) {
			if (map[ (tile_x + i) + ((tile_y - 1) * SCENE_WIDTH) ] != 0)
				on_base = true;
		} else {
			if (map[ (tile_x + i) + (tile_y * SCENE_WIDTH) ] != 0) {
				y = (tile_y + 1) * TILE_SIZE;
				on_base = true;
			}
		}
		i++;
	}*/
	return on_base;
}

void cPepe::GetArea(cRect *rc) {
	rc->left   = x;
	rc->right  = x+w;
	rc->bottom = y;
	rc->top    = y+h;
}

void cPepe::DrawRect(int tex_id,float xo,float yo,float xf,float yf, bool debug) {
	float screen_x,screen_y;

	screen_x = GAME_WIDTH / 2 + SCENE_Xo;
	screen_y = GAME_HEIGHT / 2 + SCENE_Yo;

	float vx = GAME_WIDTH / SCENE_WIDTH;
	float vy = GAME_HEIGHT / SCENE_HEIGHT;

	if (x < SCENE_WIDTH / 2) screen_x = x*vx;
	else if (x > MAP_WIDTH - (SCENE_WIDTH / 2)) screen_x = GAME_WIDTH - (MAP_WIDTH - x)*vx;
	if (y < SCENE_HEIGHT / 2) screen_y = GAME_HEIGHT - y*vy;
	else if (y > MAP_HEIGHT - (SCENE_HEIGHT / 2)) screen_y =  (MAP_HEIGHT - y)*vy;

	if (debug) {
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
	
	glBindTexture(GL_TEXTURE_2D,tex_id);
	glBegin(GL_QUADS);	
		glTexCoord2f(xo,yo);	glVertex2i(screen_x  ,screen_y);
		glTexCoord2f(xf,yo);	glVertex2i(screen_x+(vx / TILE_SIZE)*w,screen_y);
		glTexCoord2f(xf,yf);	glVertex2i(screen_x+ (vx / TILE_SIZE)*w,screen_y+(vy / TILE_SIZE)*h);
		glTexCoord2f(xo,yf);	glVertex2i(screen_x  ,screen_y+ (vy / TILE_SIZE)*h);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void cPepe::MoveUp(int *map) {
	float yaux = y - STEP_LENGTH;

	if (checkCorrectMovement(x, yaux, map)) y = yaux;

		if (state != STATE_WALKUP) {
			state = STATE_WALKUP;
			seq = 1;
			delay = 0;
		}
}

void cPepe::MoveDown(int *map) {
	float yaux = y + STEP_LENGTH;

	if (checkCorrectMovement(x, yaux, map)) y = yaux;

		if (state != STATE_WALKDOWN) {
			state = STATE_WALKDOWN;
			seq = 1;
			delay = 0;
		}
}

void cPepe::MoveLeft(int *map) {
	float xaux = x - STEP_LENGTH;

	if (checkCorrectMovement(xaux, y, map)) x = xaux;
		if (state != STATE_WALKLEFT) {
			state = STATE_WALKLEFT;
			seq = 1;
			delay = 0;
		}
}

void cPepe::MoveRight(int *map) {
	float xaux = x + STEP_LENGTH;

	if (checkCorrectMovement(xaux, y, map)) x = xaux;

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

void cPepe::Jump(int *map) {
	if (!jumping) {
		if (CollidesMapFloor(map)) {
			jumping = true;
			jump_alfa = 0;
			jump_y = y;
		}
	}
}

void cPepe::Logic(int *map) {
	float alfa;

	if (jumping) {
		jump_alfa += JUMP_STEP;
		
		if (jump_alfa == 180) {
			jumping = false;
			y = jump_y;
		} else {
			alfa = ((float)jump_alfa) * 0.017453f;
			y = jump_y + (int)( ((float)JUMP_HEIGHT) * sin(alfa) );
		
			if (jump_alfa > 90) {
				// Over floor?
				jumping = !CollidesMapFloor(map);
			}
		}
	} else {
		// Over floor?
		if (!CollidesMapFloor(map)) y -= (2*STEP_LENGTH);
	}
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

bool cPepe::checkCorrectMovement(float x, float y, int *map) {

	int tile, newx, newy;

	// x, y point to the base left of the Pepe
	cRect * r = new cRect();

	// check collision upper left bbox
	newx = floor(x);
	newy = floor(y + h);
	tile = map[newy*MAP_WIDTH + newx];
	//if (!(tile == 2 || tile == 8 || tile == 14)) return false;

	// check collision upper right bbox
	newx = floor(x + w);
	newy = floor(y + h);
	tile = map[newy*MAP_WIDTH + newx];
	//if (!(tile == 2 || tile == 8 || tile == 14)) return false;

	// check collision bottom left bbox
	newx = floor(x);
	newy = floor(y);
	tile = map[(MAP_HEIGHT-newy)*MAP_WIDTH + newx];

	std::cout << w << "\n";
	std::cout << "pos link " << x << " - " << y << " ------ ";
	std::cout << "bounding tile " << newx << " - " << newy << "\n";

	//if (!(tile == 2 || tile == 8 || tile == 14)) return false;

	// check collision bottom right bbox
	newx = floor(x - w);
	newy = floor(y);
	tile = map[newy*MAP_WIDTH + newx];
	//if (!(tile == 2 || tile == 8 || tile == 14)) return false;

	return true;
}