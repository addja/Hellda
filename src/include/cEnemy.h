#pragma once

#include "cPepe.h"

#define OCTOROC		0

class cEnemy : public cPepe {
public:
	cEnemy();
	cEnemy(float x, float y, int type, bool thrower);
	~cEnemy();

	void Reset();
	void Draw(float playerx, float playery);
	void Attack();

private:
	float original_x;
	float original_y;
	float actual_x;
	float actual_y;
	int type;
	bool thrower;
	int throw_count;
};