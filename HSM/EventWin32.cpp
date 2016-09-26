#include "StdAfx.h"
#include "EventWin32.h"


CEventWin32::CEventWin32(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	m_ulEventType=EVENT_WIN32;
	m_msg=msg;
	m_hWnd=hWnd;
	m_wParam=wParam;
	m_lParam=lParam;
}


CEventWin32::~CEventWin32(void)
{
}
