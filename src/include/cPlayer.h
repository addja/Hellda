#pragma once

#include "cPepe.h"

#define PLAYER_START_CX		119
#define PLAYER_START_CY		83

class cPlayer: public cPepe {
public:
	cPlayer();
	~cPlayer();

	void Draw(int tex_id);
};