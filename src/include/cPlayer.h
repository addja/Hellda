#pragma once

#include "cPepe.h"
#include "cOverworld.h"
#include "cDungeon.h"
#include "cZone.h"

#define LINK_LIVES	3

class cPlayer: public cPepe {
public:
	cPlayer();
	cPlayer(bool * overworld);
	~cPlayer();

	void Attack();
	void Logic(int *map);
	void Draw(int tex_id);
	void DrawPlayer(int tex_id, float xo, float yo, float xf, float yf);
	void DrawWeapon(int tex_id, float xo, float yo, float xf, float yf, float posx, float posy);
	bool checkIntersections(cZone zone);

	void TransitionLeft(int zone);
	void TransitionRight(int zone);
	void TransitionDown(int zone);
	void TransitionUp(int zone);

	float health();
	bool isDead();

private: 
	float lives;
};
