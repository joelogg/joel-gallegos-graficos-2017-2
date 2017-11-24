#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include "GL/glut.h"


#define NUMJ (12)

static int winwidth,winheight;
static int mx,my,mxLuz,myLuz;
static int flagL=0, flagR=0;
static float rotx=0.0f, roty=0.0f;
static float rotxLuz=0.0f, rotyLuz=0.0f;

GLfloat jv[NUMJ][3] = {{0,1,0},{0,2,0},
		{3,1,0},{3,2,0},
		{3,1,-1},{3,2,-1},
		{0,1,-1},{0,2,-1},
		{0,1,0},{0,2,0}};

void dibujarPrisma(float x, float y, float z, int ancho, int alto, int profundidad)
{
	glBegin(GL_QUAD_STRIP);
	//glColor3f(0.0,0.0,1.0);
	//glColor3f(1.0,0.0,0.0);
	glVertex3f(x,y,z);
	glVertex3f(x,y-alto,z);

	glVertex3f(x+ancho,y,z);
	glVertex3f(x+ancho,y-alto,z);

	//glColor3f(0.0,1.0,0.0);

	glVertex3f(x+ancho,y,z-profundidad);
	glVertex3f(x+ancho,y-alto,z-profundidad);


	glVertex3f(x,y,z-profundidad);
	glVertex3f(x,y-alto,z-profundidad);

	//glColor3f(1.0,0.0,0.0);
	glVertex3f(x,y,z);
	glVertex3f(x,y-alto,z);


	glEnd();
	glBegin(GL_QUAD_STRIP);

	//glColor3f(1.0,1.0,0.0);
	glVertex3f(x,y,z);
	glVertex3f(x,y,z-profundidad);

	//glColor3f(0.0,1.0,1.0);
	glVertex3f(x+ancho,y,z);
	glVertex3f(x+ancho,y,z-profundidad);

	glEnd();

	glEnd();
	glBegin(GL_QUAD_STRIP);

	//glColor3f(0.0,0.0,1.0);
	glVertex3f(x,y-alto,z);
	glVertex3f(x,y-alto,z-profundidad);

	//glColor3f(0.0,1.0,1.0);
	glVertex3f(x+ancho,y-alto,z);
	glVertex3f(x+ancho,y-alto,z-profundidad);

	glEnd();
}

void dibujarJoel()
{
	//J
	dibujarPrisma(-7,2.5,0, 3, 1, 1);
	dibujarPrisma(-6,1.5,0, 1, 3, 1);
	dibujarPrisma(-7,-1.5,0, 2, 1, 1);
	//O
	dibujarPrisma(-3,2.5,0, 3, 1, 1);
	dibujarPrisma(-3,-1.5,0, 3, 1, 1);
	dibujarPrisma(-3,1.5,0, 1, 3, 1);
	dibujarPrisma(-1,1.5,0, 1, 3, 1);
	//G
	dibujarPrisma(1,2.5,0, 3, 1, 1);
	dibujarPrisma(1,1.5,0, 1, 1, 1);
	dibujarPrisma(1,0.5,0, 3, 1, 1);
	dibujarPrisma(1,-0.5,0, 1, 1, 1);
	dibujarPrisma(1,-1.5,0, 3, 1, 1);
	dibujarPrisma(3,-0.5,0, 1, 1, 1);
	//G

	dibujarPrisma(5,2.5,0, 3, 1, 1);
	dibujarPrisma(5,1.5,0, 1, 1, 1);
	dibujarPrisma(5,0.5,0, 3, 1, 1);
	dibujarPrisma(5,-0.5,0, 1, 1, 1);
	dibujarPrisma(5,-1.5,0, 3, 1, 1);
	dibujarPrisma(7,-0.5,0, 1, 1, 1);
}

void drawgraphix()
{
	const GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	const GLfloat light_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	const GLfloat light_specular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	const GLfloat light_position[] = { 5, 2.0f, 10.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	const GLfloat mat_ambient[] = { 0.1f, 0.1f, 1.0f, 1.0f };
	const GLfloat mat_diffuse[] = { 0.2f, 0.2f, 0.8f, 1.0f };
	const GLfloat mat_specular[] = { 0.6f, 0.6f, 1.0f, 1.0f };
	const GLfloat high_shininess[] = { 100.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	glPushMatrix();
	glTranslatef(0.0f,0.0f,-6.0f);
	glRotatef(rotx,0.0f,1.0f,0.0f);
	glRotatef(roty,1.0f,0.0f,0.0f);

	dibujarJoel();

	glPopMatrix();

	glutSwapBuffers();
}

void reshapefunc(int width,int height)
{
	winwidth=width;
	winheight=height;

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(120.0,(float)width/height,1.0,20.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f,0.0f,5.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
}

void mousefunc(int button,int state,int x,int y)
{
	if (button==GLUT_LEFT_BUTTON)
	{
		if (state==GLUT_DOWN)
		{
			flagL=1;
		}
		else
		{
			flagL=0;
		}
	}
	else if (button==GLUT_RIGHT_BUTTON)
	{
		if (state==GLUT_DOWN)
		{
			flagR=1;
		}
		else
		{
			flagR=0;
		}
	}
}

void motionfunc(int x,int y)
{
	if (flagL>0)
	{
		if (flagL>1)
		{
			rotx+=360.0f*(x-mx)/winwidth;
			roty+=360.0f*(y-my)/winheight;
		}

		mx=x;
		my=y;

		flagL=2;
	}
	else if (flagR>0)
	{
		if (flagR>1)
		{
			rotxLuz+=360.0f*(x-mxLuz)/winwidth;
			rotyLuz+=360.0f*(y-myLuz)/winheight;
		}

		mxLuz=x;
		myLuz=y;

		flagR=2;
	}
}

void keyboardfunc(unsigned char key,int x,int y)
{
	if (key=='q' || key==27) exit(0);
}


void idlefunc()
{
	glutPostRedisplay();
}

int main(int argc,char **argv)
{
	winwidth=512;
	winheight=512;

	glutInit(&argc,argv);
	glutInitWindowSize(winwidth,winheight);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutCreateWindow("Practica 1 - Ejercicio 1.1");

	glutDisplayFunc(drawgraphix);
	glutReshapeFunc(reshapefunc);
	glutMouseFunc(mousefunc);
	glutMotionFunc(motionfunc);
	glutKeyboardFunc(keyboardfunc);
	glutIdleFunc(idlefunc);

	glutMainLoop();

	return(0);
}
