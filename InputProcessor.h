#pragma once
#include "HSM\EventListener.h"
#include "HSM\StateMachineManager.h"
#include "Input\InputManager.h"

#include "SMain.h"
#include <map>

using namespace std;
class CInputEvent;
class CInputProcessor :
	public CEventListener
{
public:
	CStateMachineManager* m_pHSMOwner;
	map <int, CInputEvent*> m_mapLastEvent;
	CInputProcessor(CStateMachineManager* pHSMOwner);
	virtual unsigned long OnEvent(CEventBase* pEvent);
	virtual ~CInputProcessor();
};

