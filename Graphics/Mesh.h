#pragma once
#include "Matrix4D.h"
#include <vector>
#include <map>
#include "DXBasicPainter.h"
using namespace std;
class CMesh
{
public:

	struct INTERSECTIONINFO
	{
		int Face;
		VECTOR4D LocalPosition;
	};
	vector<CDXBasicPainter::VERTEX> m_Vertices;
	vector<unsigned long> m_Indices;
public:
	CMesh();
	bool RayCast(VECTOR4D& RayOrigin,
		VECTOR4D & RayDir,
		multimap<float, INTERSECTIONINFO>& Faces //Lista de caras que se intersectan
		);
	bool RayCast(VECTOR4D& RayOrigin,
		VECTOR4D& RayDir,
		multimap<float, unsigned long>& Vertices,float radius);
	void VertexShade(CDXBasicPainter::VERTEX(*pVS)(CDXBasicPainter::VERTEX V));
	void LoadSuzanne();
	void CMesh::BuildTangentSpaceFromTexCoordsIndexed(void);
	void CMesh::BuildTangentSpaceFromTexCoordsIndexed(bool bGenerateNormal);
	void CMesh::Optimize();
	~CMesh();
};

