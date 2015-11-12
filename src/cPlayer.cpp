#include "include/cPlayer.h"

cPlayer::cPlayer() {}

cPlayer::cPlayer(bool * overworld, bool * tr, bool * opening) {
	setOverworld(overworld);
	setOpening(opening);
	sword_swipe = false;
	transition = tr;
	lives = LINK_LIVES;
	gold = 0;
	keys = 0;
	bombs = 0;
	SetKnocked(false);
	state_delay = -1;
	offset = 0.0f;
}

cPlayer::~cPlayer(){}

void cPlayer::Logic(int *map) {

	int tile, newx, newy, zone, state;
	GetZone(&zone);
	state = GetState();

	float posx, posy;
	GetPosition(&posx, &posy);
	newx = floor(posx);
	newy = ceil(posy);

	bool knocked;
	GetKnocked(&knocked);

	if (*transition) {

		switch (GetState()) {
			case STATE_WALKLEFT:
				if (offset == GAME_WIDTH && posx <= -2.0f) {
					*transition = false;
					SetZone(zone - 1);
					SetPosition(SCENE_WIDTH - 2.0f, posy);
					offset = 0.0f;
				}
				else if (offset == GAME_WIDTH || posx > 0.0f) SetPosition(posx - 0.05f, posy);
				else if (offset + GAME_WIDTH / TRANSITION_SPEED > GAME_WIDTH) offset = GAME_WIDTH;
				else offset += GAME_WIDTH / TRANSITION_SPEED;
				break;
			case STATE_WALKRIGHT:
				if (offset == -GAME_WIDTH && posx >= SCENE_WIDTH - 1 + 2.0f) {
					*transition = false;
					SetZone(zone + 1);
					SetPosition(1.0f, posy);
					offset = 0.0f;
				}
				else if (offset == -GAME_WIDTH || posx < SCENE_WIDTH - 1) SetPosition(posx + 0.05f, posy);
				else if (offset - GAME_WIDTH / TRANSITION_SPEED < -GAME_WIDTH) offset = -GAME_WIDTH;
				else offset -= GAME_WIDTH / TRANSITION_SPEED;
				break;
			case STATE_WALKDOWN:
				if (offset == GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1)) && posy >= SCENE_HEIGHT + 2.0f) {
					*transition = false;
					SetZone(zone + (DUNGEON_MAP_WIDTH / ZONE_WIDTH));
					SetPosition(posx, HUD_TILES + 1 + 2.0f);
					offset = 0.0f;
				}
				else if (offset == GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1)) || posy < SCENE_HEIGHT) SetPosition(posx, posy + 0.05f);
				else if (offset + (GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1))) / TRANSITION_SPEED > GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1))) offset = GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1));
				else offset += (GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1))) / TRANSITION_SPEED;
				break;
			case STATE_WALKUP:
				if (offset >= GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1)) && posy <= -1.2f + HUD_TILES + 1) {
					*transition = false;
					SetZone(zone - (DUNGEON_MAP_WIDTH / ZONE_WIDTH));
					SetPosition(posx, SCENE_HEIGHT - 1.2f);
					offset = 0.0f;
				}
				else if (offset >= GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1)) || posy > HUD_TILES + 1) SetPosition(posx, posy - 0.05f);
				else if (offset + (GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1))) / TRANSITION_SPEED > GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1))) offset = GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1));
				else offset += (GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1))) / TRANSITION_SPEED;
				break;
		}

		return;
	} else if (knocked) {
		if (state_delay >= MAX_STATE_DELAY_LINK) StopState();
		else {
			// knock back
			int dir;
			float movx, movy;
			movx = movy = 0;
			switch (state) {
				case STATE_LOOKDOWN:
				case STATE_WALKDOWN:
					dir = STATE_WALKUP;
					movy = hit_diry * STEP_LENGTH_LINK * 2;
					break;
				case STATE_LOOKUP:
				case STATE_WALKUP:
					dir = STATE_WALKDOWN;
					movy = hit_diry * STEP_LENGTH_LINK * 2;
					break;
				case STATE_LOOKLEFT:
				case STATE_WALKLEFT:
					dir = STATE_WALKRIGHT;
					movx = hit_dirx * STEP_LENGTH_LINK * 2;
					break;
				case STATE_LOOKRIGHT:
				case STATE_WALKRIGHT:
					dir = STATE_WALKLEFT;
					movx = hit_dirx * STEP_LENGTH_LINK * 2;
					break;
			}

			// check if good movement, if not stay stopped
			if (inOverworld()) {
				if (!checkCorrectMovementOverworld(posx + movx, posy + movy, map, dir, true)) {
					movx = movy = 0;
				}
			}
			else {
				if (!checkCorrectMovementDungeon(posx + movx, posy + movy, map, dir, true)) {
					movx = movy = 0;
				}
			}

			SetPosition(posx + movx, posy + movy);
			state_delay += 0.5f;
		}
	}

	// check for portals
	if (inOverworld()) {
		tile = map[(OVERWORLD_MAP_HEIGHT - newy - 1)*OVERWORLD_MAP_WIDTH + newx];
		if (overworldTransitions(tile)) {
			changeOverworld();
			SetPosition(SCENE_WIDTH / 2 - 0.5f, SCENE_HEIGHT - 2.0f);
			SetZone(32);
		}
	}
	else {
		int zonex, zoney;
		GetZone(&zone);
		state = GetState();

		switch (GetState()) {
			case STATE_LOOKDOWN:
			case STATE_WALKDOWN:
				zonex = (zone % (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_WIDTH;
				zoney = floor(zone / (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_HEIGHT + ZONE_HEIGHT / 2;
				tile = map[(DUNGEON_MAP_HEIGHT - zoney + (ZONE_HEIGHT - newy) - 1)*DUNGEON_MAP_WIDTH + zonex + newx];
				std::cout << newx << " " << newy << " " << tile << std::endl;
				if (dungeonDownTransitions(tile)) {
					std::cout << "DOWN TRANS " << tile << std::endl;
					if ((zonex + newx == 39.0f || zonex + newx == 40.0f) && zoney - (ZONE_HEIGHT - newy) == 64.0f) {
						SetPosition(116.0f, 79.0f);
						changeOverworld();
						return;
					}
					else {
						Transition();
						SetPosition(SCENE_WIDTH / 2 - 0.5f, posy + 0.05f);
						return;
					}
				}
				break;
			case STATE_LOOKUP:
			case STATE_WALKUP:
				zonex = (zone % (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_WIDTH + 1;
				zoney = floor(zone / (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_HEIGHT + ZONE_HEIGHT / 2;
				tile = map[(DUNGEON_MAP_HEIGHT - zoney + (ZONE_HEIGHT - newy) - 1)*DUNGEON_MAP_WIDTH + zonex + newx];
				if (dungeonUpTransitions(tile)) {
					std::cout << "UP TRANS " << tile << std::endl;
					Transition();
					SetPosition(SCENE_WIDTH / 2 - 0.5f, posy - 0.05f);
					return;
				}
				break;
			case STATE_LOOKLEFT:
			case STATE_WALKLEFT:
				zonex = (zone % (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_WIDTH;
				zoney = floor(zone / (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_HEIGHT + ZONE_HEIGHT / 2;
				tile = map[(DUNGEON_MAP_HEIGHT - zoney + (ZONE_HEIGHT - newy) - 1)*DUNGEON_MAP_WIDTH + zonex + newx];
				//std::cout << tile << " " << newx << " " << newy << std::endl;
				if (dungeonLeftTransitions(tile)) {
					std::cout << "LEFT TRANS " << tile << " " << newx << " " << newy << std::endl;
					Transition();
					SetPosition(posx - 0.05f, SCENE_HEIGHT - 5.0f);
					return;
				}
				tile = map[(DUNGEON_MAP_HEIGHT - zoney + (ZONE_HEIGHT - newy) - 1)*DUNGEON_MAP_WIDTH + zonex + newx + 1];
				if (dungeonLeftTransitions(tile)) {
					std::cout << "LEFT TRANS " << tile << " " << newx << " " << newy << std::endl;
					Transition();
					SetPosition(posx - 0.05f, SCENE_HEIGHT - 5.0f);
					return;
				}
				break;
			case STATE_LOOKRIGHT:
			case STATE_WALKRIGHT:
				zonex = (zone % (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_WIDTH;
				zoney = floor(zone / (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_HEIGHT + ZONE_HEIGHT / 2;
				tile = map[(DUNGEON_MAP_HEIGHT - zoney + (ZONE_HEIGHT - newy) - 1)*DUNGEON_MAP_WIDTH + zonex + newx];
				if (dungeonRightTransitions(tile)) {
					std::cout << "RIGHT TRANS " << tile << std::endl;
					Transition();
					SetPosition(posx + 0.05f, SCENE_HEIGHT - 5.0f);
				}
				tile = map[(DUNGEON_MAP_HEIGHT - zoney + (ZONE_HEIGHT - newy) - 1)*DUNGEON_MAP_WIDTH + zonex + newx + 1];
				if (dungeonRightTransitions(tile)) {
					std::cout << "RIGHT TRANS " << tile << std::endl;
					Transition();
					SetPosition(posx + 0.05f, SCENE_HEIGHT - 5.0f);
				}
				break;
			}
	}
}

void cPlayer::Draw(int tex_id) {	
	float xo,yo,xf,yf;

	// to write a weapon
	float posx, posy;
	GetPosition(&posx, &posy);

	switch (GetState()) {
		case STATE_LOOKLEFT:	xo = 1.0f / 5.0f;	yo = 0.25f;
								break;

		case STATE_LOOKRIGHT:	xo = 3.0f / 5.0f;	yo = 0.25f;
								break;

		case STATE_LOOKUP:		xo = 2.0f / 5.0f;	yo = 0.25f;
								break;

		case STATE_LOOKDOWN:	xo = 0.0f;	yo = 0.25f;
								break;

		case STATE_WALKLEFT:	xo = 1.0f / 5.0f;	yo = 0.25f + (GetFrame()*0.25f);
								NextFrame(2);
								break;
		case STATE_WALKRIGHT:	xo = 3.0f / 5.0f; yo = 0.25f + (GetFrame()*0.25f);
								NextFrame(2);
								break;
		case STATE_WALKUP:		xo = 2.0f / 5.0f;	yo = 0.25f + (GetFrame()*0.25f);
								NextFrame(2);
								break;
		case STATE_WALKDOWN:	xo = 0.0f; yo = 0.25f + (GetFrame()*0.25f);
								NextFrame(2);
								break;
		case ATTACK_UP:			xo = 2.0f / 5.0f; yo = 0.75f;
								SetState(STATE_LOOKUP);
								SetSeqNDelay(1, 0);
								DrawWeapon(tex_id, xo, yo + 0.25f, xo + 0.2f, yo, 0.0f, OFFSET_WEAPON);
								break;
		case ATTACK_LEFT:		xo = 1.0f / 5.0f; yo = 0.75f;
								SetState(STATE_LOOKLEFT);
								SetSeqNDelay(1, 0);
								DrawWeapon(tex_id, xo, yo + 0.25f, xo + 0.2f, yo, -OFFSET_WEAPON, 0.0f);
								break;
		case ATTACK_RIGHT:		xo = 3.0f / 5.0f; yo = 0.75f;
								SetState(STATE_LOOKRIGHT);
								SetSeqNDelay(1, 0);
								DrawWeapon(tex_id, xo, yo + 0.25f, xo + 0.2f, yo, OFFSET_WEAPON, 0.0f);
								break;
		case ATTACK_DOWN:		xo = 0.0f; yo = 0.75f;
								SetState(STATE_LOOKDOWN);
								SetSeqNDelay(1, 0);
								DrawWeapon(tex_id, xo, yo + 0.25f, xo + 0.2f, yo, 0.0f, -OFFSET_WEAPON);
								break;
		case STATE_LOOKBOOM:	xo = 4.0f / 5.0f; yo = 0.25f;
								break;
		case STATE_BOOM:		xo = 4.0f / 5.0f; yo = 0.25f + (GetFrame()*0.25f);
								NextFrame(4);
								break;
	}
	xf = xo + 0.2f;
	yf = yo - 0.25f;
	
	DrawPlayer(tex_id, xo, yo, xf, yf);
}

void cPlayer::Attack() {

	if (GetState() == STATE_WALKDOWN || GetState() == STATE_LOOKDOWN) {
		SetState(ATTACK_DOWN);
		SetSeqNDelay(1, 0);
	}
	else if (GetState() == STATE_WALKUP || GetState() == STATE_LOOKUP) {
		SetState(ATTACK_UP);
		SetSeqNDelay(1, 0);
	}
	else if (GetState() == STATE_WALKLEFT || GetState() == STATE_LOOKLEFT) {
		SetState(ATTACK_LEFT);
		SetSeqNDelay(1, 0);
	}
	else if (GetState() == STATE_WALKRIGHT || GetState() == STATE_LOOKRIGHT) {
		SetState(ATTACK_RIGHT);
		SetSeqNDelay(1, 0);
	}
}

void cPlayer::DrawPlayer(int tex_id, float xo, float yo, float xf, float yf) {
	float screen_x, screen_y;

	float x, y;
	int w, h;
	GetPosition(&x, &y);
	GetWidthHeight(&w, &h);

	float vx = GAME_WIDTH / SCENE_WIDTH;
	float vy = GAME_HEIGHT / (SCENE_HEIGHT - 1);

	if (inOverworld()) {

		screen_x = GAME_WIDTH / 2 + SCENE_Xo;
		screen_y = (((SCENE_HEIGHT - HUD_TILES) / 2) - 1)*vy + SCENE_Yo;

		if (x < SCENE_WIDTH / 2) screen_x = x*vx;
		else if (x > OVERWORLD_MAP_WIDTH - (SCENE_WIDTH / 2)) screen_x = GAME_WIDTH - (OVERWORLD_MAP_WIDTH - x)*vx;
		if (y <= (SCENE_HEIGHT - HUD_TILES) / 2) screen_y += ((SCENE_HEIGHT - HUD_TILES) / 2 - y)*vy;
		else if (y >= OVERWORLD_MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2) screen_y -= (y - (OVERWORLD_MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2))*vy;
	} else {
		// Draw player in Dungeon
		screen_x = x*vx;
		screen_y = (SCENE_HEIGHT - y)*vy;
	}

	if (DEBUG_MODE) {
		glColor3f(1.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2i(screen_x, screen_y);
		glVertex2i(screen_x + (vx / TILE_SIZE)*w, screen_y);
		glVertex2i(screen_x + (vx / TILE_SIZE)*w, screen_y + (vy / TILE_SIZE)*h);
		glVertex2i(screen_x, screen_y + (vy / TILE_SIZE)*h);
		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
	}

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, tex_id);

	bool knocked;
	GetKnocked(&knocked);
	if (knocked) glColor3f(0.4f, 0.1f, 0.1f);
	glBegin(GL_QUADS);
	glTexCoord2f(xo, yo);	glVertex2i(screen_x, screen_y);
	glTexCoord2f(xf, yo);	glVertex2i(screen_x + (vx / TILE_SIZE)*w, screen_y);
	glTexCoord2f(xf, yf);	glVertex2i(screen_x + (vx / TILE_SIZE)*w, screen_y + (vy / TILE_SIZE)*h);
	glTexCoord2f(xo, yf);	glVertex2i(screen_x, screen_y + (vy / TILE_SIZE)*h);
	glEnd();
	if (knocked) glColor3f(1.0f, 1.0f, 1.0f);

	glDisable(GL_TEXTURE_2D);
}

// Draws an object that follows the associated Pepe with a given offset
void cPlayer::DrawWeapon(int tex_id, float xo, float yo, float xf, float yf, float offsetx, float offsety) {
	
	float screen_x, screen_y;
	float x, y;
	int w, h;
	GetWidthHeight(&w, &h);
	GetPosition(&x, &y);

	float vx = GAME_WIDTH / SCENE_WIDTH;
	float vy = GAME_HEIGHT / (SCENE_HEIGHT - 1);

	if (inOverworld()) {
		screen_x = GAME_WIDTH / 2 + SCENE_Xo;
		screen_y = (((SCENE_HEIGHT - HUD_TILES) / 2) - 1)*vy + SCENE_Yo;

		if (x < SCENE_WIDTH / 2) screen_x = x*vx;
		else if (x > OVERWORLD_MAP_WIDTH - (SCENE_WIDTH / 2)) screen_x = GAME_WIDTH - (OVERWORLD_MAP_WIDTH - x)*vx;
		if (y <= (SCENE_HEIGHT - HUD_TILES) / 2) screen_y += ((SCENE_HEIGHT - HUD_TILES) / 2 - y)*vy;
		else if (y >= OVERWORLD_MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2) screen_y -= (y - (OVERWORLD_MAP_HEIGHT - (SCENE_HEIGHT - HUD_TILES) / 2))*vy;
	} else {
		// Draw weapon in Dungeon
		screen_x = x*vx;
		screen_y = (SCENE_HEIGHT - y)*vy;
	}

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, tex_id);
	glBegin(GL_QUADS);
	glTexCoord2f(xo, yo);	glVertex2i(screen_x + vx*offsetx, screen_y + vy*offsety);
	glTexCoord2f(xf, yo);	glVertex2i(screen_x + (vx / TILE_SIZE)*w + vx*offsetx, screen_y + vy*offsety);
	glTexCoord2f(xf, yf);	glVertex2i(screen_x + (vx / TILE_SIZE)*w + vx*offsetx, screen_y + (vy / TILE_SIZE)*h + vy*offsety);
	glTexCoord2f(xo, yf);	glVertex2i(screen_x + vx*offsetx, screen_y + (vy / TILE_SIZE)*h + vy*offsety);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void cPlayer::Transition() {
	*transition = true;
	offset = 0.0f;
}

void cPlayer::GetOffset(float * off) {
	*off = offset;
}

void cPlayer::SetOffset(float off) {
	offset = off;
}

bool cPlayer::checkIntersections(cZone zone) {
	float posx, posy, x, y;
	GetPosition(&x, &y);
	cRec player = cRec(x, y);

	for (int i = 0; i < zone.numOctorocs(); ++i) {
		cOctoroct * octoroc = zone.GetOctoroc(i);
		if (!(*octoroc).isDead()) {
			(*octoroc).GetPosition(&posx, &posy);
			if (player.intersects(cRec(posx, posy))) {
				if (posx < x) hit_dirx = 1;
				else hit_dirx = -1;
				if (posy < y) hit_diry = 1;
				else hit_diry = -1;
				if (DEBUG_MODE) std::cout << "player hit!!!!" << std::endl;
				SetKnocked(true);
				lives -= 0.5f;
				return false;
			}
		}
	}

	for (int i = 0; i < zone.numKeeses(); ++i) {
		cKeese * keese = zone.GetKeese(i);
		if (!(*keese).isDead()) {
			(*keese).GetPosition(&posx, &posy);
			if (player.intersects(cRec(posx, posy))) {
				if (posx < x) hit_dirx = 1;
				else hit_dirx = -1;
				if (posy < y) hit_diry = 1;
				else hit_diry = -1;
				if (DEBUG_MODE) std::cout << "player hit!!!!" << std::endl;
				SetKnocked(true);
				lives -= 0.5f;
				return false;
			}
		}
	}

	for (int i = 0; i < zone.numStalfos(); ++i) {
		cStalfos * stalfos = zone.GetStalfos(i);
		if (!(*stalfos).isDead()) {
			(*stalfos).GetPosition(&posx, &posy);
			if (player.intersects(cRec(posx, posy))) {
				if (posx < x) hit_dirx = 1;
				else hit_dirx = -1;
				if (posy < y) hit_diry = 1;
				else hit_diry = -1;
				if (DEBUG_MODE) std::cout << "player hit!!!!" << std::endl;
				SetKnocked(true);
				lives -= 0.5f;
				return false;
			}
		}
	}

	cBoss * boss = zone.GetBoss();
	if (zone.isBossArea()) {
		if (!(*boss).isDead()) {
			(*boss).GetPosition(&posx, &posy);
			if (player.intersects(cRec(posx, posy))) {
				if (posx < x) hit_dirx = 1;
				else hit_dirx = -1;
				if (posy < y) hit_diry = 1;
				else hit_diry = -1;
				if (DEBUG_MODE) std::cout << "player hit!!!!" << std::endl;
				SetKnocked(true);
				lives -= 0.5f;
				return false;
			}
		}
	}
	return true;
}

bool cPlayer::isDead() {
	return lives == 0;
}

float cPlayer::health() {
	return lives;
}

int cPlayer::GetGold() {
	return gold;
}

int cPlayer::GetKeys() {
	return keys;
}

int cPlayer::GetBombs() {
	return bombs;
}

void cPlayer::StopState() {
	state_delay = 0.0f;
	SetKnocked(false);
	Stop();
}

bool cPlayer::swipeAgain() {
	return !sword_swipe;
}

void cPlayer::swipe() {
	sword_swipe = true;
}

void cPlayer::endSwipe() {
	sword_swipe = false;
}