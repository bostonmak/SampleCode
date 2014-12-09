// assign2.cpp : Defines the entry point for the console application.
//

/*
	CSCI 480 Computer Graphics
	Assignment 2: Simulating a Roller Coaster
	C++ starter code
*/

#include "stdafx.h"
#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "Spline.h"
#include "Point.h"
#include "Vector3.h"

const float Pi = 3.14159265358979;
const float ToRadians = 2 * Pi / 360.0f;

/* state of the world */
float g_vLandRotate[3] = { 0.0, 0.0, 0.0 };
float g_vLandTranslate[3] = { 0.0, 0.0, 0.0 };
float g_vLandScale[3] = { 1.0, 1.0, 1.0 };

int g_vMousePos[2] = { 0, 0 };
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;
int textureIndex = 1;
int cameraIndex = 0;
int totalPoints = 0;

Pic* g_pFloorTexture;
Pic* g_pFloorHeight;
Pic* g_pSkyTexture;

GLuint g_iLineList; 
GLuint g_iFloorList;
GLuint g_iSkyList;
GLuint g_iTrackList;
GLuint g_iFloorName;
GLuint g_iSkyName;

float g_fMaxDistance = 0.07f;
float g_s = 0.5;
float g_mBasisMatrix[16] = {-g_s   , 2 - g_s, g_s - 2    , g_s ,
							2 * g_s, g_s - 3, 3 - 2 * g_s, -g_s,
							-g_s   , 0      , g_s        , 0   , 
							0      , 1      , 0          , 0   };
float g_mControlMatrix[12] = {0, 0, 0,
							  0, 0, 0,
							  0, 0, 0,
							  0, 0, 0};

Vector3 g_vCameraPos = Vector3();
Vector3 g_vCameraForward = Vector3();
Vector3 g_vCameraUp = Vector3();
Vector3 g_vCameraRight = Vector3(0, 1, 0);

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;
CONTROLSTATE g_ControlState = ROTATE;

/* spline struct which contains how many control points, and an array of control points */

/* the spline array */
struct Spline *g_Splines;

/* total number of splines */
int g_iNumOfSplines;

std::vector<Point> pointsList;


int loadSplines(char *argv) {
	char *cName = (char *)malloc(128 * sizeof(char));
	FILE *fileList;
	FILE *fileSpline;
	int iType, i = 0, j, iLength;

	/* load the track file */
	fileList = fopen(argv, "r");
	if (fileList == NULL) {
		printf ("can't open file\n");
		exit(1);
	}
  
	/* stores the number of splines in a global variable */
	fscanf(fileList, "%d", &g_iNumOfSplines);

	g_Splines = (struct Spline *)malloc(g_iNumOfSplines * sizeof(struct Spline));

	/* reads through the spline files */
	for (j = 0; j < g_iNumOfSplines; j++) {
		i = 0;
		fscanf(fileList, "%s", cName);
		fileSpline = fopen(cName, "r");

		if (fileSpline == NULL) {
			printf ("can't open file\n");
			exit(1);
		}

		/* gets length for spline file */
		fscanf(fileSpline, "%d %d", &iLength, &iType);

		/* allocate memory for all the points */
		g_Splines[j].points = (struct Point *)malloc(iLength * sizeof(struct Point));
		g_Splines[j].numControlPoints = iLength;

		/* saves the data to the struct */
		while (fscanf(fileSpline, "%lf %lf %lf", 
			&g_Splines[j].points[i].x, 
			&g_Splines[j].points[i].y, 
			&g_Splines[j].points[i].z) != EOF) {
			i++;
		}
	}

	free(cName);

	return 0;
}

void moveCamera()
{
	// Camera Position
	g_vCameraPos.x = pointsList[cameraIndex].x - 75;
	g_vCameraPos.y = pointsList[cameraIndex].y - 30;
	g_vCameraPos.z = pointsList[cameraIndex].z - 5.75;
	cameraIndex++;
	// Forward Vector (T)
	g_vCameraForward.x = pointsList[cameraIndex].x - g_vCameraPos.x - 75;
	g_vCameraForward.y = pointsList[cameraIndex].y - g_vCameraPos.y - 30;
	g_vCameraForward.z = pointsList[cameraIndex].z - g_vCameraPos.z - 5.75;
	Normalize(g_vCameraForward);
	// Up Vector (N)
	crossProduct(g_vCameraForward, g_vCameraRight, g_vCameraUp);
	Normalize(g_vCameraUp);
	// Right Vector (B) = T x N
	crossProduct(g_vCameraUp, g_vCameraForward, g_vCameraRight);
	Normalize(g_vCameraRight);
	if (cameraIndex >= totalPoints)
	{
		cameraIndex = 0;
	}
}

