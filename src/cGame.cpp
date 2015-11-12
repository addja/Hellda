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

	// Music and sounds initialization
	res = initializeAudio();
	if (!res) return false;

	// Scene initialization
	 res = Data.LoadImage(IMG_OVERWORLD,"zelda-tiles-compressed.png",GL_RGB);
	 if(!res) return false;
	 res = Data.LoadImage(IMG_TEXT, "text-tiles.png", GL_RGBA);
	 if (!res) return false;
	 res = Data.LoadImage(IMG_ITEMS, "items-tiles.png", GL_RGBA);
	 if (!res) return false;
	 res = Overworld.Load();
	 if(!res) return false;
	 res = Data.LoadImage(IMG_DUNGEON, "dungeon1-tiles.png", GL_RGB);
	 if (!res) return false;
	 res = Data.LoadImage(IMG_DUNGEON_MINIMAP, "dungeon1-minimap.png", GL_RGBA);
	 if (!res) return false;
	 // load dungeon 1
	 res = Dungeon.LoadLevel(1);
	 if (!res) return false;

	// Player initialization
	 res = Data.LoadImage(IMG_PLAYER,"link.png",GL_RGBA);
	 if(!res) return false;
	 overworld = true;
	 gameover = false;
	 opening = true;
	 Player = cPlayer(&overworld,&transition,&opening);
	 Player.SetWidthHeight(TILE_SIZE,TILE_SIZE);
	 Player.SetPosition(INITIAL_POS_LINKX,INITIAL_POS_LINKY);
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
	if (key == 'A') key = 'a';
	if (key == 'S') key = 's';
	keys[key] = press;
	if ((key == 'a' && !press) || (keys['a'] && 'a' != key) ) Player.endSwipe();
}

void cGame::ReadMouse(int button, int state, int x, int y) {}

