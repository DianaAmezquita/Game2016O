#pragma once
#include "DXManager.h"
class CImageBMP
{
	unsigned long m_ulSizeX; //Pixeles horizontales
	unsigned long m_ulSizeY; //Pixeles verticales
public:
	struct PIXEL
	{
		unsigned char r, g, b, a;
	};
protected:
	PIXEL* m_pBuffer; //Buffer de pixeles.
public:
	static CImageBMP* CreateBitmapFromFile(char* pszFileName, PIXEL(*pFnAlpha)(PIXEL& P));
	ID3D11Texture2D*  CreateTexture(CDXManager* pManager);
	static void       DestroyBitmap(CImageBMP* pBmp);
protected:
	CImageBMP();
	~CImageBMP();
};