void makeLines() 
{
	glBegin(GL_LINE_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < g_iNumOfSplines; i++) {
		for (int j = 1; j < g_Splines[i].numControlPoints - 2; j++) {
			std::vector<Point> pList;
			int index = 1;
			float u = 0.5f;
			float uVector[4] = { 0, 0, 0, 0 };
			float uBasis[4] = { 0, 0, 0, 0 };
			pList.push_back(g_Splines[i].points[j]);
			createControlMatrix(j, g_Splines[i], g_mControlMatrix);
			// Subdivide segment recursively
			interpolatePoints(g_Splines[i].points[j], g_Splines[i].points[j + 1],
				pList, g_mBasisMatrix, g_mControlMatrix, g_fMaxDistance, index, u, 0.0f, 1.0f, uVector, uBasis);
			for (int k = 0; k < index; k++) {
				glVertex3d(pList[k].x - 75, pList[k].y - 30, pList[k].z - 6);
			}
			for (auto iter = pList.begin(); iter != pList.end(); iter++) {
				pointsList.push_back(*iter);
				totalPoints++;
			}
		}
		// Add last vertex
		pointsList.push_back(g_Splines[i].points[g_Splines[i].numControlPoints - 2]);
		glVertex3d(g_Splines[i].points[g_Splines[i].numControlPoints - 2].x - 75, g_Splines[i].points[g_Splines[i].numControlPoints - 2].y - 30, g_Splines[i].points[g_Splines[i].numControlPoints - 2].z - 6);
	}
	glEnd();
}

