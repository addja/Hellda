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
	 res = Data.LoadImage(IMG_OVERWORLD,"zelda-tiles-compressed.png",GL_RGB);
	 if(!res) return false;
	 res = Overworld.Load();
	 if(!res) return false;
	 res = Data.LoadImage(IMG_DUNGEON, "dungeon1-tiles.png", GL_RGB);
	 if (!res) return false;
	 // load dungeon 1
	 res = Dungeon.LoadLevel(1);
	 if (!res) return false;

	// Player initialization
	 res = Data.LoadImage(IMG_PLAYER,"link.png",GL_RGBA);
	 if(!res) return false;
	 overworld = true;
	 Player = cPlayer(&overworld,&transition);
	 Player.SetWidthHeight(TILE_SIZE,TILE_SIZE);
	 Player.SetPosition(INITIAL_POS_LINKX,INITIAL_POS_LINKY);  // TODO poner bien
	 Player.SetState(STATE_LOOKUP);

	 // Zones initialization
	 res = Data.LoadImage(IMG_ENEMIES, "octoroc.png", GL_RGBA);
	 if (!res) return false;
	 
	 initializeEnemiesOverworld();
	 initializeEnemiesDungeons();
	 initializeObjectsOverworld();
	 initializeObjectsDungeons();

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
	bool res = true;
	bool hit;
	float x, y;
	int state;

	// Process Input
	if (keys[27]) res=false;

	if (overworld) {
		if (keys[GLUT_KEY_UP])			Player.MoveUp(Overworld.GetMap());
		else if (keys[GLUT_KEY_DOWN])	Player.MoveDown(Overworld.GetMap());
		else if (keys[GLUT_KEY_LEFT])	Player.MoveLeft(Overworld.GetMap());
		else if (keys[GLUT_KEY_RIGHT])	Player.MoveRight(Overworld.GetMap());
		else if (keys[' '])				Player.Attack();
		else Player.Stop();

		// Game Logic
		Player.Logic(Overworld.GetMap());

		Player.GetPosition(&x, &y);

		// to know the zone
		int zone = calcZone(x, y);
		std::cout << zone << std::endl;

		// get border zones
		float offsetx = OVERWORLD_MAP_WIDTH / ZONE_WIDTH / 2;
		float offsety = OVERWORLD_MAP_HEIGHT / ZONE_HEIGHT / 2;
		zones.clear();
		zones.insert(calcZone(x + offsetx, y + offsety));
		zones.insert(calcZone(x + offsetx, y - offsety));
		zones.insert(calcZone(x - offsetx, y + offsety));
		zones.insert(calcZone(x - offsetx, y - offsety));
		for (std::set<int>::iterator it = zones.begin(); it != zones.end(); ++it) {
			ZonesOverworld[*it].Logic(Overworld.GetMap(), x, y, Player.GetState());
		}

		// check intersections enemies player
		for (std::set<int>::iterator it = zones.begin(); it != zones.end(); ++it) {
			hit = Player.checkIntersections(ZonesOverworld[*it]);
			if (!hit) {
				//if (Player.health == 0) gameOver();
				break;
			}
		}

	} else {
		if (transition) {
			Player.Logic(Dungeon.GetMap());
		} else {
			if (keys[GLUT_KEY_UP])			Player.MoveUp(Dungeon.GetMap());
			else if (keys[GLUT_KEY_DOWN])	Player.MoveDown(Dungeon.GetMap());
			else if (keys[GLUT_KEY_LEFT])	Player.MoveLeft(Dungeon.GetMap());
			else if (keys[GLUT_KEY_RIGHT])	Player.MoveRight(Dungeon.GetMap());
			else if (keys[' '])				Player.Attack();
			else Player.Stop();

			// Game Logic
			Player.Logic(Dungeon.GetMap());

			Player.GetPosition(&x, &y);

			// feo feillo pero para testear
			ZonesDungeon[0].Logic(Dungeon.GetMap(), x, y, Player.GetState());
			// check intersections enemies player
			// TODO: do it for the 4 zones colliding
			hit = Player.checkIntersections(ZonesDungeon[0]);
			// game over
			// if (!hit && Player.health == 0) gameover();
		}
	}

	return res;
}

