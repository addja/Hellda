#include "include/cGame.h"
#include "include/Globals.h"

cGame::cGame(void) {
	intro = true;
	delay = 0;
}

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
	 res = Data.LoadImage(IMG_INTRO, "intro.png", GL_RGB);
	 if (!res) return false;
	 res = Data.LoadImage(IMG_WATERFALL, "waterfall.png", GL_RGBA);
	 if (!res) return false;
	 res = Data.LoadImage(IMG_OVERWORLD,"zelda-tiles-compressed.png",GL_RGB);
	 if(!res) return false;
	 res = Data.LoadImage(IMG_TEXT, "text-tiles.png", GL_RGBA);
	 if (!res) return false;
	 res = Data.LoadImage(IMG_ITEMS_HUD, "items-hud-tiles.png", GL_RGBA);
	 if (!res) return false;
	 res = Data.LoadImage(IMG_OBJECTS, "objects-tiles.png", GL_RGBA);
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
	 Player = cPlayer(&overworld,&transition,&opening,&win,&Data);
	 Player.SetWidthHeight(TILE_SIZE,TILE_SIZE);
	 Player.SetPosition(INITIAL_POS_LINKX,INITIAL_POS_LINKY);
	 Player.SetState(STATE_LOOKUP);

	 // Zones initialization
	 res = Data.LoadImage(IMG_OCTOROC, "octoroc.png", GL_RGBA);
	 if (!res) return false;
	 res = Data.LoadImage(IMG_STALFOS, "stalfos.png", GL_RGBA);
	 if (!res) return false;
	 res = Data.LoadImage(IMG_KEESE, "keese.png", GL_RGBA);
	 if (!res) return false;
	 res = Data.LoadImage(IMG_BOSS, "boss.png", GL_RGBA);
	 if (!res) return false;
	 res = Data.LoadImage(IMG_AFRO, "afro.png", GL_RGBA);
	 if (!res) return false;
	 res = Data.LoadImage(IMG_BLONDE, "blonde.png", GL_RGBA);
	 if (!res) return false;
	 
	 initializeEnemiesOverworld();
	 initializeEnemiesDungeons();
	 initializeObjectsOverworld();
	 initializeObjectsDungeons();

	 delay = 0.0f;

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
	setZones();

	// Process Input
	if (keys[27]) {
		return false;
	}

	if (intro) {
		if (keys[13]) {
			intro = false;
			delay = 0.0f;
		}

		if (delay > 60) {
			kx = 0.0f;
			ky = 0.5f;
			delay = 0;
		}
		else if (delay > 50) {
			kx = 0.5f;
			ky = 0.5f;
		}
		else if (delay > 40) {
			kx = 0.0f;
			ky = 1.0f;
		}
		else if (delay > 30) {
			kx = 0.5f;
			ky = 1.0f;
		}
		else if (delay > 20) {
			kx = 0.0f;
			ky = 1.0f;
		}
		else if (delay > 10) {
			kx = 0.5f;
			ky = 0.5f;
		}
		else {
			kx = 0.0f;
			ky = 0.5f;
		}
		++delay;
		return res;
	}

	if (gameover) {
		if (delay >= 150) {
			if (delay == 1500) {
				Data.stopMusic(MUSIC_GAMEOVER);
				Data.playMusic(MUSIC_LOVE);
			}
			if (keys[13]) {
				Init();
				setZones();
				return res;
			}
		}
		else if (delay == 105) Player.SetState(STATE_LOOKBOOM);
		else if (delay == 130) Player.SetState(STATE_BOOM);
		else if (delay > 25 && delay < 100 && delay % 5 == 0) {
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
					if (delay > 98) delay = 105;
					break;
				case STATE_LOOKDOWN:
					Player.SetState(STATE_LOOKLEFT);
					break;
			}
		}
	}
	else if (win) {
		if (delay < 10.0f) {
			Data.stopMusic(MUSIC_DUNGEON);
			Data.playMusic(MUSIC_WIN);
		}
		if (delay >= 100) {
			if (delay == 300) {
				Data.stopMusic(MUSIC_WIN);
				Data.playMusic(MUSIC_LOVE);
			}
		}
	}
	else {
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
						Player.Attack(); // this is because we want to have the magic sword shoot, we are not bad coders
						Player.swipe();
					}
					else Player.Attack();
				}
				else Player.Stop();

				// Game Logic
				Player.Logic(Overworld.GetMap());

				if (overworld) {
					Player.GetPosition(&x, &y);
					float bx, by;
					bool control = false;
					Player.getBulletPos(bx, by);
					for (std::set<int>::iterator it = zones.begin(); it != zones.end(); ++it) {
						ZonesOverworld[*it].Logic(Overworld.GetMap(), x, y, Player.GetState(), Player.hasShoot(),bx,by,control);
						if (control) {
							Player.endShoot();
						}
					}

					// check intersections enemies player
					if (!knocked) {
						for (std::set<int>::iterator it = zones.begin(); it != zones.end(); ++it) {
							hit = Player.checkIntersections(&ZonesOverworld[*it]);
							if (!hit) Data.playSound(SOUND_HIT);
							if (hit && Player.health() <= 0) {
								gameOver();
								break;
							}
						}
					}
				}
				else {
					Data.stopMusic(MUSIC_OVERWORLD);
					Data.playMusic(MUSIC_DUNGEON);
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
						Player.Attack(); // this is because we want to have the magic sword shoot, we are not bad coders
						Player.swipe();
					}
					else Player.Attack();
				}
				else Player.Stop();

				// Game Logic
				Player.Logic(Dungeon.GetMap());

				if (!overworld) {
					Player.GetPosition(&x, &y);

					// to know the zone
					int zone;
					Player.GetZone(&zone);
					float bx, by;
					Player.getBulletPos(bx, by);
					bool control = false;

					ZonesDungeon[zone].Logic(Dungeon.GetMap(), x, y, Player.GetState(), Player.hasShoot(), bx, by, control);
					if (control) {
						Player.endShoot();
					}
					// remember to check intersections only if 
					hit = Player.checkIntersections(&ZonesDungeon[zone]);
					if (!hit) Data.playSound(SOUND_HIT);
					if (hit && Player.health() <= 0) {
						gameOver();
					}
				}
				else {
					Data.stopMusic(MUSIC_DUNGEON);
					Data.playMusic(MUSIC_OVERWORLD);
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

	if (intro) {
		// BACKGROUND
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_INTRO));
		glBegin(GL_QUADS);
			glTexCoord2f(kx, ky);	glVertex2i(0.0f, 0.0f);
			glTexCoord2f(kx + 0.5f, ky);	glVertex2i(GAME_WIDTH, 0.0f);
			glTexCoord2f(kx + 0.5f, ky - 0.5f);	glVertex2i(GAME_WIDTH, GAME_HEIGHT);
			glTexCoord2f(kx, ky - 0.5f);	glVertex2i(0.0f, GAME_HEIGHT);
		glEnd();

		// WATERFALL
		float vx = GAME_WIDTH/SCENE_WIDTH;
		float vy = GAME_HEIGHT / (SCENE_HEIGHT - 1);
		float rx = 2.0f / 16.0f;
		glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_WATERFALL));
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);	glVertex2i(5*vx, 0.0f);
			glTexCoord2f(0.0f + 2.0f / 16.0f, 1.0f);	glVertex2i(7*vx , 0.0f);
			glTexCoord2f(0.0f + 2.0f / 16.0f, 0.0f);	glVertex2i(7*vx, 4*vx);
			glTexCoord2f(0.0f, 0.0f);	glVertex2i(5*vx, 4*vx);
		glEnd();

		float ot = (delay / 5) % 6;	
		glBegin(GL_QUADS);
			glTexCoord2f(2.0f / 16.0f + ot*rx, 1.0f);	glVertex2i(5 * vx, 0.0f);
			glTexCoord2f(4.0f / 16.0f + ot*rx, 1.0f);	glVertex2i(7 * vx, 0.0f);
			glTexCoord2f(4.0f / 16.0f + ot*rx, 0.0f);	glVertex2i(7 * vx, 4 * vx);
			glTexCoord2f(2.0f / 16.0f + ot*rx, 0.0f);	glVertex2i(5 * vx, 4 * vx);
		glEnd();

		float ot2 = (delay / 5) % 4;
		glBegin(GL_QUADS);
			glTexCoord2f(14.0f / 16.0f, 0.25f + ot2*0.25f);	glVertex2i(5 * vx, 3 * vx);
			glTexCoord2f(1.0f, 0.25f + ot2*0.25f);	glVertex2i(7 * vx, 3 * vx);
			glTexCoord2f(1.0f, 0.0f + ot2*0.25f);	glVertex2i(7 * vx, 4 * vx);
			glTexCoord2f(14.0f / 16.0f, 0.0f + ot2*0.25f);	glVertex2i(5 * vx, 4 * vx);
		glEnd();

		// PUSH ENTER KEY
		glColor3f(0.0f, 0.0f, 0.0f);
		glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_TEXT));
		float px[14] = { 9.0f / 16.0f, 14.0f / 16.0f, 12.0f / 16.0f, 1.0f / 16.0f, 13.0f / 16.0f, 14.0f / 16.0f, 7.0f / 16.0f, 13.0f / 16.0f,
			14.0f / 16.0f, 11.0f / 16.0f, 13.0f / 16.0, 4.0f / 16.0f, 14.0f / 16.0f, 2.0f / 16.0f };
		float py[14] = { 2.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 3.0f / 6.0f, 1.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 1.0f / 6.0f,
			2.0f / 6.0f, 3.0f / 6.0f, 2.0f / 6.0f, 1.0f / 6.0f, 3.0f / 6.0f };
		for (int i = 0; i < 14; i++) {
			glBegin(GL_QUADS);
				glTexCoord2f(px[i], py[i]);	glVertex2i(GAME_WIDTH* (0.27f + 0.032f*i), GAME_HEIGHT*0.3f);
				glTexCoord2f(px[i] + 0.0625f, py[i]);	glVertex2i(GAME_WIDTH* (0.27f + 0.065f + 0.032f*i), GAME_HEIGHT*0.3f);
				glTexCoord2f(px[i] + 0.0625f, py[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.27f + 0.065f + 0.032f*i), GAME_HEIGHT*(0.3f + 0.065f));
				glTexCoord2f(px[i], py[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.27f + 0.032f*i), GAME_HEIGHT*(0.3f + 0.065f));
			glEnd();
		}

		glDisable(GL_TEXTURE_2D);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glutSwapBuffers();

		return;
	}

	if (gameover) {
		if (delay > 25 && delay < 70) glColor3f(0.847f, 0.157f, 0.0f);
		else if (delay < 75) glColor3f(0.647f, 0.124f, 0.0f);
		else if (delay < 80) glColor3f(0.447f, 0.107f, 0.0f);
		else if (delay < 85) glColor3f(0.247f, 0.087f, 0.0f);
		else if (delay < 95) glColor3f(0.147f, 0.047f, 0.0f);
		else if (delay >= 115) glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		if (delay < 105) {

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
		}
		else if (delay < 150) {
			glColor4f(1, 1, 1, 1);
			Player.Draw(Data.GetID(IMG_PLAYER));
		}
		else if (delay < 1500) {
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

			// PUSH ENTER KEY TO RETRY
			float px[23] = { 9.0f / 16.0f, 14.0f / 16.0f, 12.0f / 16.0f, 1.0f / 16.0f, 13.0f / 16.0f, 14.0f / 16.0f, 7.0f / 16.0f, 13.0f / 16.0f,
				14.0f / 16.0f, 11.0f / 16.0f, 13.0f / 16.0, 4.0f / 16.0f, 14.0f / 16.0f, 2.0f / 16.0f, 13.0f / 16.0f, 13.0f / 16.0f, 8.0f / 16.0f,
				13.0f / 16.0f, 11.0f / 16.0f, 14.0f / 16.0f, 13.0f / 16.0f, 11.0f / 16.0f, 2.0f / 16.0f };
			float py[23] = { 2.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 3.0f / 6.0f, 1.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 1.0f / 6.0f,
				2.0f / 6.0f, 3.0f / 6.0f, 2.0f / 6.0f, 1.0f / 6.0f, 3.0f / 6.0f, 3.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 3.0f / 6.0f, 2.0f / 6.0f,
				1.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 3.0f / 6.0f };
			for (int i = 0; i < 23; i++) {
				glBegin(GL_QUADS);
				glTexCoord2f(px[i], py[i]);	glVertex2i(GAME_WIDTH* (0.15f + 0.03f*i), GAME_HEIGHT*0.35f);
				glTexCoord2f(px[i] + 0.0625f, py[i]);	glVertex2i(GAME_WIDTH* (0.15f + 0.05f + 0.03f*i), GAME_HEIGHT*0.35f);
				glTexCoord2f(px[i] + 0.0625f, py[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.15f + 0.05f + 0.03f*i), GAME_HEIGHT*(0.35f + 0.05f));
				glTexCoord2f(px[i], py[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.15f + 0.03f*i), GAME_HEIGHT*(0.35f + 0.05f));
				glEnd();
			}

			glDisable(GL_TEXTURE_2D);
		}
		else {
			glPushMatrix();
			if (delay < GAME_HEIGHT + 1500) glTranslatef(0.0f, delay - 1500, 0.0f);
			else if (delay < GAME_HEIGHT + 1500 + 100) glTranslatef(0.0f, GAME_HEIGHT, 0.0f);
			else if (delay < GAME_HEIGHT + GAME_HEIGHT + 1500 + 100) glTranslatef(0.0f, delay - 1500 - 100, 0.0f);
			else if (delay < GAME_HEIGHT + GAME_HEIGHT + 1500 + 100 + 100) glTranslatef(0.0f, 2*GAME_HEIGHT, 0.0f);
			else if (delay < GAME_HEIGHT/2 + GAME_HEIGHT + GAME_HEIGHT + 1500 + 100 + 100) glTranslatef(0.0f, delay - 1500 - 100 - 100, 0.0f);
			else delay = 200;
			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_TEXT));

			float vx = 1.0f / 16.0f;
			float vy = 1.0f / 6.0f;

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

			// PUSH ENTER KEY TO RETRY
			float px[23] = { 9.0f / 16.0f, 14.0f / 16.0f, 12.0f / 16.0f, 1.0f / 16.0f, 13.0f / 16.0f, 14.0f / 16.0f, 7.0f / 16.0f, 13.0f / 16.0f,
				14.0f / 16.0f, 11.0f / 16.0f, 13.0f / 16.0, 4.0f / 16.0f, 14.0f / 16.0f, 2.0f / 16.0f, 13.0f / 16.0f, 13.0f / 16.0f, 8.0f / 16.0f,
				13.0f / 16.0f, 11.0f / 16.0f, 14.0f / 16.0f, 13.0f / 16.0f, 11.0f / 16.0f, 2.0f / 16.0f };
			float py[23] = { 2.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 3.0f / 6.0f, 1.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 1.0f / 6.0f,
				2.0f / 6.0f, 3.0f / 6.0f, 2.0f / 6.0f, 1.0f / 6.0f, 3.0f / 6.0f, 3.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 3.0f / 6.0f, 2.0f / 6.0f,
				1.0f / 6.0f, 2.0f / 6.0f, 2.0f / 6.0f, 3.0f / 6.0f };
			for (int i = 0; i < 23; i++) {
				glBegin(GL_QUADS);
				glTexCoord2f(px[i], py[i]);	glVertex2i(GAME_WIDTH* (0.15f + 0.03f*i), GAME_HEIGHT*0.35f);
				glTexCoord2f(px[i] + 0.0625f, py[i]);	glVertex2i(GAME_WIDTH* (0.15f + 0.06f + 0.03f*i), GAME_HEIGHT*0.35f);
				glTexCoord2f(px[i] + 0.0625f, py[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.15f + 0.06f + 0.03f*i), GAME_HEIGHT*(0.35f + 0.06f));
				glTexCoord2f(px[i], py[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.15f + 0.03f*i), GAME_HEIGHT*(0.35f + 0.06f));
				glEnd();
			}


			// GUILLERMO OJEDA NODA
			float gx[20] = {0.0f, 14*vx, 2*vx, 5*vx, 5*vx, 14*vx, 11*vx, 6*vx, 8*vx, 12*vx, 8*vx, 3*vx, 14*vx, 13*vx, 10*vx, 12*vx, 7*vx, 8*vx, 13*vx, 10*vx };
			float gy[20] = {2*vy, 2*vy, 2*vy, 2*vy, 2*vy, vy, 2*vy, 2*vy, 2*vy, 3*vy, 2*vy, 2*vy, vy, vy, vy, 3*vy, 2*vy, 2*vy, vy, vy };
			for (int i = 0; i < 20; i++) {
				glBegin(GL_QUADS);
				glTexCoord2f(gx[i], gy[i]);	glVertex2i(GAME_WIDTH* (0.1f + 0.04f*i), -GAME_HEIGHT + GAME_HEIGHT*0.42f);
				glTexCoord2f(gx[i] + 0.0625f, gy[i]);	glVertex2i(GAME_WIDTH* (0.1f + 0.07f + 0.04f*i), -GAME_HEIGHT + GAME_HEIGHT*0.42f);
				glTexCoord2f(gx[i] + 0.0625f, gy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.1f + 0.07f + 0.04f*i), -GAME_HEIGHT + GAME_HEIGHT*(0.42f + 0.07f));
				glTexCoord2f(gx[i], gy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.1f + 0.04f*i), -GAME_HEIGHT + GAME_HEIGHT*(0.42f + 0.07f));
				glEnd();
			}

			glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_AFRO));

			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);	glVertex2i(GAME_WIDTH* (0.4f), -GAME_HEIGHT + GAME_HEIGHT*0.2f);
			glTexCoord2f(1.0f, 1.0f);	glVertex2i(GAME_WIDTH* (0.4f + 0.2f), -GAME_HEIGHT + GAME_HEIGHT*0.2f);
			glTexCoord2f(1.0f, 0.0f);	glVertex2i(GAME_WIDTH* (0.4f + 0.2f), -GAME_HEIGHT + GAME_HEIGHT*(0.2f + 0.2f));
			glTexCoord2f(0.0f, 0.0f);	glVertex2i(GAME_WIDTH* (0.4f), -GAME_HEIGHT + GAME_HEIGHT*(0.2f + 0.2f));
			glEnd();

			glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_TEXT));

			
			// DANIEL OTERO AVALLE
			float ox[19] = { 13 * vx, 10 * vx, 7 * vx, 2 * vx, 14 * vx, 5 * vx, 12 * vx, 8 * vx, 13 * vx, 14 * vx, 11 * vx, 8 * vx, 12 * vx, 10 * vx, 15 * vx, 10 * vx, 5 * vx, 5 * vx, 14 * vx };
			float oy[19] = { vy, vy, 2 * vy, 2 * vy, vy, 2 * vy, 3 * vy, 2 * vy, 2 * vy, vy, 2 * vy, 2 * vy, 3 * vy, vy, 2 * vy, vy, 2 * vy, 2 * vy, vy };
			for (int i = 0; i < 19; i++) {
				glBegin(GL_QUADS);
				glTexCoord2f(ox[i], oy[i]);	glVertex2i(GAME_WIDTH* (0.1f + 0.04f*i), -2 * GAME_HEIGHT + GAME_HEIGHT*0.42f);
				glTexCoord2f(ox[i] + 0.0625f, oy[i]);	glVertex2i(GAME_WIDTH* (0.1f + 0.07f + 0.04f*i), -2 * GAME_HEIGHT + GAME_HEIGHT*0.42f);
				glTexCoord2f(ox[i] + 0.0625f, oy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.1f + 0.07f + 0.04f*i), -2 * GAME_HEIGHT + GAME_HEIGHT*(0.42f + 0.07f));
				glTexCoord2f(ox[i], oy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.1f + 0.04f*i), -2 * GAME_HEIGHT + GAME_HEIGHT*(0.42f + 0.07f));
				glEnd();
			}
			
			glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_BLONDE));

			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);	glVertex2i(GAME_WIDTH* (0.4f), -2 * GAME_HEIGHT + GAME_HEIGHT*0.15f);
			glTexCoord2f(1.0f, 1.0f);	glVertex2i(GAME_WIDTH* (0.4f + 0.2f), -2 * GAME_HEIGHT + GAME_HEIGHT*0.15f);
			glTexCoord2f(1.0f, 0.0f);	glVertex2i(GAME_WIDTH* (0.4f + 0.2f), -2 * GAME_HEIGHT + GAME_HEIGHT*(0.15f + 0.2f));
			glTexCoord2f(0.0f, 0.0f);	glVertex2i(GAME_WIDTH* (0.4f), -2 * GAME_HEIGHT + GAME_HEIGHT*(0.15f + 0.2f));
			glEnd();

			glDisable(GL_TEXTURE_2D);


			glDisable(GL_TEXTURE_2D);
			glPopMatrix();
		}
		++delay;
	}
	else if (win) {
		if (delay > 65 && delay < 70) glColor3f(0.847f, 0.847f, 0.847f);
		else if (delay < 75) glColor3f(0.647f, 0.647f, 0.647f);
		else if (delay < 80) glColor3f(0.447f, 0.447f, 0.447f);
		else if (delay < 85) glColor3f(0.247f, 0.247f, 0.247f);
		else if (delay < 95) glColor3f(0.147f, 0.147f, 0.147f);
		else if (delay >= 100) glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		if (delay < 100) {

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
		}
		else if (delay < 150) {
			glColor4f(1, 1, 1, 1);
			Player.Draw(Data.GetID(IMG_PLAYER));
		}
		else if (delay < 300) {
			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_TEXT));

			float vx = 1.0f / 16.0f;
			float vy = 1.0f / 6.0f;

			// YOU WIN!
			float dx[8] = { 2 * vx, 8 * vx, 14 * vx, 12 * vx, 0.0f, 2 * vx, 7 * vx, 5 * vx };
			float dy[8] = { 3 * vy, 2 * vy, 2 * vy, 3 * vy, 3 * vy, 2 * vy, 2 * vy, 3 * vy };
			for (int i = 0; i < 8; i++) {
				glBegin(GL_QUADS);
				glTexCoord2f(dx[i], dy[i]);	glVertex2i(GAME_WIDTH* (0.29f + 0.06f*i), GAME_HEIGHT*0.42f);
				glTexCoord2f(dx[i] + 0.0625f, dy[i]);	glVertex2i(GAME_WIDTH* (0.29f + 0.12f + 0.06f*i), GAME_HEIGHT*0.42f);
				glTexCoord2f(dx[i] + 0.0625f, dy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.29f + 0.12f + 0.06f*i), GAME_HEIGHT*(0.42f + 0.12f));
				glTexCoord2f(dx[i], dy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.29f + 0.06f*i), GAME_HEIGHT*(0.42f + 0.12f));
				glEnd();
			}

			glDisable(GL_TEXTURE_2D);
		}
		else {
			glPushMatrix();
			if (delay < GAME_HEIGHT + 300) glTranslatef(0.0f, delay - 300, 0.0f);
			else if (delay < GAME_HEIGHT + 300 + 100) glTranslatef(0.0f, GAME_HEIGHT, 0.0f);
			else if (delay < GAME_HEIGHT + GAME_HEIGHT + 300 + 100) glTranslatef(0.0f, delay - 300 - 100, 0.0f);
			else if (delay < GAME_HEIGHT + GAME_HEIGHT + 300 + 100 + 100) glTranslatef(0.0f, 2 * GAME_HEIGHT, 0.0f);
			else if (delay < GAME_HEIGHT / 2 + GAME_HEIGHT + GAME_HEIGHT + 300 + 100 + 100) glTranslatef(0.0f, delay - 300 - 100 - 100, 0.0f);
			else delay = 150;
			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_TEXT));

			float vx = 1.0f / 16.0f;
			float vy = 1.0f / 6.0f;

			// YOU WIN!
			float dx[8] = { 2 * vx, 8 * vx, 14 * vx, 12 * vx, 0.0f, 2 * vx, 7 * vx, 5 * vx };
			float dy[8] = { 3 * vy, 2 * vy, 2 * vy, 3 * vy, 3 * vy, 2 * vy, 2 * vy, 3 * vy };
			for (int i = 0; i < 8; i++) {
				glBegin(GL_QUADS);
				glTexCoord2f(dx[i], dy[i]);	glVertex2i(GAME_WIDTH* (0.29f + 0.06f*i), GAME_HEIGHT*0.42f);
				glTexCoord2f(dx[i] + 0.0625f, dy[i]);	glVertex2i(GAME_WIDTH* (0.29f + 0.12f + 0.06f*i), GAME_HEIGHT*0.42f);
				glTexCoord2f(dx[i] + 0.0625f, dy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.29f + 0.12f + 0.06f*i), GAME_HEIGHT*(0.42f + 0.12f));
				glTexCoord2f(dx[i], dy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.29f + 0.06f*i), GAME_HEIGHT*(0.42f + 0.12f));
				glEnd();
			}

			// GUILLERMO OJEDA NODA
			float gx[20] = { 0.0f, 14 * vx, 2 * vx, 5 * vx, 5 * vx, 14 * vx, 11 * vx, 6 * vx, 8 * vx, 12 * vx, 8 * vx, 3 * vx, 14 * vx, 13 * vx, 10 * vx, 12 * vx, 7 * vx, 8 * vx, 13 * vx, 10 * vx };
			float gy[20] = { 2 * vy, 2 * vy, 2 * vy, 2 * vy, 2 * vy, vy, 2 * vy, 2 * vy, 2 * vy, 3 * vy, 2 * vy, 2 * vy, vy, vy, vy, 3 * vy, 2 * vy, 2 * vy, vy, vy };
			for (int i = 0; i < 20; i++) {
				glBegin(GL_QUADS);
				glTexCoord2f(gx[i], gy[i]);	glVertex2i(GAME_WIDTH* (0.1f + 0.04f*i), -GAME_HEIGHT + GAME_HEIGHT*0.42f);
				glTexCoord2f(gx[i] + 0.0625f, gy[i]);	glVertex2i(GAME_WIDTH* (0.1f + 0.07f + 0.04f*i), -GAME_HEIGHT + GAME_HEIGHT*0.42f);
				glTexCoord2f(gx[i] + 0.0625f, gy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.1f + 0.07f + 0.04f*i), -GAME_HEIGHT + GAME_HEIGHT*(0.42f + 0.07f));
				glTexCoord2f(gx[i], gy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.1f + 0.04f*i), -GAME_HEIGHT + GAME_HEIGHT*(0.42f + 0.07f));
				glEnd();
			}

			glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_AFRO));

				glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 1.0f);	glVertex2i(GAME_WIDTH* (0.4f), -GAME_HEIGHT + GAME_HEIGHT*0.2f);
				glTexCoord2f(1.0f, 1.0f);	glVertex2i(GAME_WIDTH* (0.4f + 0.2f), -GAME_HEIGHT + GAME_HEIGHT*0.2f);
				glTexCoord2f(1.0f, 0.0f);	glVertex2i(GAME_WIDTH* (0.4f + 0.2f), -GAME_HEIGHT + GAME_HEIGHT*(0.2f + 0.2f));
				glTexCoord2f(0.0f, 0.0f);	glVertex2i(GAME_WIDTH* (0.4f), -GAME_HEIGHT + GAME_HEIGHT*(0.2f + 0.2f));
				glEnd();

			glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_TEXT));
			glColor3f(1, 1, 1);

			// DANIEL OTERO AVALLE
			float ox[19] = { 13 * vx, 10 * vx, 7 * vx, 2 * vx, 14 * vx, 5 * vx, 12 * vx, 8 * vx, 13 * vx, 14 * vx, 11 * vx, 8 * vx, 12 * vx, 10 * vx, 15 * vx, 10 * vx, 5 * vx, 5 * vx, 14 * vx };
			float oy[19] = { vy, vy, 2 * vy, 2 * vy, vy, 2 * vy, 3 * vy, 2 * vy, 2 * vy, vy, 2 * vy, 2 * vy, 3 * vy, vy, 2 * vy, vy, 2 * vy, 2 * vy, vy };
			for (int i = 0; i < 19; i++) {
				glBegin(GL_QUADS);
				glTexCoord2f(ox[i], oy[i]);	glVertex2i(GAME_WIDTH* (0.1f + 0.04f*i), -2 * GAME_HEIGHT + GAME_HEIGHT*0.42f);
				glTexCoord2f(ox[i] + 0.0625f, oy[i]);	glVertex2i(GAME_WIDTH* (0.1f + 0.07f + 0.04f*i), -2 * GAME_HEIGHT + GAME_HEIGHT*0.42f);
				glTexCoord2f(ox[i] + 0.0625f, oy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.1f + 0.07f + 0.04f*i), -2 * GAME_HEIGHT + GAME_HEIGHT*(0.42f + 0.07f));
				glTexCoord2f(ox[i], oy[i] - 0.16666f);	glVertex2i(GAME_WIDTH* (0.1f + 0.04f*i), -2 * GAME_HEIGHT + GAME_HEIGHT*(0.42f + 0.07f));
				glEnd();
			}

			glColor4f(1, 1, 1, 1);
			glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_BLONDE));

			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);	glVertex2i(GAME_WIDTH* (0.4f), -2 * GAME_HEIGHT + GAME_HEIGHT*0.15f);
			glTexCoord2f(1.0f, 1.0f);	glVertex2i(GAME_WIDTH* (0.4f + 0.2f), -2 * GAME_HEIGHT + GAME_HEIGHT*0.15f);
			glTexCoord2f(1.0f, 0.0f);	glVertex2i(GAME_WIDTH* (0.4f + 0.2f), -2 * GAME_HEIGHT + GAME_HEIGHT*(0.15f + 0.2f));
			glTexCoord2f(0.0f, 0.0f);	glVertex2i(GAME_WIDTH* (0.4f), -2 * GAME_HEIGHT + GAME_HEIGHT*(0.15f + 0.2f));
			glEnd();

			glDisable(GL_TEXTURE_2D);
			glPopMatrix();
		}
		++delay;
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

				ZonesDungeon[zone].Draw(x, y);

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

		if (Player.hasMinimap()) {
			glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_DUNGEON_MINIMAP));
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);	glVertex2i(GAME_WIDTH*0.065, GAME_HEIGHT*0.775f);
			glTexCoord2f(0.0f + 1.0f, 1.0f);	glVertex2i(GAME_WIDTH*0.34, GAME_HEIGHT*0.775f);
			glTexCoord2f(0.0f + 1.0f, 1.0f - 1.0f);	glVertex2i(GAME_WIDTH*0.34, GAME_HEIGHT*0.90);
			glTexCoord2f(0.0f, 1.0f - 1.0f);	glVertex2i(GAME_WIDTH*0.065, GAME_HEIGHT*0.90);
			glEnd();
		}

		glDisable(GL_TEXTURE_2D);

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
		glColor4f(1, 1, 1, 1);

		// Compass
		if (Player.hasCompass()) {
			zone = 11; // Triforce location
			zonex = (zone % (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_WIDTH;
			zoney = DUNGEON_MAP_HEIGHT - floor(zone / (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_HEIGHT + ZONE_HEIGHT / 2;
			pointx = (zonex + 8.0f - 1.5f) * (0.34*GAME_WIDTH - 0.065*GAME_WIDTH);
			pointy = (zoney - 11.0f - 2.5f) * (0.90*GAME_HEIGHT - 0.775*GAME_HEIGHT);
			pointx /= DUNGEON_MAP_WIDTH;
			pointy /= DUNGEON_MAP_HEIGHT;
			pointx += 0.065*GAME_WIDTH;
			pointy += 0.775*GAME_HEIGHT;
			if (delay % 40 < 20) glColor3f(0.8359f, 0.1641f, 0.0f);
			else glColor3f(0.055f, 0.212f, 0.431f);
			glRectf(pointx, pointy, pointx + MINIMAP_PLAYER_SIZE / 2, pointy + MINIMAP_PLAYER_SIZE / 2); // Red point
		}

		glColor4f(1, 1, 1, 1);
	}

	// HUD gold
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_ITEMS_HUD));
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

	int gold = Player.GetGold();
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

	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_ITEMS_HUD));
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

	int keys = Player.GetKeys();
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

	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_ITEMS_HUD));
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

	int bombs = Player.GetBombs();
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
	glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_ITEMS_HUD));

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
		glTexCoord2f(2.0f / 6.0f, 1.0f);	glVertex2i(GAME_WIDTH*0.5725f, GAME_HEIGHT*0.83f);
		glTexCoord2f(2.0f / 6.0f + 1.0f / 6.0f, 1.0f);	glVertex2i(GAME_WIDTH* (0.5725f + 0.06f), GAME_HEIGHT*0.83f);
		glTexCoord2f(2.0f / 6.0f + 1.0f / 6.0f, 1.0f - 0.25f);	glVertex2i(GAME_WIDTH* (0.5725f + 0.06f), GAME_HEIGHT*(0.83f + 0.06f));
		glTexCoord2f(2.0f / 6.0f, 1.0f - 0.25f);	glVertex2i(GAME_WIDTH*0.5725f, GAME_HEIGHT*(0.83f + 0.06f));
	glEnd();

	// Bomb
	if (Player.hasBoomerang()) {
		glBindTexture(GL_TEXTURE_2D, Data.GetID(IMG_OBJECTS));
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f / 7.0f, 1.0f);	glVertex2i(GAME_WIDTH*0.670f, GAME_HEIGHT*0.83f);
			glTexCoord2f(1.0f / 7.0f + 1.0f / 7.0f, 1.0f);	glVertex2i(GAME_WIDTH* (0.67f + 0.06f), GAME_HEIGHT*0.83f);
			glTexCoord2f(1.0f / 7.0f + 1.0f / 7.0f, 1.0f - 0.5f);	glVertex2i(GAME_WIDTH* (0.67f + 0.06f), GAME_HEIGHT*(0.83f + 0.06f));
			glTexCoord2f(1.0f / 7.0f, 1.0f - 0.5f);	glVertex2i(GAME_WIDTH*0.67f, GAME_HEIGHT*(0.83f + 0.06f));
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);

}

