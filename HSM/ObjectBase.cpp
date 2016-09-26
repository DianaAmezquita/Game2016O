#include "StdAfx.h"
#include "ObjectBase.h"

//Master instance counter
unsigned long CObjectBase::s_ulInstanceCount=0;
CObjectBase::CObjectBase(void)
{
	s_ulInstanceCount++;  
	m_ulRefCount=1; //Regla 1
	m_ulObjectID=0;
}

unsigned long CObjectBase::AddRef(void)
{
	return ++m_ulRefCount;
}
unsigned long CObjectBase::Release(void)
{
	unsigned long ulReminder=--m_ulRefCount;
	if(m_ulRefCount==0)
		delete this;
	return ulReminder;
}
CObjectBase::~CObjectBase(void)
{
	s_ulInstanceCount--;
}

unsigned long& CObjectBase::ID()
{
	return m_ulObjectID;
}