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
	if (x < playerx - SCENE_WIDTH / 2 - 1) return;		// -1 beacuse we need a little offset if to show the half of Pepe

	// check if on the left of the right screen limit 
	if (x > playerx + SCENE_WIDTH / 2) return;

	// check if on the downside of the upper screen limit 
	if (y > playery + SCENE_HEIGHT / 2) return;

	// check if on the upside of the bottom screen limit 
	if (y < playery - SCENE_HEIGHT / 2) return;

	// transform player post if it is on the limits of the map
	if (playerx < SCENE_WIDTH / 2) playerx = SCENE_WIDTH / 2;
	else if (playerx > MAP_WIDTH - (SCENE_WIDTH / 2)) playerx = MAP_WIDTH - (SCENE_WIDTH / 2);
	if (playery <= (SCENE_HEIGHT - HUD_TILES) / 2) playery = (SCENE_HEIGHT - HUD_TILES) / 2;
	else if (playery >= MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2) playery = MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2;

	switch (type) {
		case OCTOROC:
			switch (GetState()) {
				default:
					DrawEntity(IMG_PLAYER, 0.0f, 0.25f, 0.25f, 0.5f, x - playerx, playery - y);
					break;
			}
			break;
	}

}

void cEnemy::Attack() {

}

void cEnemy::Logic(int *map) {
	
	float x, y, nextx, nexty;
	GetPosition(&x, &y);

	// take all walkable near tiles
	std::vector<int> tiles;

	// one step ^
	nexty = y - STEP_LENGTH;
	nextx = x;
	if (true) {
		tiles.push_back(0);
	}

	// one step v
	nexty = y + STEP_LENGTH;
	nextx = x;
	if (true) {
		tiles.push_back(1);
	}

	// one step <
	nexty = y;
	nextx = x - STEP_LENGTH;
	if (true) {
		tiles.push_back(2);
	}

	// one step >
	nexty = y;
	nextx = x + STEP_LENGTH;
	if (true) {
		tiles.push_back(3);
	}

	// take randomly one and update position
	int i = rand() % tiles.size();

	// Discomment this shit to make them dance!
	/*switch (tiles[i]) { 
		case 0: SetPosition(x, y - STEP_LENGTH); break;
		case 1: SetPosition(x, y + STEP_LENGTH); break;
		case 2: SetPosition(x - STEP_LENGTH, y); break;
		case 3: SetPosition(x + STEP_LENGTH, y); break;
	}*/
}