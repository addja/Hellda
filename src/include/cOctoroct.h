#pragma once

#include "cEnemy.h"

class cOctoroct : public cEnemy {
public:
	cOctoroct();
	~cOctoroct();

	void Draw(int tex_id, float playerx, float playery);
	void Attack();
};