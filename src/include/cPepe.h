#pragma once

#include "cTexture.h"
#include "Globals.h"

#define FRAME_DELAY		8
#define STEP_LENGTH		0.1

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

class cRect
{
public:
	int left,top,right,bottom;
};

class cPepe
{
public:
	cPepe(void);
	cPepe(int x,int y,int w,int h);
	~cPepe(void);

	void SetPosition(float x, float y);
	void GetPosition(float *x, float *y);
	void SetTile(int tx,int ty);
	void GetTile(int *tx,int *ty);
	void SetWidthHeight(int w,int h);
	void GetWidthHeight(int *w,int *h);

	bool checkCorrectMovement(float & x, float & y, int *map, int movement);
	bool walkable(int tile);
	bool diagonallyWalkableUpRight(int tile);
	bool diagonallyWalkableUpLeft(int tile);
	bool diagonallyWalkableDownRight(int tile);
	bool diagonallyWalkableDownLeft(int tile);

	void DrawRect(int tex_id, float xo, float yo, float xf, float yf);
	void DrawWeapon(int tex_id, float xo, float yo, float xf, float yf, float posx, float posy);

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
	
private:
	float x,y;
	int w,h;
	int state;

	int seq,delay;
};
