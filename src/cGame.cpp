#include "include/cGame.h"
#include "include/Globals.h"

cGame::cGame(void) {}

cGame::~cGame(void) {}

bool cGame::Init() {
	bool res=true;

	// Graphics initialization
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,GAME_WIDTH,0,GAME_HEIGHT,0,1);
	glMatrixMode(GL_MODELVIEW);
	
	glAlphaFunc(GL_GREATER, 0.05f);
	glEnable(GL_ALPHA_TEST);

	// Scene initialization
	 res = Data.LoadImage(IMG_OVERLORD,"zelda-tiles-compressed.png",GL_RGB);
	 if(!res) return false;
	 res = Scene.LoadLevel(1);
	 if(!res) return false;

	//Player initialization
	 res = Data.LoadImage(IMG_PLAYER,"bub.png",GL_RGBA);
	 if(!res) return false;
	 Player.SetWidthHeight(32,32);
	 Player.SetPosition(119.0f,82.0f);
	 Player.SetState(STATE_LOOKRIGHT);

	return res;
}

bool cGame::Loop() {
	bool res=true;

	res = Process();
	if (res) Render();

	return res;
}

void cGame::Finalize() {}

// Input
void cGame::ReadKeyboard(unsigned char key, int x, int y, bool press) {
	keys[key] = press;
}

void cGame::ReadMouse(int button, int state, int x, int y) {}

// Process
bool cGame::Process() {
	bool res=true;
	
	// Process Input
	if (keys[27]) res=false;
	
	if (keys[GLUT_KEY_UP])			Player.MoveUp(Scene.GetMap());
	else if (keys[GLUT_KEY_DOWN])	Player.MoveDown(Scene.GetMap());
	if (keys[GLUT_KEY_LEFT])		Player.MoveLeft(Scene.GetMap());
	else if (keys[GLUT_KEY_RIGHT])	Player.MoveRight(Scene.GetMap());
	else Player.Stop();
	
	// Game Logic
	//Player.Logic(Scene.GetMap());

	return res;
}

//Output
void cGame::Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	glLoadIdentity();

	float x, y;
	Player.GetPosition(&x, &y);

	Scene.Draw(Data.GetID(IMG_OVERLORD), x, y);
	Player.Draw(Data.GetID(IMG_PLAYER));

	glutSwapBuffers();
}