#include "include/cEnemy.h"

cEnemy::cEnemy() {}

cEnemy::cEnemy(float posx, float posy, int ty, bool th) : cPepe() {
	original_x = posx;
	original_y = posy;
	SetPosition(posx, posy);
	type = ty;
	thrower = th;
} 

cEnemy::~cEnemy() {}

void cEnemy::Reset() {
	SetPosition(original_x, original_y);
}

void cEnemy::Draw(float playerx, float playery) {
	
	float x,y;
	GetPosition(&x, &y);
	// check if on the right of the left screen limit
	if (x < playerx - SCENE_WIDTH / 2) return;

	// check if on the left of the right screen limit 
	if (x > playerx + SCENE_WIDTH / 2) return;

	// check if on the downside of the upper screen limit 
	if (y > playery + SCENE_HEIGHT / 2) return;

	// check if on the upside of the bottom screen limit 
	if (y < playery - SCENE_HEIGHT / 2) return;

	switch (type) {
		case OCTOROC:
			switch (GetState()) {
				default:
					DrawEntity(IMG_PLAYER, 0.0f, 0.25f, 0.25f, 0.5f, x - playerx, y - playery);
					break;
			}
			break;
	}


	// DrawRect(tex_id, xo, yo, xf, yf);
}

void cEnemy::Attack() {

}