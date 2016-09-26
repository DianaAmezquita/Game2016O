#include "StdAfx.h"
#include "StateBase.h"
#include "StateMachineManager.h"

CStateBase::CStateBase(void)
{
	m_pSMOwner=0;
}


CStateBase::~CStateBase(void)
{
}

unsigned long CStateBase::OnEvent(CEventBase* pEvent)
{
	return m_pSMOwner->QuerySuperStateOf(ID());
}
CStateBase* CStateBase::GetSuperState(void)
{
	return (CStateBase*)m_pSMOwner->GetObjectByID(m_pSMOwner->QuerySuperStateOf(ID()));
}