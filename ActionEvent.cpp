#include "stdafx.h"
#include "ActionEvent.h"


CActionEvent::CActionEvent(int nSource, unsigned long ulTime, int nAction)
{
	m_ulEventType = ACTION_EVENT;
	m_nSource = nSource;
	m_ulTime = ulTime;
	m_nAction = nAction;
}

CActionEvent::~CActionEvent()
{
}
