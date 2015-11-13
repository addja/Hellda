#include "include/cEnemy.h"

cEnemy::cEnemy() {}

cEnemy::cEnemy(float posx, float posy, float step, bool th, bool drop, int z, bool * overworld) {
	original_x = posx;
	original_y = posy;
	SetPosition(posx, posy);
	thrower = th;
	SetZone(z);
	SetStepLength(step);
	setOverworld(overworld);
	dead = false;
	SetDropper(drop);
} 

cEnemy::~cEnemy() {}

void cEnemy::Reset() {
	SetPosition(original_x, original_y);
}

void cEnemy::Draw(int tex_id, float playerx, float playery) {

}

bool cEnemy::notInScreen(float playerx, float playery) {
	
	if (dead) return true;
	
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
	}

	return false;
}

void cEnemy::Attack() {

}

bool cEnemy::Logic(int *map, float playerx, float playery, int state) {
	
	if (dead) return false;

	float x, y, nextx, nexty;
	GetPosition(&x, &y);

	if (hitWithWeapon(playerx, playery, state, x, y)) {
		dead = true;
		return true;
	}

	if (state_delay > MAX_STATE_DELAY) StopState();
	else {
		float step;
		GetStepLength(&step);
		switch (GetState()) {
			case STATE_WALKUP:		nexty = y - step;
									nextx = x;
									state_delay += step;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKUP, false) && inZone(nextx, nexty)) MoveUp(map);
										else StopState();
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKUP, false)) MoveUp(map);
										else StopState();
									}
									break;
			case STATE_WALKDOWN:	nexty = y + step;
									nextx = x;
									state_delay += step;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKDOWN, false) && inZone(nextx, nexty)) MoveDown(map);
										else StopState();
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKDOWN, false)) MoveDown(map);
										else StopState();
									}
									break;
			case STATE_WALKLEFT:	nexty = y;
									nextx = x - step;
									state_delay += step;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKLEFT, false) && inZone(nextx, nexty)) MoveLeft(map);
										else StopState();
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKLEFT, false)) MoveLeft(map);
										else StopState();
									}
									break;
			case STATE_WALKRIGHT:	nexty = y;
									nextx = x + step;
									state_delay += step;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKRIGHT, false) && inZone(nextx, nexty)) MoveRight(map);
										else StopState();
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKRIGHT, false)) MoveRight(map);
										else StopState();
									}
									break;
			default:
									state_delay = 0.0f;
									// take all walkable near tiles
									std::vector<int> tiles;
									tiles.push_back(0);

									// one step ^
									nexty = y - step;
									nextx = x;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKUP, false) && inZone(nextx, nexty)) tiles.push_back(1);
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKUP, false)) tiles.push_back(1);
									}

									// one step v
									nexty = y + step;
									nextx = x;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKDOWN, false) && inZone(nextx, nexty)) tiles.push_back(2);
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKDOWN, false)) tiles.push_back(2);
									}

									// one step <
									nexty = y;
									nextx = x - step;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKLEFT, false) && inZone(nextx, nexty)) tiles.push_back(3);
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKLEFT, false)) tiles.push_back(3);
									}

									// one step >
									nexty = y;
									nextx = x + step;
									if (inOverworld()) {
										if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKRIGHT, false) && inZone(nextx, nexty)) tiles.push_back(4);
									} else {
										if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKRIGHT, false)) tiles.push_back(4);
									}

									// take randomly one and update position
									int i = rand() % tiles.size();
									std::cout << tiles[i] << std::endl;

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
	if (thrower) {
		GetPosition(&x, &y);
		if (!hasShoot()) {
			Shoot();
			switch (GetState()) {
			case STATE_WALKDOWN: setBulletDir(0); setBulletPos(x, y + 1); break;
			case STATE_WALKUP: setBulletDir(1); setBulletPos(x, y - 1); break;
			case STATE_WALKLEFT: setBulletDir(2); setBulletPos(x - 1, y); break;
			case STATE_WALKRIGHT: setBulletDir(3); setBulletPos(x + 1, y); break;
			}
			setBulletLife(60);
		}
		else {
			int i = getBulletLife();
			std::cout << i << std::endl;
			setBulletLife(i - 1);
			if (i - 1 < 1) endShoot();
			else {
				getBulletPos(x, y);
				switch (getBulletDir()) {
				case 0: setBulletPos(x, y + 1.5 * STEP_OCTOROCT); break;
				case 1: setBulletPos(x, y - 1.5 * STEP_OCTOROCT); break;
				case 2: setBulletPos(x -  1.5 * STEP_OCTOROCT, y); break;
				case 3: setBulletPos(x + 1.5 * STEP_OCTOROCT, y); break;
				}
			}
		}
	}
	return false;
}

bool cEnemy::inZone(float tx, float ty) {
	float x, y;
	GetPosition(&x, &y);
	int zonex = floor(x / (OVERWORLD_MAP_WIDTH / ZONE_WIDTH));
	int zoney = floor(y / (OVERWORLD_MAP_HEIGHT / ZONE_HEIGHT));
	int zzonex = floor(tx / (OVERWORLD_MAP_WIDTH / ZONE_WIDTH));
	int zzoney = floor(ty / (OVERWORLD_MAP_HEIGHT / ZONE_HEIGHT));
	return zonex + (OVERWORLD_MAP_HEIGHT / ZONE_HEIGHT) * zoney == zzonex + (OVERWORLD_MAP_HEIGHT / ZONE_HEIGHT) * zzoney;
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

bool cEnemy::hitWithWeapon(float playerx, float playery, int state, float x, float y) {

	cRec enemy = cRec(x, y);

	switch (state) {
		case ATTACK_DOWN:
			if (enemy.intersects(cRec(playerx, playery + OFFSET_WEAPON))) return true;
			break;
		case ATTACK_UP:
			if (enemy.intersects(cRec(playerx, playery - OFFSET_WEAPON))) return true;
			break;
		case ATTACK_LEFT:
			if (enemy.intersects(cRec(playerx - OFFSET_WEAPON, playery))) return true;
			break;
		case ATTACK_RIGHT:
			if (enemy.intersects(cRec(playerx + OFFSET_WEAPON, playery))) return true;
			break;
		case 5:
			if (enemy.intersects(cRec(playerx, playery))) return true;
			break;
	}

	return false;
}

void cEnemy::die() {
	dead = true;
}

void cEnemy::rebirth() {
	dead = false;
	state_delay = 0.0f;
}

bool cEnemy::isDead() {
	return dead;
}

bool cEnemy::isDropper() {
	return dropper;
}

bool cEnemy::isThrower() {
	return thrower;
}

void cEnemy::SetDropper(bool drop) {
	dropper = drop;
}

int cEnemy::GetHealth() {
	return health;
}

void cEnemy::SetHealth(int lifes) {
	health = lifes;
	if (lifes == 0) die();
}

void cEnemy::SetStateDelay(float delay) {
	state_delay = delay;
}

float cEnemy::GetStateDelay() {
	return state_delay;
}