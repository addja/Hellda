#pragma once

#include "cPepe.h"
#include "cOverworld.h"
#include "cDungeon.h"

class cPlayer: public cPepe {
public:
	cPlayer();
	~cPlayer();

	void Attack();
	void Logic(int *map);
	void Draw(int tex_id);
	void DrawPlayer(int tex_id, float xo, float yo, float xf, float yf);
	void DrawWeapon(int tex_id, float xo, float yo, float xf, float yf, float posx, float posy);

	void TransitionLeft(int zone);
	void TransitionRight(int zone);
	void TransitionDown(int zone);
	void TransitionUp(int zone);
};