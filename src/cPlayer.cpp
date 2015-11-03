#include "include/cPlayer.h"

cPlayer::cPlayer(){}

cPlayer::~cPlayer(){}

void cPlayer::Draw(int tex_id) {	
	float xo,yo,xf,yf;

	// to write a weapon
	float posx, posy;
	GetPosition(&posx, &posy);

	switch (GetState()) {
		case STATE_LOOKLEFT:	xo = 0.25f;	yo = 0.25f;
								break;
		
		case STATE_LOOKRIGHT:	xo = 0.75f;	yo = 0.25f;
								break;

		case STATE_LOOKUP:		xo = 0.5f;	yo = 0.25f;
								break;
			
		case STATE_LOOKDOWN:	xo = 0.0f;	yo = 0.25f;
								break;

		case STATE_WALKLEFT:	xo = 0.25f;	yo = 0.25f + (GetFrame()*0.25f);
								NextFrame(2);
								break;
		case STATE_WALKRIGHT:	xo = 0.75f; yo = 0.25f + (GetFrame()*0.25f);
								NextFrame(2);
								break;
		case STATE_WALKUP:		xo = 0.5f;	yo = 0.25f + (GetFrame()*0.25f);
								NextFrame(2);
								break;
		case STATE_WALKDOWN:	xo = 0.0f; yo = 0.25f + (GetFrame()*0.25f);
								NextFrame(2);
								break;
		case ATTACK_UP:			xo = 0.5f; yo = 0.75f;
								SetState(STATE_LOOKUP);
								SetSeqNDelay(1, 0);
								DrawEntity(tex_id, xo, yo + 0.25f, xo + 0.25f, yo, 0.0f, 0.8f);
								break;
		case ATTACK_LEFT:		xo = 0.25f; yo = 0.75f;
								SetState(STATE_LOOKLEFT);
								SetSeqNDelay(1, 0);
								DrawEntity(tex_id, xo, yo + 0.25f, xo + 0.25f, yo, -0.8f, 0.0f);
								break;
		case ATTACK_RIGHT:		xo = 0.75f; yo = 0.75f;
								SetState(STATE_LOOKRIGHT);
								SetSeqNDelay(1, 0);
								DrawEntity(tex_id, xo, yo + 0.25f, xo + 0.25f, yo, 0.8f, 0.0f);
								break;
		case ATTACK_DOWN:		xo = 0.0f; yo = 0.75f;
								SetState(STATE_LOOKDOWN);
								SetSeqNDelay(1, 0);
								DrawEntity(tex_id, xo, yo+0.25f, xo+0.25f, yo,0.0f,-0.8f);
								break;
	}
	xf = xo + 0.25f;
	yf = yo - 0.25f;
	
	DrawRect(tex_id, xo, yo, xf, yf);
}

void cPlayer::Attack() {

	if (GetState() == STATE_WALKDOWN || GetState() == STATE_LOOKDOWN) {
		SetState(ATTACK_DOWN);
		SetSeqNDelay(1, 0);
	}
	else if (GetState() == STATE_WALKUP || GetState() == STATE_LOOKUP) {
		SetState(ATTACK_UP);
		SetSeqNDelay(1, 0);
	}
	else if (GetState() == STATE_WALKLEFT || GetState() == STATE_LOOKLEFT) {
		SetState(ATTACK_LEFT);
		SetSeqNDelay(1, 0);
	}
	else if (GetState() == STATE_WALKRIGHT || GetState() == STATE_LOOKRIGHT) {
		SetState(ATTACK_RIGHT);
		SetSeqNDelay(1, 0);
	}
}

void cPlayer::DrawRect(int tex_id, float xo, float yo, float xf, float yf) {
	float screen_x, screen_y;

	float x, y;
	int w, h;
	GetPosition(&x, &y);
	GetWidthHeight(&w, &h);

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