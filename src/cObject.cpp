#include "include/cObject.h"

cObject::cObject() {}

cObject::cObject(float posx, float posy, int ty, bool sh, int z, bool * overworld) {
	original_x = posx;
	original_y = posy;
	SetPosition(posx, posy);
	SetZone(z);
	setOverworld(overworld);
	shown = sh;
	dead = false;
	type = ty;
}

cObject::~cObject() {}

void cObject::Reset() {
	SetPosition(original_x, original_y);
}

void cObject::Draw(int tex_id, float playerx, float playery) {
	if (isDead() || !isShown()) return;

	float xo, yo, xf, yf;

	switch (type) {
		case GOLD:		xo = 5.0f / 7.0f;	yo = 0.5f;
						break;
		case CRYSTAL:	xo = 5.0f / 7.0f;	yo = 1.0f;
						break;
		case KEY:		xo = 3.0f / 7.0f;	yo = 0.5f;
						break;
		case BOMB:		xo = 2.0f / 7.0f; yo = 1.0f;
						break;
		case MAP:		xo = 4.0f / 7.0f;	yo = 1.0f;
						break;
		case COMPASS:	xo = 4.0f / 7.0f; yo = 0.5f;
						break;
		case TRIFORCE:  xo = 6.0f / 7.0f; yo = 0.5f + (GetFrame()*0.5f);
						NextFrame(2);
						break;
		case SWORD:		xo = 2.0f / 7.0f; yo = 0.5f;
						break;
		case BOW:		xo = 0.0f; yo = 1.0f;
						break;
		case BOOMERANG:	xo = 1.0f / 7.0f; yo = 1.0f;
						break;
		case SHIELD:	xo = 0.0f;	yo = 0.5f;
						break;
		case HEART:		xo = 3.0f / 7.0f;	yo = 1.0f;
						break;
		case CANDLE:	xo = 2.0f / 7.0f;	yo = 0.5f;
						break;
	}

	xf = xo + 1.0f / 7.0f;
	yf = yo - 0.5f;

	DrawEntity(tex_id, xo, yo, xf, yf, playerx, playery);
}

void cObject::SetOriginalPosition(float x, float y) {
	original_x = x;
	original_y = y;
}

int cObject::GetType() {
	return type;
}

void cObject::die() {
	dead = true;
}

void cObject::rebirth() {
	dead = false;
}

bool cObject::isDead() {
	return dead;
}

bool cObject::isShown() {
	return shown;
}

void cObject::SetShown(bool s) {
	shown = s;
}