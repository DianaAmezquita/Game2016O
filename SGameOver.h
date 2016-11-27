#pragma once
#include "HSM\StateBase.h"
//#include "HSM\StateBase.h"
#include "Graphics\DXBasicPainter.h"
#include "Graphics\DXManager.h"
#include "Graphics\FX.h"
#include "Graphics\ImageBMP.h"
#include "Sound\SndControl.h"

#define SND_EXPLOSION 1
#define SND_BACKGROUND 2
#define CLSID_CSGameOver 0x6bc8e57a
class CSGameOver :
	public CStateBase
{
public:
	CSGameOver();
	virtual ~CSGameOver();
public:
	CDXBasicPainter* m_pDXPainter;
	CDXManager*  m_pDXManager;

	CImageBMP*   g_pSysTextur; //CPU
	CFX* m_pEffects;

	ID3D11Texture2D* m_pImag;
	unsigned long GetClassID() { return CLSID_CSGameOver; }
	const char* GetClassString() { return "CSGameOver"; }
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
	CSndControl* m_pSndBackground;
};

