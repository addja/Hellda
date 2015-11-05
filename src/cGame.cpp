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
	 res = Data.LoadImage(IMG_PLAYER,"link.png",GL_RGBA);
	 if(!res) return false;
	 Player.SetWidthHeight(TILE_SIZE,TILE_SIZE);
	 Player.SetPosition(120.0f,82.0f);  // TODO poner bien
	 Player.SetState(STATE_LOOKUP);

	 // Zones initialization
	 // Enemies
	 Zones[0] = cZone();
	 Zones[0].addEnemy(118.0f, 81.0f, OCTOROC, true);
	 Zones[0].addEnemy(118.0f, 70.0f, OCTOROC, true);

	 // Objects

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
	else if (keys[GLUT_KEY_LEFT])	Player.MoveLeft(Scene.GetMap());
	else if (keys[GLUT_KEY_RIGHT])	Player.MoveRight(Scene.GetMap());
	else if (keys[' '])				Player.Attack();
	else Player.Stop();
	
	// Game Logic
	//Player.Logic(Scene.GetMap());
	
	// feo feillo pero para testear
	Zones[0].Logic(Scene.GetMap());
	Zones[1].Logic(Scene.GetMap());


	return res;
}

//Output
void cGame::Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	glLoadIdentity();

	float x, y;
	Player.GetPosition(&x, &y);

	// draw scene
	Scene.Draw(Data.GetID(IMG_OVERLORD), x, y);
	
	// feo feillo pero para testear
	Zones[0].Draw(x, y);
	Zones[1].Draw(x,y);

	// draw plawer
	Player.Draw(Data.GetID(IMG_PLAYER));

	// HUD black background
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
		glVertex2i(0, GAME_HEIGHT);
		glVertex2i(GAME_WIDTH, GAME_HEIGHT);
		glVertex2i(GAME_WIDTH, GAME_HEIGHT - 4 * (GAME_HEIGHT / (SCENE_HEIGHT - 1)));
		glVertex2i(0, GAME_HEIGHT - 4 * (GAME_HEIGHT / (SCENE_HEIGHT - 1)));
	glEnd();
	glColor4f(1, 1, 1, 1);

	glutSwapBuffers();
}