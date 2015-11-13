#include "include/cStalfos.h"

cStalfos::cStalfos() {}

cStalfos::cStalfos(float posx, float posy, float step, bool th, bool drop, int z, bool * overworld) {
	SetOriginalPosition(posx, posy);
	SetPosition(posx, posy);
	SetThrower(th);
	SetZone(z);
	SetStepLength(step);
	setOverworld(overworld);
	SetDropper(drop);
	rebirth();
	SetHealth(2);
}


cStalfos::~cStalfos() {}

void cStalfos::Draw(int tex_id, float playerx, float playery) {
	if (notInScreen(playerx, playery)) return;

	DrawEntity(tex_id, 0.0f, 0.5f + (GetFrame()*0.5f), 1.0, (GetFrame()*0.5f), playerx, playery);

	NextFrame(2);
}

void cStalfos::Attack() {

}