#include "include/cBoss.h"

cBoss::cBoss() {}

cBoss::cBoss(float posx, float posy, float step, bool th, int z, bool * overworld) {
	SetOriginalPosition(posx, posy);
	SetPosition(posx, posy);
	SetThrower(th);
	SetZone(z);
	SetStepLength(step);
	setOverworld(overworld);
	rebirth();
}


cBoss::~cBoss() {}

void cBoss::Draw(int tex_id, int tex_id_shoot, float playerx, float playery) {
	if (notInScreen(playerx, playery)) return;

	switch (GetState()) {
		case ATTACK_LEFT:
			DrawEntity(tex_id, 2.0f / 3.0f, 1.0, 1.0f, 0.0, playerx, playery);
			break;
		default:
			DrawEntity(tex_id, 0.0f + (GetFrame()*1.0f / 3.0f), 1.0f, 1.0f / 3.0f + (GetFrame()*1.0f / 3.0f), 0.0f, playerx, playery);
			NextFrame(2);
			break;
	}

	int xo, xf, yo, yf;

	if (hasShoot()) {
		float bx, by;
		getBulletPos(bx, by);
		xo = 8.0f / 9.0f;	yo = 0.5f + (GetFrame()*0.5f);
		xf = xo + 1.0f / 9.0f;
		yf = yo - 0.5f;
		DrawBullet(tex_id_shoot, xo, yo, xf, yf, playerx, playery, bx, by);
	}

	
}

void cBoss::Attack() {

}