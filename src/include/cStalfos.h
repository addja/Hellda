#pragma once

#include "cEnemy.h"

class cStalfos : public cEnemy {
public:
	cStalfos();
	cStalfos(float x, float y, float step, bool thrower, bool drop, int zone, bool * overworld);
	~cStalfos();

	void Draw(int tex_id, float playerx, float playery);
	void Attack();
};