// Process
bool cGame::Process() {
	bool res = true;
	bool hit;
	float x, y;
	bool knocked;
	Player.GetKnocked(&knocked);

	// Process Input
	if (keys[27]) {
		return false;
	}

	if (gameover) {
		if (gameover_delay >= 200) {
			if(keys[13]) Init();
		}
		else if (gameover_delay == 155) Player.SetState(STATE_LOOKBOOM);
		else if (gameover_delay == 180) Player.SetState(STATE_BOOM);
		else if (gameover_delay > 25 && gameover_delay < 125 && gameover_delay % 5 == 0) {
			int state = Player.GetState();
			switch (state) {
				case STATE_LOOKLEFT:
					Player.SetState(STATE_LOOKRIGHT);
					break;
				case STATE_LOOKUP:
					Player.SetState(STATE_LOOKDOWN);
					break;
				case STATE_LOOKRIGHT:
					Player.SetState(STATE_LOOKDOWN);
					if (gameover_delay > 110) gameover_delay = 125;
					break;
				case STATE_LOOKDOWN:
					Player.SetState(STATE_LOOKLEFT);
					break;
			}
		}
	} else {
		if (overworld) {
			if (knocked) {
				Player.Logic(Overworld.GetMap());
			}
			else {
				if (keys[GLUT_KEY_UP])			Player.MoveUp(Overworld.GetMap());
				else if (keys[GLUT_KEY_DOWN])	Player.MoveDown(Overworld.GetMap());
				else if (keys[GLUT_KEY_LEFT])	Player.MoveLeft(Overworld.GetMap());
				else if (keys[GLUT_KEY_RIGHT])	Player.MoveRight(Overworld.GetMap());
				else if (keys['a']) {
					if (Player.swipeAgain()) {
						Data.playSound(SOUND_SWORD);
						Player.swipe();
					}
					Player.Attack();
				}
				else Player.Stop();

				// Game Logic
				Player.Logic(Overworld.GetMap());

				if (overworld) {
					Player.GetPosition(&x, &y);

					// to know the zone
					int zone = calcZone(x, y);

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
					if (!knocked) {
						for (std::set<int>::iterator it = zones.begin(); it != zones.end(); ++it) {
							hit = Player.checkIntersections(ZonesOverworld[*it]);
							if (hit && Player.health() == 0) {
								gameOver();
								break;
							}
						}
					}
				}
			}
		}
		else {
			if (transition || knocked) {
				Player.Logic(Dungeon.GetMap());
			}
			else {
				if (keys[GLUT_KEY_UP])			Player.MoveUp(Dungeon.GetMap());
				else if (keys[GLUT_KEY_DOWN])	Player.MoveDown(Dungeon.GetMap());
				else if (keys[GLUT_KEY_LEFT])	Player.MoveLeft(Dungeon.GetMap());
				else if (keys[GLUT_KEY_RIGHT])	Player.MoveRight(Dungeon.GetMap());
				else if (keys['a']) {
					if (Player.swipeAgain()) {
						Data.playSound(SOUND_SWORD);
						Player.swipe();
					}
					Player.Attack();
				}
				else Player.Stop();

				// Game Logic
				Player.Logic(Dungeon.GetMap());

				if (!overworld) {
					Player.GetPosition(&x, &y);

					ZonesDungeon[0].Logic(Dungeon.GetMap(), x, y, Player.GetState());
					// remember to check intersections only if 
					hit = Player.checkIntersections(ZonesDungeon[0]);
					if (hit && Player.health() == 0) {
						gameOver();
					}
				}
			}
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

	if (gameover) {
		if (gameover_delay > 25 && gameover_delay < 125) glColor3f(0.847f, 0.157f, 0.0f);
		else if (gameover_delay < 130) glColor3f(0.647f, 0.124f, 0.0f);
		else if (gameover_delay < 145) glColor3f(0.447f, 0.107f, 0.0f);
		else if (gameover_delay < 150) glColor3f(0.247f, 0.087f, 0.0f);
		else if (gameover_delay < 155) glColor3f(0.147f, 0.047f, 0.0f);
		else if (gameover_delay >= 155) glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		if (gameover_delay < 155) {

			if (overworld) {
				Overworld.Draw(Data.GetID(IMG_OVERWORLD), x, y);
				glColor4f(1, 1, 1, 1);
				Player.Draw(Data.GetID(IMG_PLAYER));
			}
			else {
				int zone;
				Player.GetZone(&zone);
				Dungeon.Draw(Data.GetID(IMG_DUNGEON), zone);
				glColor4f(1, 1, 1, 1);
				Player.Draw(Data.GetID(IMG_PLAYER));
			}

			++gameover_delay;
		}
		else if (gameover_delay < 200) {
			glColor4f(1, 1, 1, 1);
			Player.Draw(Data.GetID(IMG_PLAYER));
			++gameover_delay;
		}
		else {
			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_TEXT));

			// GAME OVER
			float dx[9] = { 0.0f, 10.0f / 16.0f, 6.0f / 16.0f, 14.0f / 16.0f, 12.0f / 16.0f, 8.0f / 16.0f, 15.0f / 16.0f, 14.0f / 16.0f, 11.0f / 16.0f };
			float dy[9] = { 2.0f / 6.0f, 1.0f / 6.0f, 2.0f / 6.0f, 1.0f / 6.0f, 3.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 1.0f / 6.0f, 2.0f / 6.0f };
			for (int i = 0; i < 9; i++) {
				glBegin(GL_QUADS);
				glTexCoord2f(dx[i], dy[i]);	glVertex2i(GAME_WIDTH* (0.29f + 0.05f*i), GAME_HEIGHT*0.42f);
				glTexCoord2f(dx[i] + 0.0625f, dy[i]);	glVertex2i(GAME_WIDTH* (0.29f + 0.1f + 0.05f*i), GAME_HEIGHT*0.42f);
				glTexCoord2f(dx[i] + 0.0625f, dy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.29f + 0.1f + 0.05f*i), GAME_HEIGHT*(0.42f + 0.1f));
				glTexCoord2f(dx[i], dy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.29f + 0.05f*i), GAME_HEIGHT*(0.42f + 0.1f));
				glEnd();
			}

			// PRESS ENTER KEY TO RETRY
			float px[24] = { 9.0f / 16.0f, 11.0f / 16.0f, 14.0f / 16.0f, 12.0f / 16.0f, 12.0f / 16.0f, 13.0f / 16.0f, 14.0f / 16.0f, 7.0f / 16.0f, 13.0f / 16.0f,
				14.0f / 16.0f, 11.0f / 16.0f, 13.0f / 16.0, 4.0f / 16.0f, 14.0f / 16.0f, 2.0f / 16.0f, 13.0f / 16.0f, 13.0f / 16.0f, 8.0f / 16.0f,
				13.0f / 16.0f, 11.0f / 16.0f, 14.0f / 16.0f, 13.0f / 16.0f, 11.0f / 16.0f, 2.0f / 16.0f };
			float py[24] = { 2.0f / 6.0f, 2.0f / 6.0f, 1.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 3.0f / 6.0f, 1.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 1.0f / 6.0f,
				2.0f / 6.0f, 3.0f / 6.0f, 2.0f / 6.0f, 1.0f / 6.0f, 3.0f / 6.0f, 3.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 3.0f / 6.0f, 2.0f / 6.0f,
				1.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 3.0f / 6.0f };
			for (int i = 0; i < 24; i++) {
				glBegin(GL_QUADS);
				glTexCoord2f(px[i], py[i]);	glVertex2i(GAME_WIDTH* (0.15f + 0.03f*i), GAME_HEIGHT*0.35f);
				glTexCoord2f(px[i] + 0.0625f, py[i]);	glVertex2i(GAME_WIDTH* (0.15f + 0.06f + 0.03f*i), GAME_HEIGHT*0.35f);
				glTexCoord2f(px[i] + 0.0625f, py[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.15f + 0.06f + 0.03f*i), GAME_HEIGHT*(0.35f + 0.06f));
				glTexCoord2f(px[i], py[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.15f + 0.03f*i), GAME_HEIGHT*(0.35f + 0.06f));
				glEnd();
			}

			glDisable(GL_TEXTURE_2D);
		}
	}
	else {
		if (overworld) {
			// draw scene
			Overworld.Draw(Data.GetID(IMG_OVERWORLD), x, y);

			for (std::set<int>::iterator it = zones.begin(); it != zones.end(); ++it) {
				ZonesOverworld[*it].Draw(x, y);
			}
			// draw plawer
			Player.Draw(Data.GetID(IMG_PLAYER));
		}
		else {
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
					glColor3f(0.094f, 0.235f, 0.361f);
					glRectf(0.0f, GAME_HEIGHT - (HUD_TILES *GAME_HEIGHT / (SCENE_HEIGHT - 1)) + 1.0f, GAME_WIDTH, GAME_HEIGHT - (HUD_TILES *GAME_HEIGHT / (SCENE_HEIGHT - 1) + 2.0f)); // Border blue
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
					glRectf(0.0f, 0.0f, GAME_WIDTH, -2.0f); // Border blue
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
	}

	if (opening) {
		float offset;
		Player.GetOffset(&offset);
		glColor3f(0.0f, 0.0f, 0.0f);
		glRectf(0.0f, 0.0f, GAME_WIDTH / 2 - offset, GAME_HEIGHT);
		glRectf(GAME_WIDTH / 2 + offset, 0.0f, GAME_WIDTH, GAME_HEIGHT);
		glColor4f(1, 1, 1, 1);
		if (offset + 1.0f >= GAME_WIDTH / 2) {
			opening = false;
			Player.SetOffset(0.0f);
		}
		else Player.SetOffset(offset + 10.0f);
	}

	RenderHUD();


	glutSwapBuffers();
}

