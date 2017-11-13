#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include "Vector3D.h"
#include "CubeMesh.h"
#include "QuadMesh.h"

//Constants
const int vWidth = 1000;
const int vHeight = 1000;
const int meshSize = 20;

// Light positions
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };

//Light properties
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

//Mouse Triggers
int leftTrigger = -1;
int rightTrigger = -1;
GLfloat shoulderPitch = 0.0;
GLfloat shoulderYaw = 0.0;
GLfloat elbowPitch = 0.0;

//A quad mesh object, given by the provided QuadMesh.c file
static QuadMesh groundMesh;


void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int mx, int my);
void functionKeys(int key, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMove(int x, int y);
void drawRobotArm(void);

int main(int argc, char** argv)
{

	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - 1000) / 2, 0);
	glutCreateWindow("Assignment 2");

	initOpenGL(vWidth, vHeight);

	//Register Callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	//Enter GLUT event processing cycle
	glutMainLoop();
	return 1;
}

//Set up OpenGL. Sets up lighting, other OpenGL stuff, and the ground floor
void initOpenGL(int w, int h)
{
	//Light1 Properties setup
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	//Light2 Properties setup, which is the same as Light1
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	//Light1 and Light2 Position setup
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	//Enable Lights
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	//Ground setup
	Vector3D origin = NewVector3D(-20.0f, 0.0f, 10.0f); //doesn't refer to centroid, refers to one of the corners of the mesh
	Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
	Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
	Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
	Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
	Vector3D specular = NewVector3D(0.4f, 0.04f, 0.04f);

	groundMesh = NewQuadMesh(meshSize);
	InitMeshQM(&groundMesh, meshSize, origin, 40.0, 40.0, dir1v, dir2v);
	SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);
}

//sets up camera, sets materials and does the modeling transforms
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(0.0, 6.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//Drawing Global axes
	glDisable(GL_TEXTURE_2D);
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 5, 0);
	glVertex3f(5, 5, 0);
	glVertex3f(0, 5, 0);
	glVertex3f(0, 10, 0);
	glVertex3f(0, 5, 0);
	glVertex3f(0, 5, 5);
	glEnd();

	drawRobotArm();

	DrawMeshQM(&groundMesh, meshSize);
	glutSwapBuffers(); //Double buffering, swap buffers
}

//Called whenever user resizes the window, including at initialization
//set up viewport, projection, then change to modelview matrix mode
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int mx, int my)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'w':
		elbowPitch += 2;
		break;
	case 's':
		elbowPitch -= 2;
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

void functionKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1: //Help
		printf("\n\nINSTRUCTIONS\nThis Program allows you to control a robot arm in order for you to dig holes in the terrain\n\n");
			printf("Moving the Robot\n");
			printf("\t-Left Key: moves arm counter clockwise\n");
			printf("\t-Right Key: moves arm clockwise\n");
			printf("\t-Up Key: elevates arm\n");
			printf("\t-Down Key: lowers arm\n");
			printf("\t-W Key: elevates arm shoulder\n");
			printf("\t-S Key: lowers arm shoulder\n\n");
			printf("Moving the Camera\n");
			printf("\tUse the mouse to orbit around the scene. The camera will always point at the origin where the robot resides\n");
			printf("\t-Move Mouse Left + Hold Left Mouse Button: moves the camera clockwise of the scene\n");
			printf("\t-Move Mouse Right + Hold Left Mouse Button: moves the camera counter clockwise of the scene\n");
			printf("\t-Move Mouse Up + Hold Right Mouse Button: moves the camera upward, the limit being no more than directly over the scene\n");
			printf("\t-Move Mouse Down + Hold Right Mouse Button: moves the camera downward, the limit being no more than ground level\n");
			printf("\t-Move Mouse Scroll Wheel: use scroll wheel on mouse to zoom in and out, does not move camera closer to scene\n");
		break;
	case GLUT_KEY_DOWN:
		shoulderPitch += 2.0f;
		break;
	case GLUT_KEY_UP:
		shoulderPitch -= 2.0f;
		break;
	case GLUT_KEY_LEFT:
		shoulderYaw += 2.0f;
		break;
	case GLUT_KEY_RIGHT:
		shoulderYaw -= 2.0f;
		break;
	}
}

void mouseButton(int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		leftTrigger = 1;

	}
	else {
		leftTrigger = -1;

	}
	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
		rightTrigger = 1;
	}
	else {
		rightTrigger = -1;
	}
}

void mouseMove(int x, int y)
{
	if (leftTrigger >= 0) {
		printf("left trigger activated and moving\n");
		printf("X: %d\tY: %d\n", x, y);
	}
	else if (rightTrigger >= 0) {
		printf("right trigger activated and moving\n");
		printf("X: %d\tY: %d\n", x, y);
	}
}

void drawRobotArm(void)
{
	glPushMatrix();
	glRotatef(shoulderYaw, 0.0, 1.0, 0.0);
		glPushMatrix();		
			//Shoulder Transformations
			glRotatef(shoulderPitch, 0.0, 0.0, 1.0);
			//Base Shoulder Sphere
			glPushMatrix();
				glutSolidSphere(2.0, 20.0, 50);
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0.0, 7.0, 0.0);
				glScalef(1.0, 10.0, 1.0);
				glutSolidCube(1.0);
			glPopMatrix();

			//Elbow Transformations
				glRotatef(elbowPitch, 0.0, 0.0, 1.0);
				//Elbow Sphere
				glPushMatrix();
					glTranslatef(0.0, 12.0, 0.0);
					glutSolidSphere(1.0, 20, 50);
				glPopMatrix();
				glPushMatrix();
					glTranslatef(5.0, 12.0, 0.0);
					glScalef(10.0, 1.0, 1.0);
					glutSolidCube(1.0);
				glPopMatrix();
				//Head
				glPushMatrix();
				glTranslatef(11.0, 12.0, 0.0);
					glScalef(4.0, 0.5, 3.0);
					glutSolidCube(1.0);
				glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}