void makeTracks()
{
	Vector3 up = Vector3();
	Vector3 currentQuad[4] = { Vector3(), Vector3(), Vector3(), Vector3() };
	Vector3 right = Vector3(0, 1, 0);
	float trackWidth = 0.5f;
	float railSize = 0.025f;
	int crossIndex = 0;
	for (int i = 0; i < totalPoints - 1; i++)
	{
		Vector3 forward = Vector3(pointsList[i + 1].x - pointsList[i].x, pointsList[i + 1].y - pointsList[i].y, pointsList[i + 1].z - pointsList[i].z);
		crossProduct(forward, right, up);
		Normalize(up);
		Multiply(right, railSize);
		Multiply(up, railSize);
		// Base vertices
		Vector3 point, v0, v1, v2, v3, v4, v5, v6, v7;
		if (i == 0)
		{
			point = Vector3(pointsList[i].x, pointsList[i].y, pointsList[i].z);
			v0 = Vector3(pointsList[i].x + up.x - right.x - 75, pointsList[i].y + up.y - right.y - 30, pointsList[i].z + up.z - right.z - 6);
			v1 = Vector3(pointsList[i].x - up.x - right.x - 75, pointsList[i].y - up.y - right.y - 30, pointsList[i].z - up.z - right.z - 6);
			v2 = Vector3(pointsList[i].x - up.x + right.x - 75, pointsList[i].y - up.y + right.y - 30, pointsList[i].z - up.z + right.z - 6);
			v3 = Vector3(pointsList[i].x + up.x + right.x - 75, pointsList[i].y + up.y + right.y - 30, pointsList[i].z + up.z + right.z - 6);
			v4 = Vector3(pointsList[i + 1].x + up.x - right.x - 75, pointsList[i + 1].y + up.y - right.y - 30, pointsList[i + 1].z + up.z - right.z - 6);
			v5 = Vector3(pointsList[i + 1].x - up.x - right.x - 75, pointsList[i + 1].y - up.y - right.y - 30, pointsList[i + 1].z - up.z - right.z - 6);
			v6 = Vector3(pointsList[i + 1].x - up.x + right.x - 75, pointsList[i + 1].y - up.y + right.y - 30, pointsList[i + 1].z - up.z + right.z - 6);
			v7 = Vector3(pointsList[i + 1].x + up.x + right.x - 75, pointsList[i + 1].y + up.y + right.y - 30, pointsList[i + 1].z + up.z + right.z - 6);

			currentQuad[0] = v4;
			currentQuad[1] = v5;
			currentQuad[2] = v6;
			currentQuad[3] = v7;
		}
		else
		{
			v0 = currentQuad[0];
			v1 = currentQuad[1];
			v2 = currentQuad[2];
			v3 = currentQuad[3];
			v4 = Vector3(pointsList[i + 1].x + up.x - right.x - 75, pointsList[i + 1].y + up.y - right.y - 30, pointsList[i + 1].z + up.z - right.z - 6);
			v5 = Vector3(pointsList[i + 1].x - up.x - right.x - 75, pointsList[i + 1].y - up.y - right.y - 30, pointsList[i + 1].z - up.z - right.z - 6);
			v6 = Vector3(pointsList[i + 1].x - up.x + right.x - 75, pointsList[i + 1].y - up.y + right.y - 30, pointsList[i + 1].z - up.z + right.z - 6);
			v7 = Vector3(pointsList[i + 1].x + up.x + right.x - 75, pointsList[i + 1].y + up.y + right.y - 30, pointsList[i + 1].z + up.z + right.z - 6);

			currentQuad[0] = v4;
			currentQuad[1] = v5;
			currentQuad[2] = v6;
			currentQuad[3] = v7;
		}

		// Make the right rail vertices
		Vector3 r0, r1, r2, r3, r4, r5, r6, r7;
		Vector3 scale = right;
		Multiply(scale, -6.0f);
		r0 = Add(v0, scale);
		r1 = Add(v1, scale);
		r2 = Add(v2, scale);
		r3 = Add(v3, scale);
		r4 = Add(v4, scale);
		r5 = Add(v5, scale);
		r6 = Add(v6, scale);
		r7 = Add(v7, scale);
		// Make the left rail vertices
		scale = right;
		Vector3 left = right;
		Multiply(scale, 7.0f);
		Multiply(left, -1.0f);
		left = Add(left, scale);
		Vector3 l0, l1, l2, l3, l4, l5, l6, l7;
		l0 = Add(v0, left);
		l1 = Add(v1, left);
		l2 = Add(v2, left);
		l3 = Add(v3, left);
		l4 = Add(v4, left);
		l5 = Add(v5, left);
		l6 = Add(v6, left);
		l7 = Add(v7, left);
		glBegin(GL_QUADS);
		glColor3f(0.0f, 0.0f, 1.0f);
		// Left Rail
		// Left face
		glVertex3f(l0.x, l0.y, l0.z);
		glVertex3f(l4.x, l4.y, l4.z);
		glVertex3f(l5.x, l5.y, l5.z);
		glVertex3f(l1.x, l1.y, l1.z);
		// Right face
		glVertex3f(l3.x, l3.y, l3.z);
		glVertex3f(l2.x, l2.y, l2.z);
		glVertex3f(l6.x, l6.y, l6.z);
		glVertex3f(l7.x, l7.y, l7.z);
		// Bottom face
		glVertex3f(l1.x, l1.y, l1.z);
		glVertex3f(l5.x, l5.y, l5.z);
		glVertex3f(l6.x, l6.y, l6.z);
		glVertex3f(l2.x, l2.y, l2.z);
		// Top face
		glColor3f(1, 1, 1);
		glVertex3f(l0.x, l0.y, l0.z);
		glVertex3f(l3.x, l3.y, l3.z);
		glVertex3f(l7.x, l7.y, l7.z);
		glVertex3f(l4.x, l4.y, l4.z);


		// Right Rail
		glColor3f(0.0f, 0.0f, 1.0f);
		// Left face
		glVertex3f(r0.x, r0.y, r0.z);
		glVertex3f(r4.x, r4.y, r4.z);
		glVertex3f(r5.x, r5.y, r5.z);
		glVertex3f(r1.x, r1.y, r1.z);
		// Right face
		glVertex3f(r3.x, r3.y, r3.z);
		glVertex3f(r2.x, r2.y, r2.z);
		glVertex3f(r6.x, r6.y, r6.z);
		glVertex3f(r7.x, r7.y, r7.z);
		// Bottom face
		glVertex3f(r1.x, r1.y, r1.z);
		glVertex3f(r5.x, r5.y, r5.z);
		glVertex3f(r6.x, r6.y, r6.z);
		glVertex3f(r2.x, r2.y, r2.z);
		// Top face
		glColor3f(1, 1, 1);
		glVertex3f(r0.x, r0.y, r0.z);
		glVertex3f(r3.x, r3.y, r3.z);
		glVertex3f(r7.x, r7.y, r7.z);
		glVertex3f(r4.x, r4.y, r4.z);

		// Cross beam every 7 sections
		if (crossIndex % 7 == 0)
		{
			Normalize(forward);
			Multiply(forward, railSize);
			v0 = l3;
			v1 = l2;
			v2 = r1;
			v3 = r0;
			v4 = Add(v0, forward);
			v5 = Add(v1, forward);
			v6 = Add(v2, forward);
			v7 = Add(v3, forward);

			glColor3f(0, 0, 1);
			// Front cross face
			glVertex3f(v0.x, v0.y, v0.z);
			glVertex3f(v1.x, v1.y, v1.z);
			glVertex3f(v2.x, v2.y, v2.z);
			glVertex3f(v3.x, v3.y, v3.z);
			// Back cross face
			glVertex3f(v7.x, v7.y, v7.z);
			glVertex3f(v6.x, v6.y, v6.z);
			glVertex3f(v5.x, v5.y, v5.z);
			glVertex3f(v4.x, v4.y, v4.z);
			// Bottom cross face
			glVertex3f(v1.x, v1.y, v1.z);
			glVertex3f(v5.x, v5.y, v5.z);
			glVertex3f(v6.x, v6.y, v6.z);
			glVertex3f(v2.x, v2.y, v2.z);
			// Top cross face
			glColor3f(1, 1, 1);
			glVertex3f(v0.x, v0.y, v0.z);
			glVertex3f(v3.x, v3.y, v3.z);
			glVertex3f(v7.x, v7.y, v7.z);
			glVertex3f(v4.x, v4.y, v4.z);


		}
		glEnd();

		Multiply(up, 2.0f);
		Multiply(right, 2.0f);
		crossProduct(up, forward, right);
		Normalize(right);
		crossIndex++;
	}
}

