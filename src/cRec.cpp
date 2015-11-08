#include "include/cRec.h"

cRec::cRec() {}

cRec::~cRec() {}

cRec::cRec(float x, float y) {
	left = x;
	right = left + 1;
	bot = y;
	top = bot - 1;
}

bool cRec::intersects(cRec rec) {
	if (left > rec.right && right < rec.right && bot < rec.bot && top > rec.bot) return true;
	if (left > rec.right && right < rec.right && bot < rec.top && top > rec.top) return true;
	if (left > rec.left && right < rec.left && bot < rec.bot && top > rec.bot) return true;
	if (left > rec.left && right < rec.left && bot < rec.top && top > rec.top) return true;
	return false;
}