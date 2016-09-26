#pragma once
#include "HSM\StateBase.h"
#include <d3d11.h>
#define CLSID_CSMainMenu 0x33661188
class CSMainMenu :
	public CStateBase
{
public:
	const char* GetClassString() { return "CSMainMenu"; }
	unsigned long GetClassID() { return CLSID_CSMainMenu; }
public:
	CSMainMenu();
	virtual ~CSMainMenu();
private:
	ID3D11ShaderResourceView* m_pSRVBackGround;
	ID3D11ShaderResourceView* m_pSRVMainOption1;
	ID3D11ShaderResourceView* m_pSRVMainOption2;
	int m_nOption; // 0: option1, 1:option2
	void OnEntry();
	unsigned long OnEvent(CEventBase*  pEvent);
	void OnExit();
};

