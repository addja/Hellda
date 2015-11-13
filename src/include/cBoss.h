#pragma once

#include "cEnemy.h"

class cBoss : public cEnemy {
public:
	cBoss();
	cBoss(float x, float y, float step, bool thrower, int zone, bool * overworld);
	~cBoss();

	void Draw(int tex_id, int tex_id_shoot, float playerx, float playery);
	void Attack();
};