#pragma once
#include "HSM\StateBase.h"
#include "Graphics\DXBasicPainter.h"
#include "Graphics\DXManager.h"
#define MAIN ((CSMain*)m_pSMOwner->GetObjectByID(CLSID_CSMain))
#define CLSID_CSMain 0x0dccd3ed
class CSMain :
	public CStateBase
{
public:
	HWND m_hWnd;
	CDXManager* m_pDXManager;
	CDXBasicPainter* m_pDXPainter;
	bool m_bInitializationCorrect;
	unsigned long GetClassID() { return CLSID_CSMain; }
	const char* GetClassString() { return "CSMain"; }
public:
	CSMain();
	virtual ~CSMain();

protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);

};

