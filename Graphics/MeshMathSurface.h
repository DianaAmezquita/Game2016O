#pragma once
#include "Mesh.h"
class CMeshMathSurface :
	public CMesh
{
protected:
	unsigned long m_nVx;
	unsigned long m_nVy;
	void Tesselate(); //Construye el buffer de indices
public:
	void BuildAnalyticSurface(
		unsigned long nVx, //Resolución de la malla
		unsigned long nVy,
		float x0, float y0,
		float dx, float dy,
		float(*pFn)(float x, float y),
		VECTOR4D(*pFnDivergent)(float x, float y, float z)
		);
	void BuildParametricSurface(
		unsigned long nVx, //Resolución de la malla
		unsigned long nVy,
		float u0, float v0,
		float du, float dv,
		VECTOR4D (*pFn)(float u, float v));
	void BuildTextureCoords(float u0, float v0, float du, float dv);
	void SetColor(VECTOR4D& A,
		VECTOR4D& B, VECTOR4D& C, VECTOR4D& D);
	CMeshMathSurface();
	~CMeshMathSurface();
};

