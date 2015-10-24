#include "include/cPepe.h"
#include "include/cScene.h"
#include "include/Globals.h"

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

void cPepe::SetPosition(int posx,int posy) {
	x = posx;
	y = posy;
}

void cPepe::GetPosition(int *posx,int *posy) {
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
	if ( (x % TILE_SIZE) != 0) width_tiles++;

	on_base = false;
	i=0;
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
	}
	return on_base;
}

void cPepe::GetArea(cRect *rc) {
	rc->left   = x;
	rc->right  = x+w;
	rc->bottom = y;
	rc->top    = y+h;
}

void cPepe::DrawRect(int tex_id,float xo,float yo,float xf,float yf) {
	int screen_x,screen_y;

	screen_x = x + SCENE_Xo;
	screen_y = y + SCENE_Yo + (BLOCK_SIZE - TILE_SIZE);

	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,tex_id);
	glBegin(GL_QUADS);	
		glTexCoord2f(xo,yo);	glVertex2i(screen_x  ,screen_y);
		glTexCoord2f(xf,yo);	glVertex2i(screen_x+w,screen_y);
		glTexCoord2f(xf,yf);	glVertex2i(screen_x+w,screen_y+h);
		glTexCoord2f(xo,yf);	glVertex2i(screen_x  ,screen_y+h);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void cPepe::MoveLeft(int *map) {
	int xaux;
	
	// Whats next tile?
	if ( (x % TILE_SIZE) == 0) {
		xaux = x;
		x -= STEP_LENGTH;

		if (CollidesMapWall(map,false)) {
			x = xaux;
			state = STATE_LOOKLEFT;
		}
	}
	// Advance, no problem
	else {
		x -= STEP_LENGTH;
		if (state != STATE_WALKLEFT) {
			state = STATE_WALKLEFT;
			seq = 0;
			delay = 0;
		}
	}
}

void cPepe::MoveRight(int *map) {
	int xaux;

	// Whats next tile?
	if ( (x % TILE_SIZE) == 0) {
		xaux = x;
		x += STEP_LENGTH;

		if (CollidesMapWall(map,true)) {
			x = xaux;
			state = STATE_LOOKRIGHT;
		}
	}
	// Advance, no problem
	else {
		x += STEP_LENGTH;

		if (state != STATE_WALKRIGHT) {
			state = STATE_WALKRIGHT;
			seq = 0;
			delay = 0;
		}
	}
}

void cPepe::Stop() {
	switch (state) {
		case STATE_WALKLEFT:	state = STATE_LOOKLEFT;		break;
		case STATE_WALKRIGHT:	state = STATE_LOOKRIGHT;	break;
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