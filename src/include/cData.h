#pragma once

#include "cTexture.h"
#include "Globals.h"

//Image array size
#define NUM_IMG		6

//Image identifiers
#define IMG_OVERWORLD	0
#define IMG_PLAYER		1
#define IMG_ENEMIES		2
#define IMG_DUNGEON		3
#define IMG_TEXT		4
#define IMG_ITEMS		5

class cData {
public:
	cData(void);
	~cData(void);

	int  GetID(int img);
	void GetSize(int img,int *w,int *h);
	bool LoadImage(int img,char *filename,int type = GL_RGBA);

private:
	cTexture texture[NUM_IMG];
};
