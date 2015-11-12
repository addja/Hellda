#pragma once

#include "cEnemy.h"

class cBoss : public cEnemy {
public:
	cBoss();
	cBoss(float x, float y, float step, bool thrower, int zone, bool * overworld);
	~cBoss();

	void Draw(int tex_id, float playerx, float playery);
	void Attack();
};