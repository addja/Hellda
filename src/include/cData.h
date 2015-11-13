#pragma once

#include "cTexture.h"
#include "cSound.h"
#include "cMusic.h"
#include "Globals.h"

// Image array size
#define NUM_IMG					15

// Image identifiers
#define IMG_OVERWORLD			0
#define IMG_PLAYER				1
#define IMG_OCTOROC				2
#define IMG_DUNGEON				3
#define IMG_TEXT				4
#define IMG_ITEMS_HUD			5
#define IMG_DUNGEON_MINIMAP		6
#define IMG_INTRO				7
#define IMG_WATERFALL			8
#define IMG_STALFOS				9
#define IMG_KEESE				10
#define IMG_BOSS				11
#define IMG_OBJECTS				12
#define IMG_AFRO				13
#define IMG_BLONDE				14

// Sounds array size
#define NUM_SOUNDS				7

// Sounds identifiers
#define SOUND_SWORD				0
#define SOUND_ITEM				1
#define SOUND_KEY				2
#define SOUND_COIN				3
#define SOUND_DOOR				4
#define SOUND_HIT				5
#define SOUND_WIN				6

// Musics array size
#define NUM_MUSICS				5

// Musics identifiers
#define MUSIC_OVERWORLD			0
#define MUSIC_DUNGEON			1
#define MUSIC_GAMEOVER			2
#define MUSIC_WIN				3
#define MUSIC_LOVE				4

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
