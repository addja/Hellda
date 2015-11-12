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

void cBoss::Draw(int tex_id, float playerx, float playery) {
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

	
}

void cBoss::Attack() {

}