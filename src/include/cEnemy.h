#pragma once

#include "cPepe.h"
#include "cScene.h"
#include "cData.h"
#include <vector>

#define OCTOROC		0

class cEnemy : public cPepe {
public:
	cEnemy();
	cEnemy(float x, float y, int type, bool thrower);
	~cEnemy();

	void Reset();
	void Draw(float playerx, float playery);
	void Attack();
	void Logic(int *map);

private:
	float original_x;
	float original_y;
	int type;
	bool thrower;
	int throw_count;
};