#include "stdafx.h"
#include "ImageBMP.h"


CImageBMP::CImageBMP()
{
	m_pBuffer = 0;
	m_ulSizeX = 0;
	m_ulSizeY = 0;
}
CImageBMP::~CImageBMP()
{
}
#include <fstream>
#include <Windows.h>
using namespace std;
CImageBMP* CImageBMP::CreateBitmapFromFile(
	char* pszFileName,
	PIXEL(*pFnAlpha)(PIXEL& P))
{
	CImageBMP* pNewImage = 0;
	fstream file;
	file.open(pszFileName, ios::in | ios::binary);
	if (!file.is_open())
		return 0;
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	memset(&bfh, 0, sizeof(bfh));
	memset(&bih, 0, sizeof(bih));
	file.read((char*)&bfh.bfType, sizeof(WORD));
	if (bfh.bfType != 'MB')
		return 0;
	file.read((char*)&bfh.bfSize, 
		sizeof(bfh) - sizeof(WORD));
	file.read((char*)&bih.biSize, sizeof(DWORD));
	if (bih.biSize != sizeof(BITMAPINFOHEADER))
		return 0;
	file.read((char*)&bih.biWidth, 
		sizeof(bih) - sizeof(DWORD));
	//Ya se tiene la información del bitmap en 
	//bih. Se procede con la carga de datos.
	unsigned long RowLength =
		4 * ((bih.biBitCount*bih.biWidth + 31) / 32);
	switch (bih.biBitCount)
	{
	case 1: //1bpp (Tarea) Dicromático
	case 4: //4bpp (Tarea) Hexadecacromático
	break;
	case 8: //8bpp  256 colores
	{
		RGBQUAD Paleta[256];
		unsigned long Colors =
			bih.biClrUsed == 0 ? 256 : bih.biClrUsed;
		file.read((char*)Paleta, sizeof(RGBQUAD)*Colors);
		unsigned char *pRow = new unsigned char[RowLength];
		pNewImage = new CImageBMP();
		pNewImage->m_ulSizeX = bih.biWidth;
		pNewImage->m_ulSizeY = bih.biHeight;
		pNewImage->m_pBuffer = 
			new PIXEL[bih.biWidth*bih.biHeight];
		for (long j = 0; j < bih.biHeight; j++)
		{
			file.read((char*)pRow, RowLength);
			for (long i = 0; i < bih.biWidth; i++)
			{
				PIXEL* p = &pNewImage->m_pBuffer[(bih.biHeight-j-1)*bih.biWidth + i];
				p->b = Paleta[pRow[i]].rgbBlue;
				p->g = Paleta[pRow[i]].rgbGreen;
				p->r = Paleta[pRow[i]].rgbRed;
				p->a = Paleta[pRow[i]].rgbReserved;
			}
		}
		return pNewImage;
	}
	break;
	case 24: //24bpp 16M Colores
	case 32: //32bpp 16M Colores 256 alphas
	{
				 unsigned char *pRow = new unsigned char[RowLength];
				 pNewImage = new CImageBMP();
				 pNewImage->m_ulSizeX = bih.biWidth;
				 pNewImage->m_ulSizeY = bih.biHeight;
				 pNewImage->m_pBuffer = new PIXEL[bih.biWidth*bih.biHeight];
				 for (long j = 0; j < bih.biHeight; j++)
				 {
					 file.read((char*)pRow, RowLength);
					 for (long i = 0; i < bih.biWidth; i++)
					 {
						 PIXEL* p = &pNewImage->m_pBuffer[(bih.biHeight - j - 1)*bih.biWidth + i];
						 p->b = pRow[i * 3];
						 p->g = pRow[i * 3 + 1];
						 p->r = pRow[i * 3 + 2];
						 if (bih.biBitCount == 32)
							 p->a = pRow[i * 3 + 3];
						 else
							 p->a = 1;
					 }

				 }
				 return pNewImage;
	}
	}
	return NULL;
}


ID3D11Texture2D* CImageBMP::CreateTexture
(CDXManager* pManager)
{
	ID3D11Texture2D* pTexture = NULL;
	D3D11_TEXTURE2D_DESC dtd;
	memset(&dtd, 0, sizeof(dtd));
	dtd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dtd.ArraySize = 1;
	dtd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	dtd.Height = m_ulSizeY;
	dtd.Width = m_ulSizeX;
	dtd.MipLevels = 1;
	dtd.SampleDesc.Count = 1;
	dtd.SampleDesc.Quality = 0;
	D3D11_SUBRESOURCE_DATA dsd;
	dsd.SysMemSlicePitch = 0;
	dsd.SysMemPitch = sizeof(PIXEL)*m_ulSizeX;
	dsd.pSysMem = m_pBuffer;
	pManager->GetDevice()->CreateTexture2D(
		&dtd, &dsd, &pTexture);
	return pTexture;
}

void CImageBMP::DestroyBitmap(CImageBMP* pBmp)
{
	delete[] pBmp->m_pBuffer;
	delete pBmp;
}

