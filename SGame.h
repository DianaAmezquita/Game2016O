#pragma once
#include "HSM\StateBase.h"
#include "Graphics\DXBasicPainter.h"
#include "Graphics\DXManager.h"
#include "Graphics\MeshMathSurface.h"
#include "Graphics\ImageBMP.h"
#include "Sound\SndControl.h"
#define SND_GAME 4

#define CLSID_CSGame 0x0f347dcbb
class CSGame :
	public CStateBase
{
	struct Player
	{
		MATRIX4D world;
		VECTOR4D brightness;
	};
public:
	MATRIX4D a;
	bool m_combination[6];
	CDXManager*  m_pDXManager;
	CDXBasicPainter* m_pDXPainter;
	float g_iWidth;
	float g_iHeight;
	MATRIX4D g_World;
	MATRIX4D g_View;
	MATRIX4D g_Projection;
	CImageBMP*      g_pSysTexture; //CPU
	ID3D11Texture2D* g_pTexture;   //GPU
	ID3D11Texture2D* g_pNormalMapTrue;
	ID3D11Texture2D* g_pEmissiveMap;

	ID3D11ShaderResourceView* pSRV = NULL;
	ID3D11ShaderResourceView* pSRVNormalMap = NULL;
	ID3D11ShaderResourceView* pSRVEnvMap = NULL;
	ID3D11ShaderResourceView* pSRVNormalMapTrue = NULL;
	ID3D11ShaderResourceView* pSRVEmissiveMap = NULL;
	// Ahora tenemos que inicializar

	ID3D11Texture2D* g_pNormalMap;
	ID3D11Texture2D* g_pEnvMap;
	CMeshMathSurface g_Surface[2];
	unsigned int counterCLicks1;
	unsigned int counterCLicks2;
	bool m_bInitializationCorrect;
	unsigned long GetClassID() { return CLSID_CSGame; }
	const char* GetClassString() { return "CSGame"; }
public:
	CSGame();
	virtual ~CSGame();
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
	CSndControl* m_pSndBackground;
};

