#ifndef _OBJECT_SHADOWS_H_
#define _OBJECT_SHADOWS_H_

//==========================================================================
// bi.x shadow map
const size_t biplaneNumVertices = 28;
const size_t biplaneNumSurfaces = 26;

float biplaneMaxX = 5.753740f;
float biplaneMinX = 1.332962f;
float biplaneMinZ = -3.637103f;
float biplaneMaxZ =  0.126533f;

float biplaneSize = 1 / (biplaneMaxX - biplaneMinZ);

float biplaneCX = (biplaneMaxX + biplaneMinX) * 0.5f;
float biplaneCZ = (biplaneMaxZ + biplaneMinZ) * 0.5f;

const float v_biplaneShadowmap[] =	{
										5.664129f,0.082558f,
										5.753740f,1.292298f,
										5.619324f,1.262428f,
										3.244649f,1.531259f,
										1.497247f,1.650740f,
										1.512182f,1.277363f,
										5.634259f,1.605935f,
										3.946597f,1.531259f,
										3.737507f,0.082558f,
										1.332962f,1.277363f,
										1.362832f,0.127364f,
										3.438805f,0.052688f,
										3.513481f,-0.126533f,
										3.662831f,-0.111598f,
										3.453740f,2.696194f,
										3.752442f,2.726064f,
										4.260234f,3.517622f,
										4.125818f,3.637103f,
										3.647896f,3.577363f,
										4.334909f,3.398142f,
										4.275169f,3.144246f,
										4.125818f,2.979960f,
										3.946597f,2.785805f,
										3.155039f,2.950090f,
										3.199844f,3.637103f,
										3.050494f,3.159181f,
										2.990753f,3.353337f,
										3.020624f,3.502687f
									};

const size_t s_biplaneShadowmap[] =	{
										0,1,2,
										3,4,5,
										2,6,7,
										8,0,2,
										8,2,7,
										5,9,10,
										5,10,11,
										3,5,11,
										11,12,13,
										11,13,8,
										3,11,8,
										3,8,7,
										14,3,7,
										14,7,15,
										16,17,18,
										19,16,18,
										20,19,18,
										21,20,18,
										22,21,18,
										15,22,18,
										14,15,18,
										23,14,18,
										23,18,24,
										25,23,24,
										26,25,24,
										26,24,27
									};

//==========================================================================

#endif
