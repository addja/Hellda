#pragma once

#include "cPepe.h"
#include "cOverworld.h"
#include "cDungeon.h"
#include "cData.h"
#include <vector>

#define OCTOROC				0
#define STEP_LENGTH_MONSTER	0.05f
#define MAX_STATE_DELAY		4.0f

class cEnemy : public cPepe {
public:
	cEnemy();
	cEnemy(float x, float y, int type, bool thrower, int zone, bool * overworld);
	~cEnemy();

	void Reset();
	void Draw(int tex_id, float playerx, float playery);
	void Attack();
	void Logic(int *map);
	bool inZone(float x, float y);
	void StopState();

private:
	float original_x;
	float original_y;
	int type;
	bool thrower;
	int throw_count;
	float state_delay;
};