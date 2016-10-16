#include "StdAfx.h"
#include "SndFactory.h"
#include "SndFx.h"
#include "SndManager.h"
#include <string.h>

CSndManager* g_pSndManager=NULL;

CSndFactory::CSndFactory(void)
{
}

CSndFactory::~CSndFactory(void)
{
}


CSndObject* CSndFactory::CreateObject(TCHAR *pszClassName)
{
	if(g_pSndManager==NULL)
		g_pSndManager=new CSndManager(this);
	if(wcscmp(pszClassName,TEXT("CSndFx"))==0)
		return new CSndFx(g_pSndManager);
	else if(wcscmp(pszClassName,TEXT("CSndManager"))==0)
		return g_pSndManager;
	return NULL;
}
void CSndFactory::DestroyObject(CSndObject* pInObjectToDestroy)
{
	delete pInObjectToDestroy;
}
