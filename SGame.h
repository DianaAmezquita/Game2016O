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
#define SCORE 11
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
	struct Sequence
	{
		int counter = 0;
		bool combination[3] = { false, false, false };
		char comChar[3] = { ' ',' ',' ' };
	};
	struct Player
	{
		CMeshMathSurface g_Surface;
		MATRIX4D world;
		MATRIX4D scalationFactor;
		VECTOR4D brightness = { 0,0,0,0 };
		VECTOR4D color;
		MATRIX4D counterPosition;
		bool combination[3] = { false, false, false };
		Sequence buttons;
		unsigned int counterClicks = 0;
		POS position;
	};
	void printCounter(const unsigned int nCounter, MATRIX4D& ST);
	void initializePositions();
	void initializeCounterPositions();
public:
	Player players[MAX_PLAYERS];
	ID3D11ShaderResourceView* m_pSRVBackGround;
	ID3D11ShaderResourceView* m_pSRVBackGround2;
	ID3D11ShaderResourceView* m_pSRVButtonAB;
	ID3D11ShaderResourceView* m_pSRVButtonABPressed;
	ID3D11ShaderResourceView* m_pSRVButtonBA;
	ID3D11ShaderResourceView* m_pSRVButtonBAPressed;
	ID3D11ShaderResourceView* m_pSRVTexture;
	//ID3D11Texture2D* m_pTexture;
	bool changeBackground = true;
	bool AB = true;
	bool freqButBackground = true;
	char correctCombination[3] = { 'a','b',' ' };
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
	void LoadScene(char * filename, const unsigned int p);
public:
	CSGame();
	virtual ~CSGame();
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
	CSndControl* m_pSndBackground;
};

