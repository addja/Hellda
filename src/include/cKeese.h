#pragma once

#include "cEnemy.h"

class cKeese : public cEnemy {
public:
	cKeese();
	cKeese(float x, float y, float step, bool thrower, bool dropper, int zone, bool * overworld);
	~cKeese();

	void Draw(int tex_id, float playerx, float playery);
	void Attack();
};