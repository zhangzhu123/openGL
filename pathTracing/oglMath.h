#pragma once

#define PI 3.1415926

#include <math.h>
#include <GL/glew.h>

void oglMatrixMul(GLfloat am[16], GLfloat bm[16], GLfloat(&solve)[16])
{
	solve[0] = am[0] * bm[0] + am[4] * bm[1] + am[8] * bm[2] + am[12] * bm[3];
	solve[1] = am[1] * bm[0] + am[5] * bm[1] + am[9] * bm[2] + am[13] * bm[3];
	solve[2] = am[2] * bm[0] + am[6] * bm[1] + am[10] * bm[2] + am[14] * bm[3];
	solve[3] = am[3] * bm[0] + am[7] * bm[1] + am[11] * bm[2] + am[15] * bm[3];

	solve[4] = am[0] * bm[4] + am[4] * bm[5] + am[8] * bm[6] + am[12] * bm[7];
	solve[5] = am[1] * bm[4] + am[5] * bm[5] + am[9] * bm[6] + am[13] * bm[7];
	solve[6] = am[2] * bm[4] + am[6] * bm[5] + am[10] * bm[6] + am[14] * bm[7];
	solve[7] = am[3] * bm[4] + am[7] * bm[5] + am[11] * bm[6] + am[15] * bm[7];

	solve[8] = am[0] * bm[8] + am[4] * bm[9] + am[8] * bm[10] + am[12] * bm[11];
	solve[9] = am[1] * bm[8] + am[5] * bm[9] + am[9] * bm[10] + am[13] * bm[11];
	solve[10] = am[2] * bm[8] + am[6] * bm[9] + am[10] * bm[10] + am[14] * bm[11];
	solve[11] = am[3] * bm[8] + am[7] * bm[9] + am[11] * bm[10] + am[15] * bm[11];

	solve[12] = am[0] * bm[12] + am[4] * bm[13] + am[8] * bm[14] + am[12] * bm[15];
	solve[13] = am[1] * bm[12] + am[5] * bm[13] + am[9] * bm[14] + am[13] * bm[15];
	solve[14] = am[2] * bm[12] + am[6] * bm[13] + am[10] * bm[14] + am[14] * bm[15];
	solve[15] = am[3] * bm[12] + am[7] * bm[13] + am[11] * bm[14] + am[15] * bm[15];
}

void oglVectexMatrixMul(GLfloat m[16], const GLfloat v[4], GLfloat(&solve)[4])
{
	GLfloat tempSolve[4];
	tempSolve[0] = m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12] * v[3];
	tempSolve[1] = m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13] * v[3];
	tempSolve[2] = m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14] * v[3];
	tempSolve[3] = m[3] * v[0] + m[7] * v[1] + m[11] * v[2] + m[15] * v[3];

	solve[0] = tempSolve[0];
	solve[1] = tempSolve[1];
	solve[2] = tempSolve[2];
	solve[3] = tempSolve[3];

}

void oglTranspositionMatrix(GLfloat rawM[16], GLfloat(&dstM)[16])
{
	dstM[0] = rawM[0];
	dstM[1] = rawM[4];
	dstM[2] = rawM[8];
	dstM[3] = rawM[12];

	dstM[4] = rawM[1];
	dstM[5] = rawM[5];
	dstM[6] = rawM[9];
	dstM[7] = rawM[13];

	dstM[8] = rawM[2];
	dstM[9] = rawM[6];
	dstM[10] = rawM[10];
	dstM[11] = rawM[14];

	dstM[12] = rawM[3];
	dstM[13] = rawM[7];
	dstM[14] = rawM[11];
	dstM[15] = rawM[15];
}

