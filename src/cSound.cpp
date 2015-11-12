
#include "include/cSound.h"

cSound::cSound(void) {}

cSound::~cSound(void) {}

bool cSound::load(const std::string & filename) {
	bool res = buffer.loadFromFile(filename);
	sound.setBuffer(buffer);

	return res;
}

void cSound::play() {
	sound.play();
}
