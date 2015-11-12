#pragma once

#include "Globals.h"
#include <SFML\Audio.hpp>

class cMusic {
public:
	cMusic(void);
	~cMusic (void);

	bool load(const std::string & filename);
	void play();
	void stop();

private:
	sf::Music music;
};