void makeFloor()
{
	glBindTexture(GL_TEXTURE_2D, g_iFloorName);
	glEnable(GL_TEXTURE_2D);
	for (int y = 0; y < g_pFloorTexture->ny - 1; y++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x < g_pFloorTexture->nx; x++)
		{
			glTexCoord2f((float)x / g_pFloorTexture->nx, (y + 1) / (float)g_pFloorTexture->ny);
			glVertex3f(x - (float)g_pFloorTexture->nx / 2, (y + 1) - (float)g_pFloorTexture->ny / 2, PIC_PIXEL(g_pFloorHeight, x, y + 1, 0) / -4.0f + 5.0f);
			glTexCoord2f((float)x / g_pFloorTexture->nx, y / (float)g_pFloorTexture->ny);
			glVertex3f(x - (float)g_pFloorTexture->nx / 2, y - (float)g_pFloorTexture->ny / 2, PIC_PIXEL(g_pFloorHeight, x, y, 0) / -4.0f + 5.0f);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);


}

void makeSky()
{
	glBindTexture(GL_TEXTURE_2D, g_iSkyName);
	glEnable(GL_TEXTURE_2D);
	float radius = g_pSkyTexture->nx / 2.0f;
	for (int z = 0; z < radius; z++)
	//int z = 0;
	{
		// Create triangle strips in concentric circles to create a dome
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x <= g_pSkyTexture->nx; x++)
		{
			float theta = ((float)x / g_pSkyTexture->nx) * 360 * ToRadians;
			float phi = ((float)z / radius) * 90 * ToRadians;
			float layerRadius = radius * cos(phi);
			float xCoord = layerRadius * cos(theta);
			float yCoord = layerRadius * sin(theta);
			float zCoord = radius * sin(phi);
			float vectorLength = sqrt(xCoord * xCoord + yCoord * yCoord + zCoord * zCoord);
			//glColor3f(PIC_PIXEL(g_pSkyTexture, (int)xCoord * 4, (g_pSkyTexture->ny / 2) + (int)yCoord * 2, 0), PIC_PIXEL(g_pSkyTexture, (int)xCoord * 4, (g_pSkyTexture->ny / 2) + (int)yCoord * 2, 1), PIC_PIXEL(g_pSkyTexture, (int)xCoord * 4, (g_pSkyTexture->ny / 2) + (int)yCoord * 2, 2));
			float tx1 = atan2(xCoord / vectorLength, zCoord /  vectorLength) / (2. * Pi) + 0.5;
			float ty1 = asin(yCoord / vectorLength) / Pi + .5;

			glTexCoord2f(tx1, ty1);

			glVertex3f(xCoord, yCoord, zCoord - 30);

			theta = ((float)x / g_pSkyTexture->nx) * 360 * ToRadians; 
			phi = ((float)(z + 1) / radius) * 90 * ToRadians;
			layerRadius = radius * cos(phi);
			xCoord = layerRadius * cos(theta);
			yCoord = layerRadius * sin(theta);
			zCoord = radius * sin(phi);
			//glColor3f(PIC_PIXEL(g_pSkyTexture, (int)xCoord * 4, (g_pSkyTexture->ny / 2) + (int)yCoord * 2, 0), PIC_PIXEL(g_pSkyTexture, (int)xCoord * 4, (g_pSkyTexture->ny / 2) + (int)yCoord * 2, 1), PIC_PIXEL(g_pSkyTexture, (int)xCoord * 4, (g_pSkyTexture->ny / 2) + (int)yCoord * 2, 2));
			vectorLength = sqrt(xCoord * xCoord + yCoord * yCoord + zCoord * zCoord);
			float tx = atan2(xCoord / vectorLength, zCoord / vectorLength) / (2. * Pi) + 0.5;
			float ty = asin(yCoord / vectorLength) / Pi + .5;
			if (tx < 0.75 && tx1 > 0.75)
				tx += 1.0;
			else if (tx > 0.75 && tx1 < 0.75)
				tx -= 1.0;
			glTexCoord2f(tx, ty);
			glVertex3f(xCoord, yCoord, zCoord - 30);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}

void initTexture(GLuint* textureName, Pic* imagePic)
{
	// Mipmap Sky and Floor
	BYTE* data = (BYTE*)malloc(imagePic->nx * imagePic->ny * 3);
	data = imagePic->pix;
	glGenTextures(textureIndex, textureName);
	textureIndex++;
 	glBindTexture(GL_TEXTURE_2D, *textureName);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, dimX, dimY,
// 		GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imagePic->nx, imagePic->ny, 0,
 		GL_RGB, GL_UNSIGNED_BYTE, data);
	free(data);
}

