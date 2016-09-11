#pragma once

#define WIN32_LEAN_AND_MEAN     
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <gl/glut.h>
#include <windows.h>
#include <math.h>
#include <minmax.h>
#include <iostream>

#define SCREEN_W 1024
#define SCREEN_H 768
#define X .525731112119133606 
#define Z .850650808352039932

#define Ia 1.0
#define Id 1.0
#define Ip 1.0

int windowH, windowW;

struct RandomColor {
	float r = 0.4;
	float g = 0.2;
	float b = 0.3;
};

static GLfloat vdata[12][3] = {
	{ -X, 0.0, Z },{ X, 0.0, Z },{ -X, 0.0, -Z },{ X, 0.0, -Z },
	{ 0.0, Z, X },{ 0.0, Z, -X },{ 0.0, -Z, X },{ 0.0, -Z, -X },
	{ Z, X, 0.0 },{ -Z, X, 0.0 },{ Z, -X, 0.0 },{ -Z, -X, 0.0 }
};
static GLuint tindices[20][3] = {
	{ 0,4,1 },{ 0,9,4 },{ 9,5,4 },{ 4,5,8 },{ 4,8,1 },
	{ 8,10,1 },{ 8,3,10 },{ 5,3,8 },{ 5,2,3 },{ 2,7,3 },
	{ 7,10,3 },{ 7,6,10 },{ 7,11,6 },{ 11,0,6 },{ 0,1,6 },
	{ 6,1,10 },{ 9,0,11 },{ 9,11,2 },{ 9,2,5 },{ 7,2,11 } };

static GLfloat light_pos[3] = { 0.25f,1.0f,0.0f };
static GLfloat view_pos[3] = { 0.0f,0.0f,-4.5f };
static GLfloat amb_color[3] = { 0.8f,0.8f,0.8f };
static GLfloat dif_color[3] = { 1.0f,1.0f,1.0f };
static GLfloat obj_color[3] = { 0.0f,0.0f,1.0f };
static GLfloat spec_color[3] = {0.1f, 0.2f, 0.3f};
static int vertexRender = 0;
static RandomColor Cor = RandomColor();
static float Ka = .5f;
static float Kd = .5f;
static float Ks = .5f;
static int nShiny = 1;

void ProcessMenu(int value) {
	// Save menu identifier as method flag
	vertexRender = value;

	// Trigger a redraw
	glutPostRedisplay();
}

void normalize(GLfloat *a) {
	GLfloat d = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] /= d; a[1] /= d; a[2] /= d;
}

float calcMagnitude(GLfloat *vetor) {
	float magnitude = sqrt(pow(vetor[0], 2) + pow(vetor[1], 2) + pow(vetor[2], 2));
	return magnitude;
}

float dotProduct(GLfloat *vetor1, GLfloat *vetor2) {
	float mag_vetor1 = calcMagnitude(vetor1);
	float mag_vetor2 = calcMagnitude(vetor2);

	float angulo = (vetor1[0] * vetor2[0] + vetor1[1] * vetor2[1] + vetor1[2] * vetor2[2]) / (mag_vetor1*mag_vetor2);

	return angulo;
}

void calcColor(GLfloat *vetn, GLfloat x, GLfloat y, GLfloat z) {
	//Passos para o cálculo de iluminação:
	//1) calcular vetor VL (que vai para a luz)
	GLfloat vetl[3] = { light_pos[0] - x, light_pos[1] - y, light_pos[2] - z };
	//2) calcular magnitude do vetor (d)
	GLfloat d = calcMagnitude(vetl);
	//3) calcular fator de atenuação (fatt), baseado no d
	GLfloat fatt = 1 / (d*d);
	//4) calcular angulo da difusa, dotProduct
	GLfloat angulo_difusa = dotProduct(vetn, vetl);
	//5) criar variáveis novas para normalizar (normalize): VL, view_pos e VN
	GLfloat vetlN[] = {vetl[0], vetl[1], vetl[2]};
	normalize(vetlN);
	GLfloat vetPosN[] = {view_pos[0], view_pos[1], view_pos[2]};
	normalize(vetPosN);
	GLfloat vetnN[] = {vetn[0], vetn[1], vetn[2]};
	normalize(vetnN);
	//6) calcular VR conforme fórmula do slide da especular com normal, Ad e vligth
	GLfloat vr[] = { vetnN[0] * (1 - angulo_difusa) - vetlN[0], vetnN[1] * (1 - angulo_difusa) - vetlN[1], vetnN[2] * (1 - angulo_difusa) - vetlN[2] };
	//7) calcular angulo da especular, dotProduct entre VR e view
	GLfloat angulo_especular = dotProduct(vr, vetPosN);
	//8) calcular As com shiness (nShiny)
	GLfloat rgb[3];
	for (int i = 0; i < 3; i++) {
		GLfloat result = Ia * Ka * obj_color[i] + fatt * Ip * (Kd * dif_color[i] * angulo_difusa + Ks * spec_color[i] * pow(angulo_especular, nShiny));
		rgb[i] = max(0, min(1, result));
	}

	glColor3fv(rgb);
}

