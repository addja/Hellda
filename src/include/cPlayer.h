#pragma once

#include "cPepe.h"
#include "cScene.h"

class cPlayer: public cPepe {
public:
	cPlayer();
	~cPlayer();

	void Attack();
	void Draw(int tex_id);
	void DrawRect(int tex_id, float xo, float yo, float xf, float yf);
	void DrawWeapon(int tex_id, float xo, float yo, float xf, float yf, float posx, float posy);
};