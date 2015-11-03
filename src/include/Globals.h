#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gl/glut.h>
#include <string>
#include <iostream>
#include <vector>

// 1 -> on | 0 -> off
#define DEBUG_MODE 0

#define GAME_WIDTH	512
#define GAME_HEIGHT 512

#define MAP_WIDTH	4096/16
#define MAP_HEIGHT	1408/16

#define HUD_TILES	4