#include "include/cOctoroct.h"

cOctoroct::cOctoroct() {}

cOctoroct::cOctoroct(float posx, float posy, float step, bool th, bool drop, int z, bool * overworld) {
	SetOriginalPosition(posx, posy);
	SetPosition(posx, posy);
	SetThrower(th);
	SetZone(z);
	SetStepLength(step);
	setOverworld(overworld);
	SetDropper(drop);
	endShoot();
	rebirth();
	SetHealth(1);
}


cOctoroct::~cOctoroct() {}

void cOctoroct::Draw(int tex_id, float playerx, float playery) {
	if (notInScreen(playerx, playery)) return;

	float xo, yo, xf, yf;

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

	DrawEntity(tex_id, xo, yo, xf, yf, playerx, playery);

	if (hasShoot()) {
		float bx, by;
		getBulletPos(bx, by);
		xo = 8.0f / 9.0f;	yo = 0.5f + (GetFrame()*0.5f);
		xf = xo + 1.0f / 9.0f;
		yf = yo - 0.5f;
		DrawBullet(tex_id, xo, yo, xf, yf, playerx, playery, bx, by);
	}
}

void cOctoroct::Attack() {

}