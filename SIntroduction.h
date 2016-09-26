#pragma once
#include "HSM\StateBase.h"
#include "Graphics\DXBasicPainter.h"
#include "Graphics\DXManager.h"
#include "Graphics\FX.h"

#define CLSID_CSIntroduction 0x0fcad1ba
class CSIntroduction :
	public CStateBase
{
public:
	CDXBasicPainter* m_pDXPainter;
	CDXManager*  m_pDXManager;
	CFX* m_pEffects;

	ID3D11Texture2D* m_pImagIntroduction;
	unsigned long GetClassID() { return CLSID_CSIntroduction; }
	const char* GetClassString() { return "CSIntroduction"; }
public:
	CSIntroduction();
	virtual ~CSIntroduction();

protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);

	//ID3D11Texture2D* m_pSplash;
	//ID3D11ShaderResourceView* m_pSRVSplash;
};