void cGame::RenderHUD() {
	// HUD black background
	//if (DEBUG_MODE) std::cout << Player.health() << std::endl;
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
		glVertex2i(0, GAME_HEIGHT);
		glVertex2i(GAME_WIDTH, GAME_HEIGHT);
		glVertex2i(GAME_WIDTH, GAME_HEIGHT - 4 * (GAME_HEIGHT / (SCENE_HEIGHT - 1)) - 1.5f);
		glVertex2i(0, GAME_HEIGHT - 4 * (GAME_HEIGHT / (SCENE_HEIGHT - 1)) - 1.5f);
	glEnd();
	glColor4f(1, 1, 1, 1); // Cleaning the colour

						   // HUD minimap
	float x, y;
	Player.GetPosition(&x, &y);
	if (overworld) {
		glColor3f(0.33f, 0.33f, 0.33f); // Grey colour
		glRectf(0.025*GAME_WIDTH, 0.775*GAME_HEIGHT, 0.38*GAME_WIDTH, 0.96*GAME_HEIGHT); // Rectangle
		glColor3f(0.364f, 0.59f, 0.045f); // Green colour
		float pointx = x * (0.35*GAME_WIDTH - 0.025*GAME_WIDTH);
		float pointy = y * (0.943*GAME_HEIGHT - 0.775*GAME_HEIGHT);
		pointx /= OVERWORLD_MAP_WIDTH;
		pointy /= OVERWORLD_MAP_HEIGHT;
		pointx += 0.025*GAME_WIDTH;
		pointy = 0.943*GAME_HEIGHT - pointy;
		glRectf(pointx, pointy, pointx + MINIMAP_PLAYER_SIZE, pointy + MINIMAP_PLAYER_SIZE); // Green point
		glColor4f(1, 1, 1, 1); // Cleaning the colour
	}
	else {
		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_TEXT));
		float dx[7] = { 13.0f / 16.0f, 14.0f / 16.0f, 7.0f / 16.0f, 0.0f, 14.0f / 16.0f, 8.0f / 16.0f, 7.0f / 16.0f };
		float dy[7] = { 1.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 1.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f };
		for (int i = 0; i < 7; i++) {
			glBegin(GL_QUADS);
				glTexCoord2f(dx[i], dy[i]);	glVertex2i(GAME_WIDTH* (0.09f + 0.032f*i), GAME_HEIGHT*0.93f);
				glTexCoord2f(dx[i] + 0.0625f, dy[i]);	glVertex2i(GAME_WIDTH* (0.09f + 0.06f + 0.032f*i), GAME_HEIGHT*0.93f);
				glTexCoord2f(dx[i] + 0.0625f, dy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.09f + 0.06f + 0.032*i), GAME_HEIGHT*(0.93f + 0.06f));
				glTexCoord2f(dx[i], dy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.09f + 0.032f*i), GAME_HEIGHT*(0.93f + 0.06f));
			glEnd();
		}

		glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_DUNGEON_MINIMAP));
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);	glVertex2i(GAME_WIDTH*0.065, GAME_HEIGHT*0.775f);
			glTexCoord2f(0.0f + 1.0f, 1.0f);	glVertex2i(GAME_WIDTH*0.34, GAME_HEIGHT*0.775f);
			glTexCoord2f(0.0f + 1.0f, 1.0f - 1.0f);	glVertex2i(GAME_WIDTH*0.34, GAME_HEIGHT*0.90);
			glTexCoord2f(0.0f, 1.0f - 1.0f);	glVertex2i(GAME_WIDTH*0.065, GAME_HEIGHT*0.90);
		glEnd();

		glDisable(GL_TEXTURE_2D);

		int zone;
		Player.GetZone(&zone);

		int zonex = (zone % (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_WIDTH;
		int zoney = DUNGEON_MAP_HEIGHT - floor(zone / (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_HEIGHT + ZONE_HEIGHT / 2;

		glColor3f(0.364f, 0.59f, 0.045f); // Green colour
		float pointx = (zonex + x - 1.5f) * (0.34*GAME_WIDTH - 0.065*GAME_WIDTH);
		float pointy = (zoney - y - 2.5f) * (0.90*GAME_HEIGHT - 0.775*GAME_HEIGHT);
		pointx /= DUNGEON_MAP_WIDTH;
		pointy /= DUNGEON_MAP_HEIGHT;
		pointx += 0.065*GAME_WIDTH;
		pointy += 0.775*GAME_HEIGHT;
		glRectf(pointx, pointy, pointx + MINIMAP_PLAYER_SIZE / 2, pointy + MINIMAP_PLAYER_SIZE / 2); // Green point
		glColor4f(1, 1, 1, 1); // Cleaning the colour
	}

	// HUD gold
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_ITEMS));
	glBegin(GL_QUADS);
		glTexCoord2f(0.5f, 0.5f);	glVertex2i(GAME_WIDTH*0.4f, GAME_HEIGHT*0.89f);
		glTexCoord2f(0.5f + 0.25f, 0.5f);	glVertex2i(GAME_WIDTH* (0.4f + 0.0375f), GAME_HEIGHT*0.89f);
		glTexCoord2f(0.5f + 0.25f, 0.5f - 0.5f);	glVertex2i(GAME_WIDTH* (0.4f + 0.0375f), GAME_HEIGHT*(0.89f + 0.0375f));
		glTexCoord2f(0.5f, 0.5f - 0.5f);	glVertex2i(GAME_WIDTH*0.4f, GAME_HEIGHT*(0.89f + 0.0375f));
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_TEXT));
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f / 16.0f, 3.0f / 6.0f);	glVertex2i(GAME_WIDTH*0.435f, GAME_HEIGHT*0.895f);
		glTexCoord2f(1.0f / 16.0f + 0.0625f, 3.0f / 6.0f);	glVertex2i(GAME_WIDTH* (0.435f + 0.0375f), GAME_HEIGHT*0.895f);
		glTexCoord2f(1.0f / 16.0f + 0.0625f, 3.0f / 6.0f - 0.16666f);	glVertex2i(GAME_WIDTH* (0.435f + 0.0375f), GAME_HEIGHT*(0.895f + 0.0375f));
		glTexCoord2f(1.0f / 16.0f, 3.0f / 6.0f - 0.16666f);	glVertex2i(GAME_WIDTH*0.435f, GAME_HEIGHT*(0.895f + 0.0375f));
	glEnd();

	//int gold = Player.gold();
	int gold = 15;
	int ind = 0;
	float nx[10] = { 0.0f, 1.0 / 16.0f, 2.0 / 16.0f, 3.0 / 16.0f, 4.0 / 16.0f, 5.0 / 16.0f, 6.0 / 16.0f, 7.0 / 16.0f, 8.0 / 16.0f, 9.0 / 16.0f };
	float ny[10] = { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f };
	int num;
	if (gold > 9 && gold < 100) {
		num = gold / 10;
		glBegin(GL_QUADS);
			glTexCoord2f(nx[num], ny[num]);	glVertex2i(GAME_WIDTH* (0.455f + 0.032f*ind), GAME_HEIGHT*0.89f);
			glTexCoord2f(nx[num] + 0.0625f, ny[num]);	glVertex2i(GAME_WIDTH* (0.455f + 0.06f + 0.032f*ind), GAME_HEIGHT*0.89f);
			glTexCoord2f(nx[num] + 0.0625f, ny[num] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.455f + 0.06f + 0.032f*ind), GAME_HEIGHT*(0.89f + 0.06f));
			glTexCoord2f(nx[num], ny[num] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.455f + 0.032f*ind), GAME_HEIGHT*(0.89f + 0.06f));
		glEnd();
		++ind;
	}
	num = gold % 10;
	glBegin(GL_QUADS);
		glTexCoord2f(nx[num], ny[num]);	glVertex2i(GAME_WIDTH* (0.455f + 0.032f*ind), GAME_HEIGHT*0.89f);
		glTexCoord2f(nx[num] + 0.0625f, ny[num]);	glVertex2i(GAME_WIDTH* (0.455f + 0.06f + 0.032f*ind), GAME_HEIGHT*0.89f);
		glTexCoord2f(nx[num] + 0.0625f, ny[num] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.455f + 0.06f + 0.032f*ind), GAME_HEIGHT*(0.89f + 0.06f));
		glTexCoord2f(nx[num], ny[num] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.455f + 0.032f*ind), GAME_HEIGHT*(0.89f + 0.06f));
	glEnd();


	// HUD keys

	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_ITEMS));
	glBegin(GL_QUADS);
		glTexCoord2f(0.75f, 1.0f);	glVertex2i(GAME_WIDTH*0.4f, GAME_HEIGHT*0.84f);
		glTexCoord2f(0.75f + 0.25f, 1.0f);	glVertex2i(GAME_WIDTH* (0.4f + 0.0375f), GAME_HEIGHT*0.84f);
		glTexCoord2f(0.75f + 0.25f, 1.0f - 0.5f);	glVertex2i(GAME_WIDTH* (0.4f + 0.0375f), GAME_HEIGHT*(0.84f + 0.0375f));
		glTexCoord2f(0.75f, 1.0f - 0.5f);	glVertex2i(GAME_WIDTH*0.4f, GAME_HEIGHT*(0.84f + 0.0375f));
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_TEXT));
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f / 16.0f, 3.0f / 6.0f);	glVertex2i(GAME_WIDTH*0.435f, GAME_HEIGHT*0.845f);
		glTexCoord2f(1.0f / 16.0f + 0.0625f, 3.0f / 6.0f);	glVertex2i(GAME_WIDTH* (0.435f + 0.0375f), GAME_HEIGHT*0.845f);
		glTexCoord2f(1.0f / 16.0f + 0.0625f, 3.0f / 6.0f - 0.16666f);	glVertex2i(GAME_WIDTH* (0.435f + 0.0375f), GAME_HEIGHT*(0.845f + 0.0375f));
		glTexCoord2f(1.0f / 16.0f, 3.0f / 6.0f - 0.16666f);	glVertex2i(GAME_WIDTH*0.435f, GAME_HEIGHT*(0.845f + 0.0375f));
	glEnd();

	//int keys = Player.keys();
	int keys = 42;
	ind = 0;
	if (keys > 9 && keys < 100) {
		num = keys / 10;
		glBegin(GL_QUADS);
			glTexCoord2f(nx[num], ny[num]);	glVertex2i(GAME_WIDTH* (0.455f + 0.032f*ind), GAME_HEIGHT*0.84f);
			glTexCoord2f(nx[num] + 0.0625f, ny[num]);	glVertex2i(GAME_WIDTH* (0.455f + 0.06f + 0.032f*ind), GAME_HEIGHT*0.84f);
			glTexCoord2f(nx[num] + 0.0625f, ny[num] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.455f + 0.06f + 0.032f*ind), GAME_HEIGHT*(0.84f + 0.06f));
			glTexCoord2f(nx[num], ny[num] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.455f + 0.032f*ind), GAME_HEIGHT*(0.84f + 0.06f));
		glEnd();
		++ind;
	}
	num = keys % 10;
	glBegin(GL_QUADS);
		glTexCoord2f(nx[num], ny[num]);	glVertex2i(GAME_WIDTH* (0.455f + 0.032f*ind), GAME_HEIGHT*0.84f);
		glTexCoord2f(nx[num] + 0.0625f, ny[num]);	glVertex2i(GAME_WIDTH* (0.455f + 0.06f + 0.032f*ind), GAME_HEIGHT*0.84f);
		glTexCoord2f(nx[num] + 0.0625f, ny[num] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.455f + 0.06f + 0.032f*ind), GAME_HEIGHT*(0.84f + 0.06f));
		glTexCoord2f(nx[num], ny[num] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.455f + 0.032f*ind), GAME_HEIGHT*(0.84f + 0.06f));
	glEnd();

	// HUD bombs

	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_ITEMS));
	glBegin(GL_QUADS);
		glTexCoord2f(0.75f, 0.5f);	glVertex2i(GAME_WIDTH*0.395f, GAME_HEIGHT*0.79f);
		glTexCoord2f(0.75f + 0.25f, 0.5f);	glVertex2i(GAME_WIDTH* (0.395f + 0.0375f), GAME_HEIGHT*0.79f);
		glTexCoord2f(0.75f + 0.25f, 0.5f - 0.5f);	glVertex2i(GAME_WIDTH* (0.395f + 0.0375f), GAME_HEIGHT*(0.79f + 0.0375f));
		glTexCoord2f(0.75f, 0.5f - 0.5f);	glVertex2i(GAME_WIDTH*0.395f, GAME_HEIGHT*(0.79f + 0.0375f));
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_TEXT));
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f / 16.0f, 3.0f / 6.0f);	glVertex2i(GAME_WIDTH*0.435f, GAME_HEIGHT*0.795f);
		glTexCoord2f(1.0f / 16.0f + 0.0625f, 3.0f / 6.0f);	glVertex2i(GAME_WIDTH* (0.435f + 0.0375f), GAME_HEIGHT*0.795f);
		glTexCoord2f(1.0f / 16.0f + 0.0625f, 3.0f / 6.0f - 0.16666f);	glVertex2i(GAME_WIDTH* (0.435f + 0.0375f), GAME_HEIGHT*(0.795f + 0.0375f));
		glTexCoord2f(1.0f / 16.0f, 3.0f / 6.0f - 0.16666f);	glVertex2i(GAME_WIDTH*0.435f, GAME_HEIGHT*(0.795f + 0.0375f));
	glEnd();

	//int bombs = Player.bombs();
	int bombs = 69;
	ind = 0;
	if (bombs > 9 && bombs < 100) {
		num = bombs / 10;
		glBegin(GL_QUADS);
			glTexCoord2f(nx[num], ny[num]);	glVertex2i(GAME_WIDTH* (0.455f + 0.032f*ind), GAME_HEIGHT*0.79f);
			glTexCoord2f(nx[num] + 0.0625f, ny[num]);	glVertex2i(GAME_WIDTH* (0.455f + 0.06f + 0.032f*ind), GAME_HEIGHT*0.79f);
			glTexCoord2f(nx[num] + 0.0625f, ny[num] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.455f + 0.06f + 0.032f*ind), GAME_HEIGHT*(0.79f + 0.06f));
			glTexCoord2f(nx[num], ny[num] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.455f + 0.032f*ind), GAME_HEIGHT*(0.79f + 0.06f));
		glEnd();
		++ind;
	}
	num = bombs % 10;
	glBegin(GL_QUADS);
		glTexCoord2f(nx[num], ny[num]);	glVertex2i(GAME_WIDTH* (0.455f + 0.032f*ind), GAME_HEIGHT*0.79f);
		glTexCoord2f(nx[num] + 0.0625f, ny[num]);	glVertex2i(GAME_WIDTH* (0.455f + 0.06f + 0.032f*ind), GAME_HEIGHT*0.79f);
		glTexCoord2f(nx[num] + 0.0625f, ny[num] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.455f + 0.06f + 0.032f*ind), GAME_HEIGHT*(0.79f + 0.06f));
		glTexCoord2f(nx[num], ny[num] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.455f + 0.032f*ind), GAME_HEIGHT*(0.79f + 0.06f));
	glEnd();

	// HUD life text

	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_TEXT));
	float xo[6] = { 0.6875f, 0.3125, 0.125f, 0.9375f, 0.875f, 0.6875f };
	float yo[6] = { 1.0f, 0.83333f, 0.83333f, 0.66666f, 0.66666f, 1.0f };
	for (int i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
			glTexCoord2f(xo[i], yo[i]);	glVertex2i(GAME_WIDTH* (0.78f + 0.032f*i), GAME_HEIGHT*0.9f);
			glTexCoord2f(xo[i] + 0.0625f, yo[i]);	glVertex2i(GAME_WIDTH* (0.78f + 0.06f + 0.032f*i), GAME_HEIGHT*0.9f);
			glTexCoord2f(xo[i] + 0.0625f, yo[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.78f + 0.06f + 0.032*i), GAME_HEIGHT*(0.9f + 0.06f));
			glTexCoord2f(xo[i], yo[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.78f + 0.032f*i), GAME_HEIGHT*(0.9f + 0.06f));
		glEnd();
	}

	// HUD hearts
	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_ITEMS));

	float lives = Player.health();
	if (lives >= 3) {
		xo[2] = 0.0f;
		yo[2] = 0.5f;
	}
	else if (lives >= 2.5f) {
		xo[2] = 0.25f;
		yo[2] = 0.5f;
	}
	else {
		xo[2] = 0.25f;
		yo[2] = 1.0f;
	}
	if (lives > 1.5f) {
		xo[1] = 0.0f;
		yo[1] = 0.5f;
	}
	else if (lives > 1) {
		xo[1] = 0.25f;
		yo[1] = 0.5f;
	}
	else {
		xo[1] = 0.25f;
		yo[1] = 1.0f;
	}
	if (lives > 0.5f) {
		xo[0] = 0.0f;
		yo[0] = 0.5f;
	}
	else if (lives > 0) {
		xo[0] = 0.25f;
		yo[0] = 0.5f;
	}
	else {
		xo[0] = 0.25f;
		yo[0] = 1;
	}
	for (int i = 0; i < LINK_LIVES; i++) {
		glBegin(GL_QUADS);
			glTexCoord2f(xo[i], yo[i]);	glVertex2i(GAME_WIDTH* (0.782f + 0.06f*i), GAME_HEIGHT*0.825f);
			glTexCoord2f(xo[i] + 0.25f, yo[i]);	glVertex2i(GAME_WIDTH* (0.782f + 0.06f + 0.06f*i), GAME_HEIGHT*0.825f);
			glTexCoord2f(xo[i] + 0.25f, yo[i] - 0.5f);	glVertex2i(GAME_WIDTH* (0.782f + 0.06f + 0.06f*i), GAME_HEIGHT*(0.825f + 0.06f));
			glTexCoord2f(xo[i], yo[i] - 0.5f);	glVertex2i(GAME_WIDTH* (0.782f + 0.06f*i), GAME_HEIGHT*(0.825f + 0.06f));
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);

	// HUD weapon A & B

	glColor3f(0.0988f, 0.1728f, 0.7284f);
		glRectf(GAME_WIDTH*0.56f, GAME_HEIGHT*0.805f, GAME_WIDTH*0.63f, GAME_HEIGHT*0.915f);
		glRectf(GAME_WIDTH*0.66f, GAME_HEIGHT*0.805f, GAME_WIDTH*0.73f, GAME_HEIGHT*0.915f);
	glColor3f(0, 0, 0);
		glRectf(GAME_WIDTH*0.566f, GAME_HEIGHT*0.81f, GAME_WIDTH*0.625f, GAME_HEIGHT*0.908f);
		glRectf(GAME_WIDTH*0.666f, GAME_HEIGHT*0.81f, GAME_WIDTH*0.725f, GAME_HEIGHT*0.908f);
	glColor4f(1, 1, 1, 1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_TEXT));

	// A letter
	glBegin(GL_QUADS);
		glTexCoord2f(10.0f / 16.0f, 1.0f / 6.0f);	glVertex2i(GAME_WIDTH*0.583f, GAME_HEIGHT*0.895f);
		glTexCoord2f(10.0f / 16.0f + 0.0625f, 1.0f / 6.0f);	glVertex2i(GAME_WIDTH* (0.583f + 0.06f), GAME_HEIGHT*0.895f);
		glTexCoord2f(10.0f / 16.0f + 0.0625f, 1.0f / 6.0f - 0.16666f);	glVertex2i(GAME_WIDTH* (0.583f + 0.06f), GAME_HEIGHT*(0.895f + 0.06f));
		glTexCoord2f(10.0f / 16.0f, 1.0f / 6.0f - 0.16666f);	glVertex2i(GAME_WIDTH*0.583f, GAME_HEIGHT*(0.895f + 0.06f));
	glEnd();

	// S letter
	glBegin(GL_QUADS);
		glTexCoord2f(12.0f / 16.0f, 2.0f / 6.0f);	glVertex2i(GAME_WIDTH*0.683f, GAME_HEIGHT*0.895f);
		glTexCoord2f(12.0f / 16.0f + 0.0625f, 2.0f / 6.0f);	glVertex2i(GAME_WIDTH* (0.683f + 0.06f), GAME_HEIGHT*0.895f);
		glTexCoord2f(12.0f / 16.0f + 0.0625f, 2.0f / 6.0f - 0.16666f);	glVertex2i(GAME_WIDTH* (0.683f + 0.06f), GAME_HEIGHT*(0.895f + 0.06f));
		glTexCoord2f(12.0f / 16.0f, 2.0f / 6.0f - 0.16666f);	glVertex2i(GAME_WIDTH*0.683f, GAME_HEIGHT*(0.895f + 0.06f));
	glEnd();

	// Sword
	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_PLAYER));
	glBegin(GL_QUADS);
		glTexCoord2f(2.0f / 5.0f, 1.0f);	glVertex2i(GAME_WIDTH*0.5725f, GAME_HEIGHT*0.83f);
		glTexCoord2f(2.0f / 5.0f + 0.2f, 1.0f);	glVertex2i(GAME_WIDTH* (0.5725f + 0.06f), GAME_HEIGHT*0.83f);
		glTexCoord2f(2.0f / 5.0f + 0.2f, 1.0f - 0.25f);	glVertex2i(GAME_WIDTH* (0.5725f + 0.06f), GAME_HEIGHT*(0.83f + 0.06f));
		glTexCoord2f(2.0f / 5.0f, 1.0f - 0.25f);	glVertex2i(GAME_WIDTH*0.5725f, GAME_HEIGHT*(0.83f + 0.06f));
	glEnd();

	// Bomb
	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_ITEMS));
	glBegin(GL_QUADS);
		glTexCoord2f(0.75f, 0.5f);	glVertex2i(GAME_WIDTH*0.665f, GAME_HEIGHT*0.83f);
		glTexCoord2f(0.75f + 0.25f, 0.5f);	glVertex2i(GAME_WIDTH* (0.665f + 0.06f), GAME_HEIGHT*0.83f);
		glTexCoord2f(0.75f + 0.25f, 0.5f - 0.5f);	glVertex2i(GAME_WIDTH* (0.665f + 0.06f), GAME_HEIGHT*(0.83f + 0.06f));
		glTexCoord2f(0.75f, 0.5f - 0.5f);	glVertex2i(GAME_WIDTH*0.665f, GAME_HEIGHT*(0.83f + 0.06f));
	glEnd();

	glDisable(GL_TEXTURE_2D);

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

void cGame::gameOver() {
	Player.SetKnocked(false);
	Player.SetState(STATE_LOOKDOWN);
	gameover = true;
	gameover_delay = 0;
}

bool cGame::initializeAudio() {
	if (!Data.LoadSound(SOUND_SWORD, "../sounds/sword-sound.wav")) return false;
	if(!Data.LoadMusic(MUSIC_OVERWORLD, "../sounds/Overworld.ogg")) return false;
	Data.playMusic(MUSIC_OVERWORLD);
	return true;
}