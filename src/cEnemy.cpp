#include "include/cEnemy.h"

cEnemy::cEnemy() {}

cEnemy::cEnemy(float posx, float posy, float step, bool th, int z, bool * overworld) {
	original_x = posx;
	original_y = posy;
	SetPosition(posx, posy);
	thrower = th;
	SetZone(z);
	SetStepLength(step);
	setOverworld(overworld);
} 

cEnemy::~cEnemy() {}

void cEnemy::Reset() {
	SetPosition(original_x, original_y);
}

void cEnemy::Draw(int tex_id, float playerx, float playery) {

}

bool cEnemy::notInScreen(float playerx, float playery) {
	float x, y;
	GetPosition(&x, &y);
	if (inOverworld()) {
		// check if on the right of the left screen limit
		if (x < playerx - SCENE_WIDTH / 2 - 1) return true;		// -1 beacuse we need a little offset if to show the half of Pepe

															// check if on the left of the right screen limit 
		if (x > playerx + SCENE_WIDTH / 2) return true;

		// check if on the downside of the upper screen limit 
		if (y > playery + SCENE_HEIGHT / 2) return true;

		// check if on the upside of the bottom screen limit 
		if (y < playery - SCENE_HEIGHT / 2) return true;

		return false;
	}


}

void cEnemy::Attack() {

}

void cEnemy::Logic(int *map) {
	
	float x, y, nextx, nexty;
	GetPosition(&x, &y);

	if (state_delay > MAX_STATE_DELAY) StopState();
	else {
		float step;
		GetStepLength(&step);
		switch (GetState()) {
			case STATE_WALKUP:		nexty = y - step;
									nextx = x;
									state_delay += step;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKUP) && inZone(nextx, nexty)) MoveUp(map);
										else StopState();
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKUP)) MoveUp(map);
										else StopState();
									}
									break;
			case STATE_WALKDOWN:	nexty = y + step;
									nextx = x;
									state_delay += step;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKDOWN) && inZone(nextx, nexty)) MoveDown(map);
										else StopState();
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKDOWN)) MoveDown(map);
										else StopState();
									}
									break;
			case STATE_WALKLEFT:	nexty = y;
									nextx = x - step;
									state_delay += step;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKLEFT) && inZone(nextx, nexty)) MoveLeft(map);
										else StopState();
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKLEFT)) MoveLeft(map);
										else StopState();
									}
									break;
			case STATE_WALKRIGHT:	nexty = y;
									nextx = x + step;
									state_delay += step;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKRIGHT) && inZone(nextx, nexty)) MoveRight(map);
										else StopState();
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKRIGHT)) MoveRight(map);
										else StopState();
									}
									break;
			default:
									state_delay = step;
									// take all walkable near tiles
									std::vector<int> tiles;
									tiles.push_back(0);

									// one step ^
									nexty = y - step;
									nextx = x;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKUP) && inZone(nextx, nexty)) tiles.push_back(1);
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKUP)) tiles.push_back(1);
									}

									// one step v
									nexty = y + step;
									nextx = x;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKDOWN) && inZone(nextx, nexty)) tiles.push_back(2);
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKDOWN)) tiles.push_back(2);
									}

									// one step <
									nexty = y;
									nextx = x - step;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKLEFT) && inZone(nextx, nexty)) tiles.push_back(3);
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKLEFT)) tiles.push_back(3);
									}

									// one step >
									nexty = y;
									nextx = x + step;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKRIGHT) && inZone(nextx, nexty)) tiles.push_back(4);
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKRIGHT)) tiles.push_back(4);
									}

									// take randomly one and update position
									int i = rand() % tiles.size();

									// Discomment this shit to make them dance!
									switch (tiles[i]) {
										case 0: Stop(); break;
										case 1: MoveUp(map); break;
										case 2: MoveDown(map); break;
										case 3: MoveLeft(map); break;
										case 4: MoveRight(map); break;
									}
									break;
		}
	}
}

bool cEnemy::inZone(float tx, float ty) {
	int zone;
	GetZone(&zone);
	int zonex = (zone % ZONE_WIDTH) * ZONE_WIDTH;
	int zoney = floor(zone / ZONE_WIDTH) * ZONE_HEIGHT;
	
	if (tx <= zonex + 1 || tx >= zonex + ZONE_WIDTH - 1 || ty <= zoney + 1 || ty >= zoney + ZONE_HEIGHT - 1) return false;
	return true;
}

void cEnemy::StopState() {
	state_delay = 0.0f;
	Stop();
}

void cEnemy::SetOriginalPosition(float x, float y) {
	original_x = x;
	original_y = y;
}
void cEnemy::SetThrower(bool th) {
	thrower = th;
}