void cGame::initializeEnemiesOverworld() {
	ZonesOverworld[87] = cZone();
	ZonesOverworld[87].SetOverworld(true);
	ZonesOverworld[87].SetData(&Data);
	ZonesOverworld[87].addEnemy(118.0f, 81.0f, OCTOROCT, true, 87, true, &overworld);
}

void cGame::initializeEnemiesDungeons() {
	for (int i = 0; i < 34; ++i) {
		ZonesDungeon[i] = cZone();
		ZonesDungeon[i].SetOverworld(false);
		ZonesDungeon[i].SetData(&Data);
		switch (i) {
		case 1:
			// Traps & Stone unlocks -> Basement
			break;
		case 2:
			// Goriyas
			break;
		case 7:
			// Basement with the Bow
			break;
		case 8:
			ZonesDungeon[i].addEnemy(3.0f, 9.0f, STALFOS, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(10.0f, 12.0f, STALFOS, false, true, i, &overworld);
			ZonesDungeon[i].addEnemy(12.0f, 9.0f, STALFOS, false, false, i, &overworld);
			break;
		case 10:
			// BOSS AREA
			ZonesDungeon[i].addEnemy(11.0f, 11.0f, BOSS, true, false, i, &overworld);
			break;
		case 12:
			// EASTMOST PENNINSLA IS THE SECRET.
			break;
		case 13:
			// Gels & Stone unlocks left door
			break;
		case 14:
			// Gels & Map
			break;
		case 15:
			// Goriyas & Boomerang
			break;
		case 16:
			// Key & Wall Masters
			break;
		case 19:
			// Killing al enemies unlocks right door
			ZonesDungeon[i].addEnemy(2.0f, 11.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(3.0f, 13.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(5.0f, 10.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(5.0f, 12.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(10.0f, 8.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(10.0f, 14.0f, KEESE, false, false, i, &overworld);
			break;
		case 20:
			ZonesDungeon[i].addEnemy(5.0f, 10.0f, STALFOS, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(6.0f, 12.0f, STALFOS, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(8.0f, 11.0f, STALFOS, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(8.0f, 10.0f, STALFOS, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(10.0f, 8.0f, STALFOS, false, false, i, &overworld);
			break;
		case 21:
			// Compass to triforce
			ZonesDungeon[i].addEnemy(5.0f, 8.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(8.0f, 10.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(8.0f, 12.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(10.0f, 10.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(10.0f, 12.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(12.0f, 9.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(12.0f, 13.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(13.0f, 11.0f, KEESE, false, false, i, &overworld);
			break;
		case 26:
			ZonesDungeon[i].addEnemy(5.0f, 9.0f, STALFOS, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(5.0f, 12.0f, STALFOS, false, true, i, &overworld);
			ZonesDungeon[i].addEnemy(6.0f, 10.0f, STALFOS, false, false, i, &overworld);
			break;
		case 31:
			ZonesDungeon[i].addEnemy(3.1f, 9.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(2.1f, 11.0f, KEESE, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(10.0f, 14.0f, KEESE, false, false, i, &overworld);
			break;
		case 33:
			ZonesDungeon[i].addEnemy(5.0f, 8.0f, STALFOS, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(5.0f, 14.0f, STALFOS, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(8.0f, 10.0f, STALFOS, false, false, i, &overworld);
			ZonesDungeon[i].addEnemy(8.0f, 12.0f, STALFOS, false, true, i, &overworld);
			ZonesDungeon[i].addEnemy(13.0f, 11.0f, STALFOS, false, false, i, &overworld);
			break;
		}
	}
}

void cGame::initializeObjectsOverworld() {
	
}

void cGame::initializeObjectsDungeons() {
	for (int i = 0; i < 34; ++i) {
		switch (i) {
		case 1:
			// Traps & Stone unlocks -> Basement
			break;
		case 2:
			// Goriyas & killing one drops KEY in 8.0, 9.0
			ZonesDungeon[i].addObject(8.0f, 9.0f, KEY, false, i, &overworld);
			break;
		case 7:
			// Basement with the Bow
			break;
		case 8:
			ZonesDungeon[i].addObject(13.0f, 11.0f, KEY, true, i, &overworld);
			break;
		case 10:
			// BOSS AREA
			break;
		case 11:
			// TRIFORCE AREA
			ZonesDungeon[i].addObject(8.0f, 11.0f, TRIFORCE, true, i, &overworld);
			break;
		case 12:
			// EASTMOST PENNINSLA IS THE SECRET.
			break;
		case 13:
			// Gels & Stone unlocks left door
			break;
		case 14:
			// Gels & Map
			ZonesDungeon[i].addObject(12.0f, 11.0f, MAP, true, i, &overworld);
			break;
		case 15:
			// Goriyas & Boomerang
			ZonesDungeon[i].addObject(8.0f, 9.0f, BOOMERANG, true, i, &overworld);
			break;
		case 16:
			// Key & Wall Masters & one of them drops
			ZonesDungeon[i].addObject(10.0f, 14.0f, KEY, true, i, &overworld);
			break;
		case 19:
			// Killing al enemies unlocks right door
			break;
		case 20:
			ZonesDungeon[i].addObject(8.0f, 9.0f, KEY, false, i, &overworld);
			break;
		case 21:
			// Compass to triforce
			ZonesDungeon[i].addObject(12.0f, 11.0f, COMPASS, true, i, &overworld);
			break;
		case 26:
			break;
		case 31:
			ZonesDungeon[i].addObject(10.0f, 14.0f, KEY, false, i, &overworld);
			break;
		case 33:
			ZonesDungeon[i].addObject(13.0f, 13.0f, KEY, false, i, &overworld);
			break;
		}
	}
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
	delay = 0;
	if (overworld) Data.stopMusic(MUSIC_OVERWORLD);
	else Data.stopMusic(MUSIC_DUNGEON);
	Data.playMusic(MUSIC_GAMEOVER);
}

void cGame::setZones() {
	float x, y;
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
}

bool cGame::initializeAudio() {
	if (!Data.LoadSound(SOUND_SWORD, "../sounds/sword-sound.wav")) return false;
	if (!Data.LoadSound(SOUND_ITEM, "../sounds/item-sound.wav")) return false;
	if (!Data.LoadSound(SOUND_KEY, "../sounds/key-sound.wav")) return false;
	if (!Data.LoadSound(SOUND_HIT, "../sounds/hit-sound.wav")) return false;
	if (!Data.LoadSound(SOUND_COIN, "../sounds/coin-sound.wav")) return false;
	if (!Data.LoadSound(SOUND_DOOR, "../sounds/door-sound.wav")) return false;
	if (!Data.LoadSound(SOUND_WIN, "../sounds/win-sound.wav")) return false;
	if (!Data.LoadMusic(MUSIC_OVERWORLD, "../sounds/Overworld.ogg")) return false;
	if (!Data.LoadMusic(MUSIC_DUNGEON, "../sounds/dungeon-music.ogg")) return false;
	if (!Data.LoadMusic(MUSIC_GAMEOVER, "../sounds/gameover-music.ogg")) return false;
	if (!Data.LoadMusic(MUSIC_WIN, "../sounds/win-music.ogg")) return false;
	if (!Data.LoadMusic(MUSIC_LOVE, "../sounds/what-is-love-music.ogg")) return false;
	Data.playMusic(MUSIC_OVERWORLD);
	return true;
}