#include "include/cPlayer.h"

cPlayer::cPlayer() {}

cPlayer::cPlayer(bool * overworld, bool * tr) {
	setOverworld(overworld);
	transition = tr;
	lives = LINK_LIVES;
	SetKnocked(false);
}

cPlayer::~cPlayer(){}

void cPlayer::Logic(int *map) {
	int tile, newx, newy, zone, state;
	GetZone(&zone);

	float posx, posy;
	GetPosition(&posx, &posy);
	newx = floor(posx);
	newy = ceil(posy);

	bool knocked;
	GetKnocked(&knocked);

	if (*transition) {
		state = GetState();

		switch (GetState()) {
			case STATE_WALKLEFT:
				if (offset == GAME_WIDTH && posx <= -2.0f) {
					*transition = false;
					SetZone(zone - 1);
					SetPosition(SCENE_WIDTH - 2.0f, posy);
				}
				else if (offset == GAME_WIDTH) SetPosition(posx - 0.05f, posy);
				else offset += GAME_WIDTH / 175;
				break;
			case STATE_WALKRIGHT:
				if (offset == -GAME_WIDTH && posx >= SCENE_WIDTH - 1 + 2.0f) {
					*transition = false;
					SetZone(zone + 1);
					SetPosition(1.0f, posy);
				}
				else if (offset == -GAME_WIDTH) SetPosition(posx + 0.05f, posy);
				else offset -= GAME_WIDTH / 175;
				break;
			case STATE_WALKDOWN:
				if (offset == GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1)) && posy >= SCENE_HEIGHT + 2.0f) {
					*transition = false;
					SetZone(zone + (DUNGEON_MAP_WIDTH / ZONE_WIDTH));
					SetPosition(posx, HUD_TILES + 1 + 2.0f);
				}
				else if (offset == GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1))) SetPosition(posx, posy + 0.05f);
				else offset += (GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1))) / 175;
				break;
			case STATE_WALKUP:
				if (offset == GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1)) && posy <= -1.2f + HUD_TILES + 1) {
					*transition = false;
					SetZone(zone - (DUNGEON_MAP_WIDTH / ZONE_WIDTH));
					SetPosition(posx, SCENE_HEIGHT - 1.2f);
				}
				else if (offset == GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1))) SetPosition(posx, posy - 0.05f);
				else offset += (GAME_HEIGHT - (HUD_TILES*GAME_HEIGHT / (SCENE_HEIGHT - 1))) / 175;
				break;
		}

		return;
	} else if (knocked) {
		if (state_delay >= MAX_STATE_DELAY_LINK) StopState();
		else {
			std::vector<int> tiles;

			float newx = posx - 0.2f;
			if ((state != STATE_LOOKLEFT || state != STATE_WALKLEFT) && checkCorrectMovementOverworld(newx, posy, map, STATE_WALKLEFT)) tiles.push_back(0);
			newx = posx + 0.2f;
			if ((state != STATE_LOOKRIGHT || state != STATE_WALKRIGHT) && checkCorrectMovementOverworld(newx, posy, map, STATE_WALKRIGHT)) tiles.push_back(1);
			newx = posx + 0.2f;
			float newy = posy - 0.2f;
			if ((state != STATE_LOOKUP || state != STATE_WALKUP) && checkCorrectMovementOverworld(newx, newy, map, STATE_WALKUP)) tiles.push_back(2);
			newy = posy + 0.2f;
			if ((state != STATE_LOOKDOWN || state != STATE_WALKDOWN) && checkCorrectMovementOverworld(newx, newy, map, STATE_WALKDOWN)) tiles.push_back(3);

			int i = rand() % tiles.size();

			// Discomment this shit to make them dance!
			switch (tiles[i]) {
				case 0: SetPosition(posx - 0.2f, posy); break;
				case 1: SetPosition(posx + 0.2f, posy); break;
				case 2: SetPosition(posx + 0.1f, posy - 0.2f); break;
				case 3: SetPosition(posx + 0.1f, posy + 0.2f); break;
			}

			state_delay += 0.1f;
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
				zoney = floor(zone / (DUNGEON_MAP_WIDTH / ZONE_WIDTH)) * ZONE_HEIGHT + ZONE_HEIGHT / 2 - 1;
				tile = map[(DUNGEON_MAP_HEIGHT - zoney + (ZONE_HEIGHT - newy) - 1)*DUNGEON_MAP_WIDTH + zonex + newx];
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
		case STATE_LOOKLEFT:	xo = 0.25f;	yo = 0.25f;
								break;
		
		case STATE_LOOKRIGHT:	xo = 0.75f;	yo = 0.25f;
								break;

		case STATE_LOOKUP:		xo = 0.5f;	yo = 0.25f;
								break;
			
		case STATE_LOOKDOWN:	xo = 0.0f;	yo = 0.25f;
								break;

		case STATE_WALKLEFT:	xo = 0.25f;	yo = 0.25f + (GetFrame()*0.25f);
								NextFrame(2);
								break;
		case STATE_WALKRIGHT:	xo = 0.75f; yo = 0.25f + (GetFrame()*0.25f);
								NextFrame(2);
								break;
		case STATE_WALKUP:		xo = 0.5f;	yo = 0.25f + (GetFrame()*0.25f);
								NextFrame(2);
								break;
		case STATE_WALKDOWN:	xo = 0.0f; yo = 0.25f + (GetFrame()*0.25f);
								NextFrame(2);
								break;
		case ATTACK_UP:			xo = 0.5f; yo = 0.75f;
								SetState(STATE_LOOKUP);
								SetSeqNDelay(1, 0);
								DrawWeapon(tex_id, xo, yo + 0.25f, xo + 0.25f, yo, 0.0f, OFFSET_WEAPON);
								break;
		case ATTACK_LEFT:		xo = 0.25f; yo = 0.75f;
								SetState(STATE_LOOKLEFT);
								SetSeqNDelay(1, 0);
								DrawWeapon(tex_id, xo, yo + 0.25f, xo + 0.25f, yo, -OFFSET_WEAPON, 0.0f);
								break;
		case ATTACK_RIGHT:		xo = 0.75f; yo = 0.75f;
								SetState(STATE_LOOKRIGHT);
								SetSeqNDelay(1, 0);
								DrawWeapon(tex_id, xo, yo + 0.25f, xo + 0.25f, yo, OFFSET_WEAPON, 0.0f);
								break;
		case ATTACK_DOWN:		xo = 0.0f; yo = 0.75f;
								SetState(STATE_LOOKDOWN);
								SetSeqNDelay(1, 0);
								DrawWeapon(tex_id, xo, yo+0.25f, xo+0.25f, yo,0.0f,-OFFSET_WEAPON);
								break;
	}
	xf = xo + 0.25f;
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
	glBegin(GL_QUADS);
	glTexCoord2f(xo, yo);	glVertex2i(screen_x, screen_y);
	glTexCoord2f(xf, yo);	glVertex2i(screen_x + (vx / TILE_SIZE)*w, screen_y);
	glTexCoord2f(xf, yf);	glVertex2i(screen_x + (vx / TILE_SIZE)*w, screen_y + (vy / TILE_SIZE)*h);
	glTexCoord2f(xo, yf);	glVertex2i(screen_x, screen_y + (vy / TILE_SIZE)*h);
	glEnd();

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

bool cPlayer::checkIntersections(cZone zone) {
	cOctoroct * enemies= zone.GetEnemies();
	float posx, posy, x, y;
	GetPosition(&x, &y);
	cRec player = cRec(x, y);
	cRec enemy;
	for (int i = 0; i < zone.numEnemies(); ++i) {
		(enemies[i]).GetPosition(&posx, &posy);
		if (player.intersects(cRec(posx, posy))) {
			if (DEBUG_MODE) std::cout << "player hit!!!!" << std::endl;
			SetKnocked(true);
			lives -= 0.5f;
			return false;
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

void cPlayer::StopState() {
	state_delay = 0.0f;
	SetKnocked(false);
	Stop();
}