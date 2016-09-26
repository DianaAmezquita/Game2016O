#pragma once
#include "EventBase.h"
class CEventListener
{
public:
	CEventListener(void);
	virtual unsigned long OnEvent(CEventBase* pEvent)=0;
	virtual ~CEventListener(void);
};

