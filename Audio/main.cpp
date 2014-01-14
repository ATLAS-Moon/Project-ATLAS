#include <time.h>

#include "Graphics.h"
#include "Audio.h"

// Queue Defines

#define ESCAPE_KEY		27
#define WIDTH			768

int main(int argc, char* argv[]){
	
	AudioHandle	   *ATLASAudio    = new AudioHandle;

	srand(time(NULL));
	printf("\nLaunching GLUT... Waiting for setup to complete!\n\n");

	/* Menu screem and main loop */

	ATLASAudio->initAudio(ATLASAudio);
	initGLUT(argc, argv);

	free(ATLASAudio);

	return 0;
}