void myinit()
{
	g_iLineList = glGenLists(1);
	glNewList(g_iLineList, GL_COMPILE);
	makeLines();
	glEndList();

	initTexture(&g_iFloorName, g_pFloorTexture);
	g_iFloorList = glGenLists(2);
	glNewList(g_iFloorList, GL_COMPILE);
	makeFloor();
	glEndList();

	initTexture(&g_iSkyName, g_pSkyTexture);
	g_iSkyList = glGenLists(3);
	glNewList(g_iSkyList, GL_COMPILE);
	makeSky();
	glEndList();

	g_iTrackList = glGenLists(4);
	glNewList(g_iTrackList, GL_COMPILE);
	makeTracks();
	glEndList();

	glClearColor(0.0, 0.0, 0.0, 0.0);   // set background color
	glEnable(GL_DEPTH_TEST);            // enable depth buffering
	glShadeModel(GL_SMOOTH);            // interpolate colors during rasterization

	moveCamera();
}

void doIdle()
{
	/* do some stuff... */

	/* make the screen update */
	glutPostRedisplay();
}

void display()
{
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
		GL_REPLACE);
	glLoadIdentity(); // reset transformation
	// look at the center of the image
	moveCamera();
	gluLookAt(g_vCameraPos.x, g_vCameraPos.y, g_vCameraPos.z,
			  g_vCameraPos.x + g_vCameraForward.x, g_vCameraPos.y + g_vCameraForward.y, g_vCameraPos.z + g_vCameraForward.z,
			  g_vCameraUp.x, g_vCameraUp.y, g_vCameraUp.z);
	// apply transformations
	glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2]);
	glRotatef(g_vLandRotate[0], 1, 0, 0);
	glRotatef(g_vLandRotate[1], 0, 1, 0);
	glRotatef(g_vLandRotate[2], 0, 0, 1);
	glScalef(g_vLandScale[0], g_vLandScale[1], g_vLandScale[2]);

	//glCallList(g_iLineList);
	glCallList(g_iFloorList);
	glCallList(g_iSkyList);
	glCallList(g_iTrackList);



	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glFrustum(-0.5f, 1.5f, -0.5f, 1.5f, 0.5f, 10.0f);
	gluPerspective(80.0f, (float)w / h, 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mousedrag(int x, int y)
{
	int vMouseDelta[2] = { x - g_vMousePos[0], y - g_vMousePos[1] };

	switch (g_ControlState)
	{
	case TRANSLATE:
		if (g_iLeftMouseButton)
		{
			g_vLandTranslate[0] += vMouseDelta[0] * 0.01;
			g_vLandTranslate[1] -= vMouseDelta[1] * 0.01;
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandTranslate[2] += vMouseDelta[1] * 0.01;
		}
		break;
	case ROTATE:
		if (g_iLeftMouseButton)
		{
			g_vLandRotate[0] += vMouseDelta[0];
			g_vLandRotate[1] += vMouseDelta[1];
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandRotate[2] += vMouseDelta[1];
		}
		break;
	case SCALE:
		if (g_iLeftMouseButton)
		{
			g_vLandScale[0] *= 1.0 + vMouseDelta[0] * 0.01;
			g_vLandScale[1] *= 1.0 - vMouseDelta[1] * 0.01;
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandScale[2] *= 1.0 - vMouseDelta[1] * 0.01;
		}
		break;
	}
	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		g_iLeftMouseButton = (state == GLUT_DOWN);
		break;
	case GLUT_MIDDLE_BUTTON:
		g_iMiddleMouseButton = (state == GLUT_DOWN);
		break;
	case GLUT_RIGHT_BUTTON:
		g_iRightMouseButton = (state == GLUT_DOWN);
		break;
	}

	switch (glutGetModifiers())
	{
	case GLUT_ACTIVE_CTRL:
		g_ControlState = TRANSLATE;
		break;
	case GLUT_ACTIVE_SHIFT:
		g_ControlState = SCALE;
		break;
	default:
		g_ControlState = ROTATE;
		break;
	}

	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// I've set the argv[1] to track.txt.
	// To change it, on the "Solution Explorer",
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your track file name for the "Command Arguments"
	if (argc<2)
	{  
		printf ("usage: %s <trackfile>\n", argv[0]);
		exit(0);
	}
	g_pFloorTexture = jpeg_read("Ground.jpg", NULL);
	if (!g_pFloorTexture)
	{
		printf("no ground texture file found. \n");
	}
	g_pFloorHeight = jpeg_read("Height.jpg", NULL);
	if (!g_pFloorHeight)
	{
		printf("no ground height file found. \n");
	}
	g_pSkyTexture = jpeg_read("Sky.jpg", NULL);
	if (!g_pFloorTexture)
	{
		printf("no sky texture file found. \n");
	}
	glutInit(&argc, (char**)argv);

	loadSplines(argv[1]);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Assignment 2");

	/* tells glut to use a particular display function to redraw */
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	/* replace with any animate code */
	glutIdleFunc(doIdle);

	/* callback for mouse drags */
	glutMotionFunc(mousedrag);
	/* callback for idle mouse movement */
	glutPassiveMotionFunc(mouseidle);
	/* callback for mouse button changes */
	glutMouseFunc(mousebutton);


	/* do initialization */
	myinit();


	glutMainLoop();

	return 0;
}