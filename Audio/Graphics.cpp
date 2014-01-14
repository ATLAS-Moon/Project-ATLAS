#include "Graphics.h"

AudioHandle *localAudio;
GLfloat winHeight, winWidth;
GLfloat aspectRatio;
bool fullscreen = false;
float OScopeData[SPECTRUM];

RGB colorForPCM(float PCM){
	RGB tmp;

	tmp.red   = (255 - ((-log(abs(PCM))/3)*255));
	tmp.green = (0   + ((-log(abs(PCM))/3)*255));
	tmp.blue  = 0;

	return tmp;
}

float getRandomFloat(){
	return (0 + (float)rand()/((float)RAND_MAX));
}

void ChangeSize(GLsizei w, GLsizei h){
	if(h == 0){
		h = 1;
	}
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	aspectRatio = (GLfloat)w / (GLfloat)h;
	if (w <= h){
		//		  LEFT            RIGHT				    BOTTOM					     TOP             FAR  NEAR
		glOrtho (-ORTHO_WIDTH, ORTHO_WIDTH, -ORTHO_HEIGHT / aspectRatio, ORTHO_HEIGHT / aspectRatio, 1.0, -1.0);
	}
	else{
		//		  LEFT            RIGHT				    BOTTOM					     TOP             FAR  NEAR
		glOrtho (-ORTHO_WIDTH * aspectRatio, ORTHO_WIDTH * aspectRatio, -ORTHO_HEIGHT, ORTHO_HEIGHT, 1.0, -1.0);
	}

	winWidth  = w;
	winHeight = h;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void RenderScene(){

	RGB color;

	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_LINE_STRIP);
	
	for(int i = 0; i < SPECTRUM; i++){
		color = colorForPCM(OScopeData[i]);
		glColor3f((color.red / 255), (color.green / 255), (color.blue / 255));
		glVertex2f((((winWidth)/(SPECTRUM/2)) * i) - (winWidth), OScopeData[i] * MAX_HEIGHT);
	}

	glEnd();

	glutSwapBuffers();
	glFlush();
}

void timerFunction(int delay){

		RenderScene();

		glutPostRedisplay();
		glutTimerFunc(10, timerFunction, 1);

}

void WAVClient(float *WAVDataIN){
	memcpy(OScopeData, WAVDataIN, SPECTRUM * sizeof(float));
}

void addAudioAccess(AudioHandle *audio){
	
	localAudio = audio;

}

void keyHandle(unsigned char key, int x, int y){

	if(key == 'f'){
		fullscreen = !fullscreen;
		if(fullscreen){
			glutFullScreen();
		}
		else{
			glutReshapeWindow(1024, 786);
			glutPositionWindow(20,40);
		}
	}

}

void initGLUT(int argc, char* argv[]){

	GLfloat winHeight = 1024, winWidth = 768;

	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1024, 768);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("ATLAS_VR_PGM_PA-01");
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(keyHandle);
	glutReshapeFunc(ChangeSize);
	glClearColor(0, 0, 0, 1.0f);
	glutTimerFunc(1, timerFunction, 1);

	glutIdleFunc(getAudioFunc);

	glutMainLoop();
}