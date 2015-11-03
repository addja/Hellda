#include "include/cPlayer.h"

cPlayer::cPlayer(){}

cPlayer::~cPlayer(){}

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
								DrawWeapon(tex_id, xo, yo + 0.25f, xo + 0.25f, yo, 0.0f, 0.8f);
								break;
		case ATTACK_LEFT:		xo = 0.25f; yo = 0.75f;
								SetState(STATE_LOOKLEFT);
								SetSeqNDelay(1, 0);
								DrawWeapon(tex_id, xo, yo + 0.25f, xo + 0.25f, yo, -0.8f, 0.0f);
								break;
		case ATTACK_RIGHT:		xo = 0.75f; yo = 0.75f;
								SetState(STATE_LOOKRIGHT);
								SetSeqNDelay(1, 0);
								DrawWeapon(tex_id, xo, yo + 0.25f, xo + 0.25f, yo, 0.8f, 0.0f);
								break;
		case ATTACK_DOWN:		xo = 0.0f; yo = 0.75f;
								SetState(STATE_LOOKDOWN);
								SetSeqNDelay(1, 0);
								DrawWeapon(tex_id, xo, yo+0.25f, xo+0.25f, yo,0.0f,-0.8f);
								break;
	}
	xf = xo + 0.25f;
	yf = yo - 0.25f;
	
	DrawRect(tex_id, xo, yo, xf, yf);
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