void drawtri(GLfloat *a, GLfloat *b, GLfloat *c, int div, float r, GLfloat *trans_vet) {
	if (div <= 0) {
		calcColor(a, a[0] * r + trans_vet[0], a[1] * r + trans_vet[1], a[2] * r + trans_vet[2]);
		glNormal3fv(a);
		glVertex3f(a[0] * r + trans_vet[0], a[1] * r + trans_vet[1], a[2] * r + trans_vet[2]);
		if (vertexRender)
			calcColor(b, b[0] * r + trans_vet[0], b[1] * r + trans_vet[1], b[2] * r + trans_vet[2]);
		glNormal3fv(b); glVertex3f(b[0] * r + trans_vet[0], b[1] * r + trans_vet[1], b[2] * r + trans_vet[2]);

		if (vertexRender)
			calcColor(c, c[0] * r + trans_vet[0], c[1] * r + trans_vet[1], c[2] * r + trans_vet[2]);

		glNormal3fv(c);
		glVertex3f(c[0] * r + trans_vet[0], c[1] * r + trans_vet[1], c[2] * r + trans_vet[2]);
	}
	else {
		GLfloat ab[3], ac[3], bc[3];
		for (int i = 0; i<3; i++) {
			ab[i] = (a[i] + b[i]) / 2;
			ac[i] = (a[i] + c[i]) / 2;
			bc[i] = (b[i] + c[i]) / 2;
		}
		normalize(ab); normalize(ac); normalize(bc);
		drawtri(a, ab, ac, div - 1, r, trans_vet);
		drawtri(b, bc, ab, div - 1, r, trans_vet);
		drawtri(c, ac, bc, div - 1, r, trans_vet);
		drawtri(ab, bc, ac, div - 1, r, trans_vet);
	}
}

void drawsphere(int ndiv, float radius, GLfloat *trans_vet) {
	glBegin(GL_TRIANGLES);
	for (int i = 0; i<20; i++)
		drawtri(vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]], ndiv, radius, trans_vet);
	glEnd();
}

void Reshape(int w, int h) {
}

void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	glLoadIdentity();
	gluPerspective(45.0f, (float)windowW / windowH, 1.0f, 100.0f);
	gluLookAt(view_pos[0], view_pos[1], view_pos[2],
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	glPopMatrix();

	glLoadIdentity();

	/*
	* Desenha Ponto de Luz
	*/
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	glutSolidSphere(0.1 - (1.1 + light_pos[2]) / 20, 15, 15);
	glPopMatrix();

	/*
	* Desenha Esferas
	*/
	GLfloat translate[3] = { -0.5f,0.0f,1.0f };
	drawsphere(3, 0.3, translate);

	GLfloat translate2[3] = { 0.5f,0.0f,0.0f };
	drawsphere(3, 0.4, translate2);

	glFinish();
	glutSwapBuffers();
}

void Key(unsigned char key, int x, int y) {
	switch (key) {
	case 'd': case 'D':
		light_pos[0] += .1f;
		break;
	case 'a': case 'A':
		light_pos[0] -= .1f;
		break;
	case 'w': case 'W':
		light_pos[1] += .1f;
		break;
	case 's': case 'S':
		light_pos[1] -= .1f;
		break;
	case 'e': case 'E':
		light_pos[2] += .1f;
		break;
	case 'q': case 'Q':
		light_pos[2] -= .1f;
		break;
	case 'v': case 'V':
		vertexRender = true;
		break;
	case 'f': case 'F':
		vertexRender = false;
		break;
	case ',':
		if (Ka > 0.0f) Ka -= .1f;
		break;
	case '.':
		if (Ka < 1.0f) Ka += .1f;
		break;
	case '-':
		if (Kd > 0.0f) Kd -= .1f;
		break;
	case '+':
		if (Kd < 1.0f) Kd += .1f;
		break;
	case '/':
		if (Ks > 0.0f) Ks -= .1f;
		break;
	case '*':
		if (Ks < 1.0f) Ks += .1f;
		break;
	case ']':
		if (nShiny > 1) nShiny -= 1;
		break;
	case '[':
		if (nShiny < 200) nShiny += 1;
		break;
		// presets
	case '1':
		Kd = 0.0;
		Ks = 0.0;
		nShiny = 0;
		break;
	case '2':
		Kd = 1.0;
		Ks = 1.0;
		nShiny = 10;
		break;
	case 'c': case 'C':

		break;
	case 27:
		exit(0);
	}
}

bool InitGL() {
	/*
	* Carrega matrix identidade
	*/
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*
	* Parametros de shade
	*/
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	/*
	* Parκmetros de profundidade
	*/
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE); // para normalizar os vetores normais de objetos

	return true;
}


int main(int argc, char** argv) {
	windowW = 640;
	windowH = 480;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowW, windowH);
	glutInitWindowPosition((int)(SCREEN_H / 2 - windowH / 2), (int)(SCREEN_W / 2 - windowW / 2));
	glutCreateWindow("Iluminação na mão grande");

	// Cria menu
	glutCreateMenu(ProcessMenu);
	glutAddMenuEntry("Face Normals (F)", 0);
	glutAddMenuEntry("Vertex Normals (V)", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	if (!InitGL()) { return 1; }

	glutDisplayFunc(Display);
	glutIdleFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Key);

	glutMainLoop();
	return 0;
}