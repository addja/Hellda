#pragma once

#include "cPepe.h"
#include "cOverworld.h"
#include "cDungeon.h"
#include "cData.h"
#include <vector>

#define MAX_STATE_DELAY	4.0f

#define OCTOROCT		0
#define STEP_OCTOROCT	0.05f

class cEnemy : public cPepe {
public:
	cEnemy();
	cEnemy(float x, float y, float step, bool thrower, int zone, bool * overworld);
	~cEnemy();

	void Reset();
	void Draw(int tex_id, float playerx, float playery);
	void Attack();
	void Logic(int *map, float x, float y, int state);
	bool inZone(float x, float y);
	void StopState();
	bool notInScreen(float playerx, float playery);

	void SetOriginalPosition(float x, float y);
	void SetThrower(bool thrower);

	bool hitWithWeapon(float playerx, float playery, int state, float x, float y);
	void die();
	void rebirth();

private:
	float original_x;
	float original_y;
	bool thrower;
	int throw_count;
	float state_delay;
	bool dead;
};