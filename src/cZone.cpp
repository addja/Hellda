#include "include/cZone.h"

cZone::cZone(void) {
	oc_current = 0;
	st_current = 0;
	ke_current = 0;
	boss_area = false;
}

cZone::~cZone(void) {}

void cZone::Reset() {
	for (int i = 0; i < oc_current; ++i) {
		(*(oc_enemies[i])).Reset();
	}

	for (int i = 0; i < st_current; ++i) {
		(*(st_enemies[i])).Reset();
	}

	for (int i = 0; i < ke_current; ++i) {
		(*(ke_enemies[i])).Reset();
	}

	if (boss_area) (*boss).Reset();
}

void cZone::Draw(float playerx, float playery)  {
	for (int i = 0; i < oc_current; ++i) {
		(*(oc_enemies[i])).Draw((*Data).GetID(IMG_OCTOROC), playerx, playery);
	}

	for (int i = 0; i < st_current; ++i) {
		(*(st_enemies[i])).Draw((*Data).GetID(IMG_STALFOS), playerx, playery);
	}

	for (int i = 0; i < ke_current; ++i) {
		(*(ke_enemies[i])).Draw((*Data).GetID(IMG_KEESE), playerx, playery);
	}

	if (boss_area) (*boss).Draw((*Data).GetID(IMG_BOSS), playerx, playery);
}

void cZone::Logic(int *map, float x, float y, int state, bool bullet, float bx, float by, bool & hit) {
	for (int i = 0; i < oc_current; i++) (*(oc_enemies[i])).Logic(map, x, y, state);
	for (int i = 0; i < st_current; i++) (*(st_enemies[i])).Logic(map, x, y, state);
	for (int i = 0; i < ke_current; i++) (*(ke_enemies[i])).Logic(map, x, y, state);
	if (boss_area) (*boss).Logic(map, x, y, state);
	if (bullet) {
		for (int i = 0; i < oc_current; i++) {
			if ((*(oc_enemies[i])).Logic(map, bx, by, 5)) {  // state exclusive for the thrown weapon
				hit = true;
				return;
			}
		}
		for (int i = 0; i < st_current; i++) {
			if ( (*(st_enemies[i])).Logic(map, bx, by, 5)) {
				hit = true;
				return;
			}
		}
		for (int i = 0; i < ke_current; i++) {
			if ( (*(ke_enemies[i])).Logic(map, bx, by, 5) ) {
				hit = true;
				return;
			}
		}
		if (boss_area) hit = (*boss).Logic(map, bx, by, 5);
	}
}

void cZone::addEnemy(float x, float y, int type, bool thrower, int z, bool * overworld) {
	int i;
	switch (type) {
		case OCTOROCT: 
			oc_enemies[oc_current] = new cOctoroct(x, y, STEP_OCTOROCT, thrower, z, overworld); 
			(*(oc_enemies[oc_current])).SetWidthHeight(TILE_SIZE, TILE_SIZE);

			i = rand() % 4;
			switch (i) {
				case 0: (*(oc_enemies[oc_current])).SetState(STATE_LOOKDOWN); break;
				case 1: (*(oc_enemies[oc_current])).SetState(STATE_LOOKUP); break;
				case 2: (*(oc_enemies[oc_current])).SetState(STATE_LOOKLEFT); break;
				case 3: (*(oc_enemies[oc_current])).SetState(STATE_LOOKRIGHT); break;
			}
			++oc_current;
			break;
		case STALFOS: 
			st_enemies[st_current] = new cStalfos(x, y, STEP_STALFOS, thrower, z, overworld); 
			(*(st_enemies[st_current])).SetWidthHeight(TILE_SIZE, TILE_SIZE);
			(*(st_enemies[st_current])).SetState(STATE_LOOKDOWN);
			++st_current;
			break;
		case KEESE: 
			ke_enemies[ke_current] = new cKeese(x, y, STEP_KEESE, thrower, z, overworld); 
			(*(ke_enemies[ke_current])).SetWidthHeight(TILE_SIZE, TILE_SIZE);
			(*(ke_enemies[ke_current])).SetState(STATE_LOOKDOWN);
			++ke_current;
			break;
		case BOSS:
			boss = new cBoss(x, y, STEP_BOSS, thrower, z, overworld);
			(*boss).SetWidthHeight(TILE_SIZE, TILE_SIZE);
			(*boss).SetState(STATE_LOOKDOWN);
			boss_area = true;
			break;
	}
}

//void addObject();

cOctoroct* cZone::GetOctoroc(int id) {
	return oc_enemies[id];		// todo whatch this out
}

cStalfos* cZone::GetStalfos(int id) {
	return st_enemies[id];		// todo whatch this out
}

cKeese* cZone::GetKeese(int id) {
	return ke_enemies[id];		// todo whatch this out
}

cBoss* cZone::GetBoss() {
	return boss;			// todo whatch this out
}

int cZone::numOctorocs() {
	return oc_current;
}

int cZone::numStalfos() {
	return st_current;
}

int cZone::numKeeses() {
	return ke_current;
}

bool cZone::isBossArea() {
	return boss_area;
}

void cZone::SetData(cData *data) {
	Data = data;
}

void cZone::SetOverworld(bool over) {
	overworld = over;
}