#pragma once
#include "HSM\StateBase.h"
#include "Graphics\DXBasicPainter.h"
#include "Graphics\DXManager.h"
#include "Graphics\MeshMathSurface.h"
#include "Graphics\ImageBMP.h"
#include "Sound\SndControl.h"
#define SND_GAME 4

#define CLSID_CSGame 0x0f347dcbb
#define MAX_PLAYERS 4
class CSGame :
	public CStateBase
{
private:
	struct POS{
		union {
			struct {
				float x;
				float y;
				float z;
			};
			float p[3];
		};
	};
	struct Player
	{
		CMeshMathSurface g_Surface;
		MATRIX4D world;
		MATRIX4D scalationFactor;
		VECTOR4D brightness = { 0,0,0,0 };
		MATRIX4D counterPosition;
		bool combination[3] = { false, false, false };
		unsigned int counterClicks = 0;
		POS position;
	};
	void printCounter(const unsigned int nCounter, MATRIX4D& ST);
	void initializePositions();
	void initializeCounterPositions();
public:
	Player players[MAX_PLAYERS];
	//CDXBasicPainter::PARAMS old;
	ID3D11ShaderResourceView* m_pSRVBackGround;
	ID3D11ShaderResourceView* m_pSRVBackGround2;
	bool changeBackground = true;
	CDXManager*  m_pDXManager;
	CDXBasicPainter* m_pDXPainter;
	float g_iWidth;
	float g_iHeight;
	
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

	bool m_bInitializationCorrect;
	unsigned long GetClassID() { return CLSID_CSGame; }
	const char* GetClassString() { return "CSGame"; }
	void LoadScene(char * filename);
public:
	CSGame();
	virtual ~CSGame();
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
	CSndControl* m_pSndBackground;
};

