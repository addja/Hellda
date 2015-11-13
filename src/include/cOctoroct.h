#pragma once

#include "cEnemy.h"

class cOctoroct : public cEnemy {
public:
	cOctoroct();
	cOctoroct(float x, float y, float step, bool thrower, bool dropper, int zone, bool * overworld);
	~cOctoroct();

	void Draw(int tex_id, float playerx, float playery);
	void Attack();
};