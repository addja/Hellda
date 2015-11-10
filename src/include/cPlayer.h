#pragma once

#include "cPepe.h"
#include "cOverworld.h"
#include "cDungeon.h"
#include "cZone.h"

#define LINK_LIVES	3
#define MAX_STATE_DELAY_LINK 4
#define TRANSITION_SPEED 125

class cPlayer: public cPepe {
public:
	cPlayer();
	cPlayer(bool * overworld, bool * transition);
	~cPlayer();

	void Attack();
	void Logic(int *map);
	void Draw(int tex_id);
	void DrawPlayer(int tex_id, float xo, float yo, float xf, float yf);
	void DrawWeapon(int tex_id, float xo, float yo, float xf, float yf, float posx, float posy);
	bool checkIntersections(cZone zone);

	void Transition();

	void GetOffset(float * offset);

	float health();
	bool isDead();
	void StopState();

private: 
	float lives;
	bool * transition;
	float offset;
	float state_delay;
	int hit_dirx; // 1 -> left | -1 -> right
	int hit_diry; // 1 -> up | -1 -> down
};
