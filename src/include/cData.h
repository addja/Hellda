#pragma once

#include "cTexture.h"
#include "cSound.h"
#include "cMusic.h"
#include "Globals.h"

// Image array size
#define NUM_IMG		6

// Image identifiers
#define IMG_OVERWORLD			0
#define IMG_PLAYER				1
#define IMG_ENEMIES				2
#define IMG_DUNGEON				3
#define IMG_TEXT				4
#define IMG_ITEMS				5
#define IMG_DUNGEON_MINIMAP		6

// Sounds array size
#define NUM_SOUNDS				1

// Sounds identifiers
#define SOUND_SWORD				0

// Musics array size
#define NUM_MUSICS				1

// Musics identifiers
#define MUSIC_OVERWORLD			0

class cData {
public:
	cData(void);
	~cData(void);

	int  GetID(int img);
	void GetSize(int img,int *w,int *h);
	bool LoadImage(int img, char *filename, int type = GL_RGBA);
	bool LoadSound(int sound, const std::string & filename);
	bool LoadMusic(int music, const std::string & filename);

	void playSound(int sound);
	void playMusic(int music);
	void stopMusic(int music);

private:
	cTexture texture[NUM_IMG];
	cSound sound[NUM_SOUNDS];
	cMusic music[NUM_MUSICS];
};
