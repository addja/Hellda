#pragma once

#include "cPepe.h"

class cPlayer: public cPepe {
public:
	cPlayer();
	~cPlayer();

	void Attack();
	void Draw(int tex_id);
};