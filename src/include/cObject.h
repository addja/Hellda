#pragma once

#include "cPepe.h"
#include "cOverworld.h"
#include "cDungeon.h"
#include "cData.h"
#include <vector>

#define GOLD		0
#define KEY			1
#define BOMB		2
#define MAP			3
#define COMPASS		4
#define TRIFORCE	5
#define SWORD		6
#define BOW			7
#define BOOMERANG	8
#define CRYSTAL		9
#define SHIELD		10
#define HEART		11
#define CANDLE		12


class cObject : public cPepe {
public:
	cObject();
	cObject(float x, float y, int type, bool shown, int zone, bool * overworld);
	~cObject();

	void Reset();
	void Draw(int tex_id, float playerx, float playery);

	void SetOriginalPosition(float x, float y);
	int GetType();

	void die();
	void rebirth();
	bool isDead();
	bool isShown();
	void SetShown(bool shown);

private:
	float original_x;
	float original_y;
	int type;
	bool dead;
	bool shown;
};