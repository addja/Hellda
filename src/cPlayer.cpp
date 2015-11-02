#include "include/cPlayer.h"

cPlayer::cPlayer() {}

cPlayer::~cPlayer(){}

void cPlayer::Draw(int tex_id) {	
	float xo,yo,xf,yf;

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

	}
	xf = xo + 0.25f;
	yf = yo - 0.25f;

	bool debug = 0; // put to 1 to debug
	DrawRect(tex_id,xo,yo,xf,yf, debug);
}