//Output
void cGame::Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	glLoadIdentity();

	float x, y;
	Player.GetPosition(&x, &y);

	if (overworld) {
		// draw scene
		Overworld.Draw(Data.GetID(IMG_OVERWORLD), x, y);

		for (std::set<int>::iterator it = zones.begin(); it != zones.end(); ++it) {
			ZonesOverworld[*it].Draw(x,y);
		}
		// draw plawer
		Player.Draw(Data.GetID(IMG_PLAYER));
	} else {
		int zone;
		Player.GetZone(&zone);
		float offset;
		Player.GetOffset(&offset);
		int state = Player.GetState();

		if (transition) {
			switch (state) {
			case STATE_WALKLEFT:
				glPushMatrix();
				glTranslatef(-GAME_WIDTH + offset, 0.0f, 0.0f);
				Dungeon.Draw(Data.GetID(IMG_DUNGEON), zone - 1);
				ZonesDungeon[0].Draw(x, y);
				glPopMatrix();
				glPushMatrix();
				glTranslatef(offset, 0.0f, 0.0f);
				Dungeon.Draw(Data.GetID(IMG_DUNGEON), zone);
				ZonesDungeon[0].Draw(x, y);
				Player.Draw(Data.GetID(IMG_PLAYER));
				Dungeon.DrawBorder(Data.GetID(IMG_DUNGEON), LEFT_BORDER);
				glPopMatrix();
				break;
			case STATE_WALKRIGHT:
				glPushMatrix();
				glTranslatef(GAME_WIDTH + offset, 0.0f, 0.0f);
				Dungeon.Draw(Data.GetID(IMG_DUNGEON), zone + 1);
				ZonesDungeon[0].Draw(x, y);
				glPopMatrix();
				glPushMatrix();
				glTranslatef(offset, 0.0f, 0.0f);
				Dungeon.Draw(Data.GetID(IMG_DUNGEON), zone);
				ZonesDungeon[0].Draw(x, y);
				Player.Draw(Data.GetID(IMG_PLAYER));
				Dungeon.DrawBorder(Data.GetID(IMG_DUNGEON), RIGHT_BORDER);
				glPopMatrix();
				break;
			case STATE_WALKUP:
				glClearColor(0.094f, 0.235f, 0.361f, 1); // Border blue
				glPushMatrix();
				glTranslatef(0.0f, GAME_HEIGHT - (HUD_TILES *GAME_HEIGHT / (SCENE_HEIGHT - 1)) - offset, 0.0f);
				Dungeon.Draw(Data.GetID(IMG_DUNGEON), zone - (DUNGEON_MAP_WIDTH / ZONE_WIDTH));
				ZonesDungeon[0].Draw(x, y);
				glPopMatrix();
				glPushMatrix();
				glTranslatef(0.0, -offset, 0.0f);
				Dungeon.Draw(Data.GetID(IMG_DUNGEON), zone);
				ZonesDungeon[0].Draw(x, y);
				Player.Draw(Data.GetID(IMG_PLAYER));
				Dungeon.DrawBorder(Data.GetID(IMG_DUNGEON), TOP_BORDER);
				glPopMatrix();
				break;
			case STATE_WALKDOWN:
				glPushMatrix();
				glTranslatef(0.0f, -GAME_HEIGHT + (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1)) + offset, 0.0f);
				Dungeon.Draw(Data.GetID(IMG_DUNGEON), zone + (DUNGEON_MAP_WIDTH / ZONE_WIDTH));
				ZonesDungeon[0].Draw(x, y);
				glPopMatrix();
				glPushMatrix();
				glTranslatef(0.0, offset, 0.0f);
				Dungeon.Draw(Data.GetID(IMG_DUNGEON), zone);
				ZonesDungeon[0].Draw(x, y);
				Player.Draw(Data.GetID(IMG_PLAYER));
				Dungeon.DrawBorder(Data.GetID(IMG_DUNGEON), BOTTOM_BORDER);
				glColor3f(0.094f, 0.235f, 0.361f);
				glRectf(0.0f, -0.0f, GAME_WIDTH, -2.0f); // Border blue
				glPopMatrix();
				break;
			}

		}
		else {
			Dungeon.Draw(Data.GetID(IMG_DUNGEON), zone);

			ZonesDungeon[0].Draw(x, y);

			// draw plawer
			Player.Draw(Data.GetID(IMG_PLAYER));
		}
	}

	// HUD black background
	if (DEBUG_MODE) std::cout << Player.health() << std::endl;
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
		glVertex2i(0, GAME_HEIGHT);
		glVertex2i(GAME_WIDTH, GAME_HEIGHT);
		glVertex2i(GAME_WIDTH, GAME_HEIGHT - 4 * (GAME_HEIGHT / (SCENE_HEIGHT - 1)));
		glVertex2i(0, GAME_HEIGHT - 4 * (GAME_HEIGHT / (SCENE_HEIGHT - 1)));
	glEnd();
	glColor4f(1, 1, 1, 1);

	// HUD hearts
	glColor3f(1.0f, 0.0f, 1.0f);
	for (int i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
			glVertex2i(GAME_WIDTH* 0.75f, GAME_HEIGHT*0.9f);
			glVertex2i(GAME_WIDTH* (0.75f + 0.0375f*i), GAME_HEIGHT*0.9f);
			glVertex2i(GAME_WIDTH* (0.75f + 0.0375f*i), GAME_HEIGHT*(0.9f + 0.01f*i));
			glVertex2i(GAME_WIDTH* 0.75f, GAME_HEIGHT*(0.9f + 0.01f*i));
		glEnd();
	}
	glColor3f(1.0f, 1.0f, 1.0f);

	glutSwapBuffers();
}

void cGame::initializeEnemiesOverworld() {
	ZonesOverworld[87] = cZone();
	ZonesOverworld[87].SetOverworld(true);
	ZonesOverworld[87].SetData(&Data);
	ZonesOverworld[87].addEnemy(118.0f, 81.0f, OCTOROCT, true, 119, &overworld);
	//ZonesOverworld[0].addEnemy(118.0f, 70.0f, OCTOROCT, true, 103, &overworld);
}

void cGame::initializeEnemiesDungeons() {
	ZonesDungeon[0] = cZone();
	ZonesDungeon[0].SetOverworld(false);
	ZonesDungeon[0].SetData(&Data);
	ZonesDungeon[0].addEnemy(8.5f, 9.5f, OCTOROCT, true, 32, &overworld);
}

void cGame::initializeObjectsOverworld() {
	
}

void cGame::initializeObjectsDungeons() {

}

int cGame::calcZone(float x, float y) {
	int zonex = floor(x / (OVERWORLD_MAP_WIDTH / ZONE_WIDTH));
	int zoney = floor(y / (OVERWORLD_MAP_HEIGHT / ZONE_HEIGHT));
	return zonex + (OVERWORLD_MAP_HEIGHT / ZONE_HEIGHT) * zoney;
}