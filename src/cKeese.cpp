#include "include/cKeese.h"

cKeese::cKeese() {}

cKeese::cKeese(float posx, float posy, float step, bool th, bool drop, int z, bool * overworld) {
	SetOriginalPosition(posx, posy);
	SetPosition(posx, posy);
	SetThrower(th);
	SetZone(z);
	SetStepLength(step);
	setOverworld(overworld);
	SetDropper(drop);
	rebirth();
	SetHealth(1);
}


cKeese::~cKeese() {}

void cKeese::Draw(int tex_id, float playerx, float playery) {
	if (notInScreen(playerx, playery)) return;

	if (inOverworld()) DrawEntity(tex_id, 0.5f + (GetFrame()*0.25f), 1.0f, 0.75f + (GetFrame()*0.25f), 0.0f, playerx, playery);
	else DrawEntity(tex_id, 0.0f + (GetFrame()*0.25f), 1.0f, 0.25f + (GetFrame()*0.25f), 0.0f, playerx, playery);

	NextFrame(2);
}

void cKeese::Attack() {

}