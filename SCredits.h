#pragma once
#include "HSM\StateBase.h"
#include "Graphics\DXBasicPainter.h"
#include "Graphics\DXManager.h"
#include "Graphics\FX.h"
#include "Graphics\ImageBMP.h"
#include "Sound\SndControl.h"


#define CLSID_CSCredits 0xbafe4772
class CSCredits :
	public CStateBase
{
public:
	CSCredits();
	virtual ~CSCredits();

public:
	CDXBasicPainter* m_pDXPainter;
	CDXManager*  m_pDXManager;

	CImageBMP*   g_pSysTextur; //CPU
	CFX* m_pEffects;

	ID3D11Texture2D* m_pImag;
	unsigned long GetClassID() { return CLSID_CSCredits; }
	const char* GetClassString() { return "CSCredits"; }
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
	CSndControl* m_pSndBackground;
};

