#include "include/cData.h"

cData::cData(void) {}

cData::~cData(void){}

int cData::GetID(int img) {
	return texture[img].GetID();
}

void cData::GetSize(int img, int *w, int *h) {
	texture[img].GetSize(w,h);
}

bool cData::LoadImage(int img, char *filename, int type) {
	int res;

	res = texture[img].Load(filename,type);
	if(!res) return false;

	return true;
}

bool cData::LoadSound(int s, const std::string & filename) {

	if (sound[s].load(filename)) return true;
	return false;
}

bool cData::LoadMusic(int m, const std::string & filename) {

	if (music[m].load(filename)) return true;
	return false;
}

void cData::playSound(int s) {
	sound[s].play();
}

void cData::playMusic(int m) {
	music[m].play();
}

void cData::stopMusic(int m) {
	music[m].stop();
}