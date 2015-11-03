#include "include/cEnemy.h"

cEnemy::cEnemy() {}

cEnemy::cEnemy(float x, float y, int ty, bool th) : cPepe() {
	original_x = x;
	original_y = y;
	actual_x = x;
	actual_y = y;
	type = ty;
	thrower = th;
} 

cEnemy::~cEnemy() {}

void cEnemy::Reset() {
	actual_x = original_x;
	actual_y = original_y;
}

void cEnemy::Draw(float playerx, float playery) {	
	// DrawRect(tex_id, xo, yo, xf, yf);
}

void cEnemy::Attack() {

}