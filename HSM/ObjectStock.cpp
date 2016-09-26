#include "StdAfx.h"
#include "ObjectStock.h"
#include "ObjectBase.h"
#include "ObjectCommand.h"
CObjectStock::CObjectStock(void)
{
}
CObjectStock::~CObjectStock(void)
{
	ReleaseAll();
}
CObjectBase* CObjectStock::AttachObject(
	unsigned long ulIDObject,
	CObjectBase* pObject)
{
	CObjectBase* pBackup=DetachObject(ulIDObject);
	pObject->AddRef();
	m_mapObjects.insert(make_pair(ulIDObject,pObject));
	return pBackup;
}
CObjectBase* CObjectStock::DetachObject(
	unsigned long ulIDObject)
{
	CObjectBase* pBackup=NULL;
	auto it=m_mapObjects.find(ulIDObject);
	if(it!=m_mapObjects.end())
	{
		pBackup=it->second;
		m_mapObjects.erase(it);
	}
	return pBackup;
}
CObjectBase* CObjectStock::GetObjectByID(unsigned long ulIDObject)
{
	CObjectBase* pObj=NULL;
	auto it=m_mapObjects.find(ulIDObject);
	if(it!=m_mapObjects.end())
		pObj=it->second;
	pObj->AddRef();
	return pObj;
}
void CObjectStock::ForEach(CObjectCommand* pCommand)
{
	for(auto it=m_mapObjects.begin();it!=m_mapObjects.end();it++)
	{
		if(!pCommand->DoCommand(it->second))
			break;
	}
}
void CObjectStock::ForEachInSet(set<unsigned long>& setIn, CObjectCommand* pCommand)
{
	CObjectBase* pObj=0;
	for(auto itS=setIn.begin();itS!=setIn.end();itS++)
	{
		pObj=GetObjectByID(*itS);
		if(pObj)
			if(!pCommand->DoCommand(pObj))
				break;
		SAFE_RELEASE(pObj);
	}
	SAFE_RELEASE(pObj);
}
void CObjectStock::ReleaseAll(void)
{
	for(auto it=m_mapObjects.begin();it!=m_mapObjects.end();it++)
		SAFE_RELEASE(it->second);
	m_mapObjects.clear();
}