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
	if (contained(rec.left, rec.top) || contained(rec.left, rec.bot)
		|| contained(rec.right, rec.top) || contained(rec.right, rec.bot)) return true;

	return false;
}

bool cRec::contained(float x, float y) {
	return x > left && x < right && y > top && y < bot;
}