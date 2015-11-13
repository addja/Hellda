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
	cPlayer(bool * overworld, bool * transition, bool * opening, bool * win, cData * dat);
	~cPlayer();

	void Attack();
	void Logic(int *map);
	void Draw(int tex_id);
	void DrawPlayer(int tex_id, float xo, float yo, float xf, float yf);
	void DrawWeapon(int tex_id, float xo, float yo, float xf, float yf, float posx, float posy);
	bool checkIntersections(cZone * zone);

	void Transition();

	void GetOffset(float * offset);
	void SetOffset(float offset);

	float health();
	int GetGold();
	int GetBombs();
	bool hasMinimap();
	bool hasCompass();
	bool hasBoomerang();
	bool isDead();
	void StopState();

	bool swipeAgain();
	void swipe();
	void endSwipe();

	void throwSword();

private: 
	float lives;
	int gold;
	int bombs;
	bool * transition;
	float offset;
	float state_delay;
	int hit_dirx; // 1 -> left | -1 -> right
	int hit_diry; // 1 -> up | -1 -> down
	bool sword_swipe;
	bool minimap;
	bool compass;
	bool boomerang;
	bool * win;
	cData * data;
};
