
#include "include/cMusic.h"

cMusic::cMusic(void) {}

cMusic::~cMusic(void) {}

bool cMusic::load(const std::string & filenames) {
	if (music.openFromFile(filenames)) return true;
	return false;
}

void cMusic::play() {
	music.setLoop(true);
	music.play();
}

void cMusic::stop() {
	music.stop();
}
