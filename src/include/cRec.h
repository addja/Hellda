#pragma once

#include "Globals.h"

class cRec {

	public:
		cRec();
		~cRec();
	
		cRec(float x, float y);
		bool intersects(cRec r);

		float bot;
		float top;
		float left;
		float right;

};
