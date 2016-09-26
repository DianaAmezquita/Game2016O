#pragma once
#include "ObjectBase.h"
#include "EventListener.h"

class CStateBase:
	public CObjectBase,
	public CEventListener
{
protected:
	friend class CStateMachineManager;
	CStateMachineManager* m_pSMOwner;
	unsigned long m_ulInitialSubState;
	CStateMachineManager*& SMOwner(void){return m_pSMOwner;}
	unsigned long& InitialSubState(void){return m_ulInitialSubState;}
	CStateBase* GetSuperState(void);
protected:
	//Cada estado debe sobrecargar los siguientes métodos
	//Al entrar al estado y antes de recibir el primer evento
	virtual void			OnEntry()=0;
	//Al salir del estado y despues de recibir el último evento.
	virtual void			OnExit()=0;
	//Al recibir un evento, procesarlo y de ser necesario hacer una transición
	virtual unsigned long	OnEvent(CEventBase* pEvent);
public:
	CStateBase(void);
	~CStateBase(void);
};

