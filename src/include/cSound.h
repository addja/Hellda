#pragma once

#include "Globals.h"
#include <SFML/Audio.hpp>

class cSound {
public:
	cSound(void);
	~cSound(void);

	bool load(const std::string & filename);
	void play();

private:
	sf::SoundBuffer buffer;
	sf::Sound sound;
};
