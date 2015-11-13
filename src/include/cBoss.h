#pragma once

#include "cEnemy.h"

class cBoss : public cEnemy {
public:
	cBoss();
	cBoss(float x, float y, float step, bool thrower, bool dropper, int zone, bool * overworld);
	~cBoss();

	bool Logic(int *map, float x, float y, int state);
	void Draw(int tex_id, int tex_id_shoot, float playerx, float playery);
	void Attack();
};