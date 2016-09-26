#include "StdAfx.h"
#include "StateMachineManager.h"
#include <stdio.h>
#include <windows.h>
#include "EventWin32.h"
CStateMachineManager::CStateMachineManager(void)
{
	m_ulIDInitialState=0;
	m_ulIDCurrentState=0;
	m_StateNull.AddRef();
	//Un lugar seguro dónde iniciar...
	RegisterState(&m_StateNull,CLSID_CStateNull,0);
	SetInitialState(CLSID_CStateNull);
	m_uldTime=0;
	m_ulTime=0;
}


CStateMachineManager::~CStateMachineManager(void)
{
	//Limpieza total.
	Reset();
	ClearLinks();
}

void CStateMachineManager::CStateNull::OnEntry()
{
	printf("%s","CStateNull::OnEntry()\n");
}
void CStateMachineManager::CStateNull::OnExit()
{
	printf("%s","CStateNull::OnExit()\n");
}
unsigned long CStateMachineManager::CStateNull::OnEvent(CEventBase* pEvent)
{
	return 0;
}

void CStateMachineManager::RegisterState(CStateBase* pState,unsigned long ulIDState,unsigned long ulIDInitialSubState)
{
	CObjectBase* pObj=NULL;
	pState->ID()=ulIDState;
	pState->InitialSubState()=ulIDInitialSubState;
	pState->SMOwner()=this;
	pObj=AttachObject(ulIDState,pState);
	SAFE_RELEASE(pObj);
}
CStateBase* CStateMachineManager::UnregisterState(unsigned long ulIDState)
{
	UnlinkFromSuperState(ulIDState);
	return (CStateBase*)DetachObject(ulIDState);
}
void CStateMachineManager::UnlinkFromSuperState(unsigned long ulIDState)
{
	m_mapSuperStates.erase(ulIDState);
}
void CStateMachineManager::LinkToSuperState(unsigned long ulIDState,unsigned long ulIDSuperState)
{
	UnlinkFromSuperState(ulIDState);
	m_mapSuperStates.insert(make_pair(ulIDState,ulIDSuperState));
}
void CStateMachineManager::Start()
{
	InitState(m_ulIDInitialState);
}
unsigned long CStateMachineManager::QuerySuperStateOf(unsigned long ulIDState)
{
	auto it=m_mapSuperStates.find(ulIDState);
	if(it!=m_mapSuperStates.end())
		return it->second;
	return 0;
}
void CStateMachineManager::Reset()
{
	//Salir de cualquier estado en orden ascendente (normalmente)
	while(m_ulIDCurrentState)
	{
		CStateBase* pState=(CStateBase*)GetObjectByID(m_ulIDCurrentState);
		pState->OnExit();
		SAFE_RELEASE(pState);
		m_ulIDCurrentState=QuerySuperStateOf(m_ulIDCurrentState);
	}
	m_ulIDSourceState=m_ulIDCurrentState;
}
void CStateMachineManager::Transition(unsigned long ulIDTargetState)
{
	CStateBase* pState=NULL;
	//Salir de los subestados hasta sin incluir el superestado que origina la solicitud de transición
	if(m_ulIDCurrentState!=m_ulIDSourceState)
		while( QuerySuperStateOf(m_ulIDCurrentState) && (QuerySuperStateOf(m_ulIDCurrentState)!=m_ulIDSourceState))
		{
			pState=(CStateBase*)GetObjectByID(m_ulIDCurrentState);
			pState->OnExit();
			SAFE_RELEASE(pState);
			m_ulIDCurrentState=QuerySuperStateOf(m_ulIDCurrentState);
		}
	//Encontrar el ancestro común para determinar hasta que estado es necesario salir. 
	//Recordar que si las transisiones son intraestado, no es necesario salir del estado que
	//contiene la transición que se está solicitando.
	do
	{
		pState=(CStateBase*)GetObjectByID(m_ulIDCurrentState);
		if(pState)
			pState->OnExit();
		SAFE_RELEASE(pState);
		m_ulIDCurrentState=QuerySuperStateOf(m_ulIDCurrentState);
		m_ulIDSourceState=ulIDTargetState; 
		do
		{
			m_ulIDSourceState=QuerySuperStateOf(m_ulIDSourceState);
			if(m_ulIDSourceState==m_ulIDCurrentState)  
				goto match;
		}
		while(m_ulIDSourceState);
	}
	while(m_ulIDCurrentState);
match:
	while(1)
	{
		m_ulIDCurrentState=ulIDTargetState;
		//Este ciclo se asegura que inicialicemos primero a los superestados de manera descendente
		while(m_ulIDSourceState!=QuerySuperStateOf(m_ulIDCurrentState))
			m_ulIDCurrentState=QuerySuperStateOf(m_ulIDCurrentState);
		if(m_ulIDCurrentState!=ulIDTargetState)  
		{
			pState=(CStateBase*)GetObjectByID(m_ulIDCurrentState);
			pState->OnEntry();
			SAFE_RELEASE(pState);
			m_ulIDSourceState=m_ulIDCurrentState;
		}
		else
			break; //Ya hemos inicializado a todos los superestados del Estado solicitado de transición
	}
	InitState(ulIDTargetState);
}
void CStateMachineManager::Dispatch(CEventBase* pEvent)
{
	m_ulIDSourceState=m_ulIDCurrentState;
	/*
		Enviar el evento hacia el estado actual, si el estado no lo procesa o lo delega
		invocar al superestado para que lo procese, y así en forma ascendente 
	*/

	while(m_ulIDSourceState)
	{
		CStateBase* pState=(CStateBase*)GetObjectByID(m_ulIDSourceState);
		if (pState)
		{
			m_ulIDSourceState = pState->OnEvent(pEvent);
		}
			
		else 
			m_ulIDSourceState=0;
		SAFE_RELEASE(pState);
	}
}
void CStateMachineManager::InitState(unsigned long ulIDState)
{
	m_ulIDCurrentState=ulIDState;
	while(1)
	{
		//Entrar en el estado y si se trata de un superestado, entonces consultar el estado inicial y entrar en el nuevo estado y así recurrentemente
		CStateBase* pState=(CStateBase*)GetObjectByID(m_ulIDCurrentState);
		pState->OnEntry();	
		m_ulIDSourceState=pState->InitialSubState();
		SAFE_RELEASE(pState);
		if(m_ulIDSourceState)
			m_ulIDCurrentState=m_ulIDSourceState;
		else 
			break;
	}
}
void CStateMachineManager::SetInitialState(unsigned long ulIDState)
{
	m_ulIDInitialState=ulIDState;
}
void CStateMachineManager::ClearLinks(void)
{
	m_mapSuperStates.clear();
}

void CStateMachineManager::PostEvent(CEventBase* pEvent)
{
	m_lstQueuedEvents.push_back(pEvent);
}
void CStateMachineManager::ProcessEvents(void)
{
	while(m_lstQueuedEvents.size())
	{
		CEventBase* pEvent=m_lstQueuedEvents.front();
		m_lstQueuedEvents.pop_front();
		Dispatch(pEvent);
		delete pEvent;
	}
}
