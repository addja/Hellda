#include "include/cZone.h"

cZone::cZone(void) {
	oc_current = 0;
	st_current = 0;
	ke_current = 0;
	obj_current = 0;
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

	for (int i = 0; i < obj_current; ++i) {
		(*(objects[i])).Reset();
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

	for (int i = 0; i < obj_current; ++i) {
		(*(objects[i])).Draw((*Data).GetID(IMG_OBJECTS), playerx, playery);
	}

	if (boss_area) (*boss).Draw((*Data).GetID(IMG_BOSS), (*Data).GetID(IMG_OCTOROC), playerx, playery);
}

void cZone::Logic(int *map, float x, float y, int state, bool bullet, float bx, float by, bool & hit) {
	bool hi;
	for (int i = 0; i < oc_current; i++) {
		if (!(*(oc_enemies[i])).isDead()) {
			hi = (*(oc_enemies[i])).Logic(map, x, y, state);
			if ((*(oc_enemies[i])).isDead() && (*(oc_enemies[i])).isDropper()) {
				float x, y;
				(*(oc_enemies[i])).GetPosition(&x, &y);
				int zone;
				(*(oc_enemies[i])).GetZone(&zone);
				int i = rand() % 2;
				if (i == 1) addObject(x, y, CRYSTAL, true, zone, &overworld);
				else addObject(x, y, GOLD, true, zone, &overworld);
			}
		}
	}
	for (int i = 0; i < st_current; i++) {
		if (!(*(st_enemies[i])).isDead()) {
			hi = (*(st_enemies[i])).Logic(map, x, y, state);
			if ((*(st_enemies[i])).isDead() && (*(st_enemies[i])).isDropper()) {
				float x, y;
				(*(st_enemies[i])).GetPosition(&x, &y);
				int zone;
				(*(st_enemies[i])).GetZone(&zone);
				int i = rand() % 2;
				if (i == 1) addObject(x, y, CRYSTAL, true, zone, &overworld);
				else addObject(x, y, GOLD, true, zone, &overworld);
			}
		}
	}
	for (int i = 0; i < ke_current; i++) {
		if (!(*(ke_enemies[i])).isDead()) {
			hi = (*(ke_enemies[i])).Logic(map, x, y, state);
			if ((*(ke_enemies[i])).isDead() && (*(ke_enemies[i])).isDropper()) {
				float x, y;
				(*(ke_enemies[i])).GetPosition(&x, &y);
				int zone;
				(*(ke_enemies[i])).GetZone(&zone);
				int i = rand() % 2;
				if (i == 1) addObject(x, y, CRYSTAL, true, zone, &overworld);
				else addObject(x, y, GOLD, true, zone, &overworld);
			}
		}
	}
	if (boss_area && !(*boss).isDead()) {
		hi = (*boss).Logic(map, x, y, state);
		if ((*boss).isDead() && (*boss).isDropper()) {
			float x, y;
			(*boss).GetPosition(&x, &y);
			int zone;
			(*boss).GetZone(&zone);
			addObject(x, y, CRYSTAL, true, zone, &overworld);
		}
	}

	if (bullet) {
		for (int i = 0; i < oc_current; i++) {
			if (!(*(oc_enemies[i])).isDead()) {
				hi = (*(oc_enemies[i])).Logic(map, x, y, 5);
				if (hi) {  // state exclusive for the thrown weapon
					hit = true;
					return;
				}
			}
		}
		for (int i = 0; i < st_current; i++) {
			if (!(*(st_enemies[i])).isDead()) {
				hi = (*(st_enemies[i])).Logic(map, x, y, 5);
				if (hi) {
					hit = true;
					return;
				}
			}
		}
		for (int i = 0; i < ke_current; i++) {
			if (!(*(ke_enemies[i])).isDead()) {
				hi = (*(st_enemies[i])).Logic(map, x, y, 5);
				if (hi) {
					hit = true;
					return;
				}
			}
		}
		if (boss_area && !(*boss).isDead()) {
			hi = (*boss).Logic(map, x, y, 5);
			if (hi) {
				hit = true;
				return;
			}
		}
	}

	if (allEnemiesDead()) {
		for (int i = 0; i < obj_current; ++i) {
			if (!(*(objects[i])).isShown()) (*(objects[i])).SetShown(true);
		}
	}
}

void cZone::addEnemy(float x, float y, int type, bool thrower, bool dropper, int zon, bool * overworld) {
	int i;
	switch (type) {
		case OCTOROCT: 
			oc_enemies[oc_current] = new cOctoroct(x, y, STEP_OCTOROCT, thrower, dropper, zon, overworld);
			(*(oc_enemies[oc_current])).SetWidthHeight(TILE_SIZE, TILE_SIZE);
			i = rand() % 4;
			switch (i) {
				case 0: (*(oc_enemies[oc_current])).SetState(STATE_LOOKDOWN); break;
				case 1: (*(oc_enemies[oc_current])).SetState(STATE_LOOKUP); break;
				case 2: (*(oc_enemies[oc_current])).SetState(STATE_LOOKLEFT); break;
				case 3: (*(oc_enemies[oc_current])).SetState(STATE_LOOKRIGHT); break;
			}
			(*(oc_enemies[oc_current])).SetState(STATE_LOOKDOWN);
			++oc_current;
			break;
		case STALFOS: 
			st_enemies[st_current] = new cStalfos(x, y, STEP_STALFOS, thrower, dropper, zon, overworld);
			(*(st_enemies[st_current])).SetWidthHeight(TILE_SIZE, TILE_SIZE);
			(*(st_enemies[st_current])).SetState(STATE_LOOKDOWN);
			++st_current;
			break;
		case KEESE: 
			ke_enemies[ke_current] = new cKeese(x, y, STEP_KEESE, thrower, dropper, zon, overworld);
			(*(ke_enemies[ke_current])).SetWidthHeight(TILE_SIZE, TILE_SIZE);
			(*(ke_enemies[ke_current])).SetState(STATE_LOOKDOWN);
			++ke_current;
			break;
		case BOSS:
			boss = new cBoss(x, y, STEP_BOSS, thrower, dropper, zon, overworld);
			(*boss).SetWidthHeight(2*TILE_SIZE, 2*TILE_SIZE);
			(*boss).SetState(STATE_LOOKLEFT);
			boss_area = true;
			break;
	}
}

void cZone::addObject(float x, float y, int type, bool shown, int z, bool * overworld) {
	objects[obj_current] = new cObject(x, y, type, shown, z, overworld);
	(*(objects[obj_current])).SetWidthHeight(TILE_SIZE, TILE_SIZE);
	(*(objects[obj_current])).SetState(STATE_LOOKDOWN);
	++obj_current;
}

cOctoroct* cZone::GetOctoroc(int id) {
	return oc_enemies[id];		// todo whatch this out
}

cStalfos* cZone::GetStalfos(int id) {
	return st_enemies[id];		// todo whatch this out
}

cKeese* cZone::GetKeese(int id) {
	return ke_enemies[id];		// todo whatch this out
}

cObject* cZone::GetObj(int id) {
	return objects[id];		// todo whatch this out
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

int cZone::numObjects() {
	return obj_current;
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

bool cZone::allEnemiesDead() {
	for (int i = 0; i < oc_current; i++) if (!(*(oc_enemies[i])).isDead()) return false;
	for (int i = 0; i < st_current; i++) if (!(*(st_enemies[i])).isDead()) return false;
	for (int i = 0; i < ke_current; i++) if (!(*(ke_enemies[i])).isDead()) return false;
	return true;
}