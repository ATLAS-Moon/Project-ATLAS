#pragma once

#include <windows.h>
#include <math.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>
#include <stdlib.h>

#include "Audio.h"

class AudioHandle;

#define SPECTRUM		2048
#define ORTHO_WIDTH		768
#define ORTHO_HEIGHT	1024
#define MAX_HEIGHT		1000

typedef struct rgb{
	float red, blue, green;
}RGB;

void addAudioAccess(AudioHandle *audio);
float getRandomFloat();
void WAVClient(float *WAVDataIN);
void initGLUT(int argc, char* argv[]);
void ChangeSize(GLsizei w, GLsizei h);
void timerFunction(int delay);
void RenderScene();
void initAudio();
void keyHandle(unsigned char key, int x, int y);

