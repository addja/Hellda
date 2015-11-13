#include "include/cBoss.h"

cBoss::cBoss() {}

cBoss::cBoss(float posx, float posy, float step, bool th, bool drop, int z, bool * overworld) {
	SetOriginalPosition(posx, posy);
	SetPosition(posx, posy);
	SetThrower(th);
	SetZone(z);
	SetStepLength(step);
	setOverworld(overworld);
	SetDropper(drop);
	rebirth();
	SetHealth(5);
}


cBoss::~cBoss() {}

bool cBoss::Logic(int *map, float playerx, float playery, int state) {

	if (isDead()) return false;

	float x, y, nextx, nexty;
	GetPosition(&x, &y);

	if (hitWithWeapon(playerx, playery, state, x, y)) {
		die();
		return true;
	}

	if (GetStateDelay() > MAX_STATE_DELAY_BOSS) StopState();
	else {
		float step;
		float delay;
		delay = GetStateDelay();
		GetStepLength(&step);
		switch (GetState()) {
		case STATE_WALKLEFT:	nexty = y;
			nextx = x - step;
			SetStateDelay(delay + step);
			if (inOverworld()) {
				if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKLEFT, false) && inZone(nextx, nexty)) MoveLeft(map);
				else StopState();
			}
			else {
				if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKLEFT, false)) MoveLeft(map);
				else StopState();
			}
			break;
		case STATE_WALKRIGHT:	nexty = y;
			nextx = x + step;
			SetStateDelay(delay + step);
			if (inOverworld()) {
				if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKRIGHT, false) && inZone(nextx, nexty)) MoveRight(map);
				else StopState();
			}
			else {
				if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKRIGHT, false)) MoveRight(map);
				else StopState();
			}
			break;
		default:
			SetStateDelay(step);
			// take all walkable near tiles
			std::vector<int> tiles;
			tiles.push_back(0);

			// one step <
			nexty = y;
			nextx = x - step;
			if (inOverworld()) {
				if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKLEFT, false) && inZone(nextx, nexty)) tiles.push_back(3);
			}
			else {
				if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKLEFT, false)) tiles.push_back(3);
			}

			// one step >
			nexty = y;
			nextx = x + step;
			if (inOverworld()) {
				if (checkCorrectMovementOverworld(nextx, nexty, map, STATE_WALKRIGHT, false) && inZone(nextx, nexty)) tiles.push_back(4);
			}
			else {
				if (checkCorrectMovementDungeon(nextx, nexty, map, STATE_WALKRIGHT, false)) tiles.push_back(4);
			}

			// take randomly one and update position
			int i = rand() % tiles.size();

			// Discomment this shit to make them dance!
			switch (tiles[i]) {
			case 0: Stop(); break;
			case 3: MoveLeft(map); break;
			case 4: MoveRight(map); break;
			}
			break;
		}
	}
	if (isThrower()) {
		GetPosition(&x, &y);
		if (!hasShoot()) {
			Shoot();
			setBulletDir(2); 
			setBulletPos(x - 1.5, y);
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
				case 2: setBulletPos(x - 1.5 * STEP_OCTOROCT, y); break;
				case 3: setBulletPos(x + 1.5 * STEP_OCTOROCT, y); break;
				}
			}
		}
	}
	return false;
}

void cBoss::Draw(int tex_id, int tex_id_shoot, float playerx, float playery) {
	if (notInScreen(playerx, playery)) return;

	switch (GetState()) {
		case ATTACK_LEFT:
			DrawEntity(tex_id, 4.0f / 7.0f, 1.0, 6.0f / 7.0f, 0.0, playerx, playery);
			break;
		default:
			DrawEntity(tex_id, 0.0f + (GetFrame()*2.0f / 7.0f), 1.0f, 2.0f / 7.0f + (GetFrame()*2.0f / 7.0f), 0.0f, playerx, playery);
			NextFrame(2);
			break;
	}

	int xo, xf, yo, yf;

	if (hasShoot()) {
		float bx, by;
		getBulletPos(bx, by);
		DrawBullet(tex_id, 6.0f / 7.0f, 0.5f + (GetFrame()*0.5f), 1.0f, 0.0f + (GetFrame()*0.5f), playerx, playery, bx, by);
	}

	
}

void cBoss::Attack() {

}