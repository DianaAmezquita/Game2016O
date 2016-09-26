#pragma once
#include "eventbase.h"
#include <Windows.h>
#define EVENT_WIN32 0xffe1feb2
class CEventWin32 :
	public CEventBase
{
public:
	HWND m_hWnd;
	UINT m_msg;
	WPARAM m_wParam;
	LPARAM m_lParam;
public:
	CEventWin32(HWND,UINT,WPARAM,LPARAM);
	~CEventWin32(void);
};