void oglTranslate(GLfloat tv[3], GLfloat(&m)[16])
{
	GLfloat tempM[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

	tempM[12] += tv[0];
	tempM[13] += tv[1];
	tempM[14] += tv[2];

	for (int i = 0; i < 16; i++) {
		m[i] = tempM[i];
	}
}

void oglScale(GLfloat sv[3], GLfloat(&m)[16])
{
	GLfloat tempM[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

	tempM[0]  *= sv[0];
	tempM[5]  *= sv[1];
	tempM[10] *= sv[2];

	for (int i = 0; i < 16; i++) {
		m[i] = tempM[i];
	}
}

void oglRotateQuaternion(GLfloat rAxis[3], GLfloat theta, GLfloat(&m)[16])
{
	GLfloat radianTheta = theta * PI / 180.0;
	GLfloat sinTheta = sin(radianTheta / 2.0);
	GLfloat cosTheta = cos(radianTheta / 2.0);
	GLfloat quater[4] = { rAxis[0] * sinTheta, rAxis[1] * sinTheta, rAxis[2] * sinTheta, cosTheta };

	m[0] = 1 - 2 * (quater[1] * quater[1] + quater[2] * quater[2]);
	m[1] = 2 * (quater[0] * quater[1] + quater[2] * quater[3]);
	m[2] = 2 * (quater[0] * quater[2] - quater[1] * quater[3]);
	m[3] = 0;
	
	m[4] = 2 * (quater[0] * quater[1] - quater[2] * quater[3]);
	m[5] = 1 - 2 * (quater[0] * quater[0] + quater[2] * quater[2]);
	m[6] = 2 * (quater[1] * quater[2] + quater[0] * quater[3]);
	m[7] = 0;

	m[8] = 2 * (quater[0] * quater[2] + quater[1] * quater[3]);
	m[9] = 2 * (quater[1] * quater[2] - quater[0] * quater[3]);
	m[10] = 1 - 2 * (quater[0] * quater[0] + quater[1] * quater[1]);
	m[11] = 0;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;
}


void oglRotateEuler(GLfloat yawTheta, GLfloat pitchTheta, GLfloat rollTheta, GLfloat(&m)[16])
{
	GLfloat radianYawTheta = yawTheta * PI / 180.0;
	GLfloat radianPitchTheta = pitchTheta * PI / 180.0;
	GLfloat radianRollTheta = rollTheta * PI / 180.0;

	GLfloat yawMatrix[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	GLfloat pitchMatrix[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	GLfloat rollMatrix[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	
	yawMatrix[0]  = cos(radianYawTheta);
	yawMatrix[2]  = -1*sin(radianYawTheta);
	yawMatrix[8]  = sin(radianYawTheta);
	yawMatrix[10] = cos(radianYawTheta);

	pitchMatrix[5] = cos(radianPitchTheta);
	pitchMatrix[6] = sin(radianPitchTheta);
	pitchMatrix[9] = -1*sin(radianPitchTheta);
	pitchMatrix[10] = cos(radianPitchTheta);

	rollMatrix[0] = cos(radianRollTheta);
	rollMatrix[1] = sin(radianRollTheta);
	rollMatrix[4] = -1*sin(radianRollTheta);
	rollMatrix[5] = cos(radianRollTheta);

	GLfloat tempM[16];
	oglMatrixMul(yawMatrix, pitchMatrix, tempM);
	oglMatrixMul(tempM, rollMatrix, m);
}

void oglModelMatrix(GLfloat tm[16], GLfloat rm[16], GLfloat sm[16], GLfloat(&transM)[16])
{
	GLfloat tempM[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	oglMatrixMul(tm, rm, tempM);
	oglMatrixMul(tempM, sm, transM);
}

void oglViewMatrix(GLfloat tm[16], GLfloat rm[16], GLfloat(&viewm)[16])
{
	GLfloat inverseTM[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	GLfloat inverseRM[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	
	inverseTM[12] = -1 * tm[12];
	inverseTM[13] = -1 * tm[13];
	inverseTM[14] = -1 * tm[14];
	
	oglTranspositionMatrix(rm, inverseRM);
	
	oglMatrixMul(inverseRM, inverseTM, viewm);
}

void oglPerspectiveMatrix(GLfloat fov, GLfloat aspect, GLfloat nearPlane, GLfloat farPlane, GLfloat(&perspm)[16])
{
	GLfloat radianFov = fov * PI / 180.0;
	
	GLfloat top, bottom, left, right;
	right = nearPlane * tan(radianFov / 2);
	left = -1 * right;
	top = right / aspect;
	bottom = -1 * top;

	GLfloat a, b;
	a = -1 * (farPlane + nearPlane) / (farPlane - nearPlane);
	b = -2 * farPlane*nearPlane / (farPlane - nearPlane);

	perspm[0] = 2 * nearPlane / (right - left);
	perspm[1] = 0;
	perspm[2] = 0;
	perspm[3] = 0;

	perspm[4] = 0;
	perspm[5] = 2 * nearPlane / (top - bottom);
	perspm[6] = 0;
	perspm[7] = 0;

	perspm[8] = (right + left) / (right - left);
	perspm[9] = (top + bottom) / (top - bottom);
	perspm[10] = a;
	perspm[11] = -1;

	perspm[12] = 0;
	perspm[13] = 0;
	perspm[14] = b;
	perspm[15] = 0;

}