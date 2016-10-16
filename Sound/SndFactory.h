#pragma once
#include <tchar.h>
class CSndObject;

class CSndFactory
{
public:
	CSndFactory(void);
	virtual ~CSndFactory(void);
public:
	virtual CSndObject* CreateObject(TCHAR* pszClassName);
	virtual void DestroyObject(CSndObject* pInObjectToDestroy);
};
