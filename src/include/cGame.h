#pragma once

#include "cScene.h"
#include "cPlayer.h"
#include "cData.h"

class cGame {
public:
	cGame(void);
	virtual ~cGame(void);

	bool Init();
	bool Loop();
	void Finalize();

	// Input
	void ReadKeyboard(unsigned char key, int x, int y, bool press);
	void ReadMouse(int button, int state, int x, int y);			// TODO: we don't use the mouse
	// Process
	bool Process();
	// Output
	void Render();

private:
	unsigned char keys[256];
	cScene Scene;
	cPlayer Player;
	cData Data;
};
