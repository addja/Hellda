#include "include/cOctoroct.h"

cOctoroct::cOctoroct() {}

cOctoroct::cOctoroct(float posx, float posy, float step, bool th, int z, bool * overworld) {
	SetOriginalPosition(posx, posy);
	SetPosition(posx, posy);
	SetThrower(th);
	SetZone(z);
	SetStepLength(step);
	setOverworld(overworld);
	rebirth();
}


cOctoroct::~cOctoroct() {}

void cOctoroct::Draw(int tex_id, float playerx, float playery) {
	if (notInScreen(playerx, playery)) return;

	float x, y, xo, yo, xf, yf;
	GetPosition(&x, &y);
	
	// transform player post if it is on the limits of the map
	if (playerx < SCENE_WIDTH / 2) playerx = SCENE_WIDTH / 2;
	else if (playerx > OVERWORLD_MAP_WIDTH - (SCENE_WIDTH / 2)) playerx = OVERWORLD_MAP_WIDTH - (SCENE_WIDTH / 2);
	if (playery <= (SCENE_HEIGHT - HUD_TILES) / 2) playery = (SCENE_HEIGHT - HUD_TILES) / 2;
	else if (playery >= OVERWORLD_MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2) playery = OVERWORLD_MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2;

	switch (GetState()) {
	case STATE_LOOKLEFT:	xo = 1.0f / 9.0f;	yo = 0.5f;
		break;

	case STATE_LOOKRIGHT:	xo = 3.0f / 9.0f;	yo = 0.5f;
		break;

	case STATE_LOOKUP:		xo = 2.0f / 9.0f;	yo = 0.5f;
		break;

	case STATE_LOOKDOWN:	xo = 0.0f / 9.0f;	yo = 0.5f;
		break;

	case STATE_WALKLEFT:	xo = 1.0f / 9.0f;	yo = 0.5f + (GetFrame()*0.5f);
		NextFrame(2);
		break;
	case STATE_WALKRIGHT:	xo = 3.0f / 9.0f; yo = 0.5f + (GetFrame()*0.5f);
		NextFrame(2);
		break;
	case STATE_WALKUP:		xo = 2.0f / 9.0f;	yo = 0.5f + (GetFrame()*0.5f);
		NextFrame(2);
		break;
	case STATE_WALKDOWN:	xo = 0.0f / 9.0f; yo = 0.5f + (GetFrame()*0.5f);
		NextFrame(2);
		break;
	}

	xf = xo + 1.0f / 9.0f;
	yf = yo - 0.5f;

	DrawEntity(tex_id, xo, yo, xf, yf, x - playerx, playery - y);
}

void cOctoroct::Attack() {

}