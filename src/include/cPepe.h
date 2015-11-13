#pragma once

#include "cTexture.h"
#include "Globals.h"
#include "cRec.h"

#define FRAME_DELAY			8
#define STEP_LENGTH_LINK	0.1

#define STATE_LOOKLEFT		0
#define STATE_LOOKRIGHT		1
#define STATE_LOOKUP		2
#define STATE_LOOKDOWN		3
#define STATE_WALKLEFT		4
#define STATE_WALKRIGHT		5
#define STATE_WALKUP		6
#define STATE_WALKDOWN		7
#define ATTACK_UP			8
#define ATTACK_DOWN			9
#define ATTACK_LEFT			10
#define ATTACK_RIGHT		11
#define STATE_LOOKBOOM		12
#define STATE_BOOM			13
#define STATE_WIN			14

#define OFFSET_WEAPON		0.8f

class cPepe
{
public:
	cPepe();
	cPepe(int x,int y,int w,int h, bool * overworld);
	~cPepe(void);

	void SetPosition(float x, float y);
	void GetPosition(float *x, float *y);
	void SetTile(int tx,int ty);
	void GetTile(int *tx,int *ty);
	void SetWidthHeight(int w,int h);
	void GetWidthHeight(int *w,int *h);
	void SetStepLength(float step);
	void GetStepLength(float *step);
	void SetZone(int zone);
	void GetZone(int *zone);

	bool checkCorrectMovementOverworld(float x, float y, int *map, int movement, bool player);
	bool checkCorrectMovementDungeon(float x, float y, int *map, int movement, bool player);
	bool walkable(int tile);
	bool walkableDungeon(int tile);
	bool diagonallyWalkableUpRight(int tile);
	bool diagonallyWalkableUpLeft(int tile);
	bool diagonallyWalkableDownRight(int tile);
	bool diagonallyWalkableDownLeft(int tile);
	bool overworldTransitions(int tile);
	bool dungeonUpTransitions(int tile);
	bool dungeonDownTransitions(int tile);
	bool dungeonRightTransitions(int tile);
	bool dungeonLeftTransitions(int tile);
	bool lockDoorsDungeon(int tile);

	bool overworldDoors(int tile);
	bool dungeonDoors(int tile);

	void DrawEntity(int tex_id, float xo, float yo, float xf, float yf, float playerx, float playery);
	void DrawBullet(int tex_id, float xo, float yo, float xf, float yf, float playerx, float playery, float bx, float by);

	void MoveUp(int *map);
	void MoveDown(int *map);
	void MoveRight(int *map);
	void MoveLeft(int *map);
	void Stop();
	void Logic(int *map);

	int  GetState();
	void SetState(int s);
	void SetSeqNDelay(int s, int d);

	void NextFrame(int max);
	int  GetFrame();

	bool inOverworld();
	void setOverworld(bool *overworld);
	void setOpening(bool *opening);
	void changeOverworld();

	void SetKnocked(bool knocked);
	void GetKnocked(bool * knocked);

	void SetKeys(int keys);
	int GetKeys();

	void swapDoors(int* map, int pos);

	bool hasShoot();
	void Shoot();
	void endShoot();
	void getBulletPos(float & bulletx, float & bullety);
	void setBulletPos(float bulletx, float bullety);

	void setBulletDir(int i);
	int getBulletDir();
	int getBulletLife();
	void setBulletLife(int i);

	// To remove later
	bool testingDungeon(int tile);
	
private:
	float x,y;
	int w,h;
	int state;
	float step_length;
	bool * overworld;
	bool * opening;
	bool knocked;
	int zone;
	int keys;

	int seq, delay;

	int bulletLife;
	bool bullet;
	float bulletx, bullety;
	int bulletDir;	// 0 -> down | 1 -> up | 2 -> left | 3 -> right
};
