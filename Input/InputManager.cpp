#include "StdAfx.h"
#include "InputManager.h"

CInputManager::CInputManager(void)
{
	m_pIDI=NULL;
	m_nDIDeviceCount=0;
	memset(m_pIDIDevice,0,sizeof(m_pIDIDevice));
}


CInputManager::~CInputManager(void)
{
	//Tarea, terminar el destructor.
}
#include <stdio.h>
//Función que se invoca cada vez que DIEnumDevices encuentra un nuevo periférico
BOOL CALLBACK CInputManager::MyDIEnumDevicesCallback(
         LPCDIDEVICEINSTANCE lpddi,
         LPVOID pvRef)
{
	CInputManager* pManager=(CInputManager*)pvRef;
	TCHAR szMessage[1024];
	wsprintf(szMessage,TEXT("Product Name:%s\n\rModel Name:%s"),
		lpddi->tszProductName,lpddi->tszInstanceName);
	wprintf(L"%s\n\r",szMessage);
	pManager->m_pIDI->CreateDevice(lpddi->guidInstance,
		&pManager->m_pIDIDevice[pManager->m_nDIDeviceCount],
		NULL);
	return ++pManager->m_nDIDeviceCount < MAX_INPUTDEVICES ? DIENUM_CONTINUE : DIENUM_STOP;
}
bool CInputManager::InitializeDirectInputSession(HINSTANCE hInstance)
{
	HRESULT hr=DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,(void**)&m_pIDI,NULL);
	if(FAILED(hr))
		return false;
	for(int i=0;i<m_nDIDeviceCount;i++)
	{
		m_pIDIDevice[i]->Release();
		m_pIDIDevice[i]=NULL;
	}
	m_pIDI->EnumDevices(DI8DEVCLASS_GAMECTRL,MyDIEnumDevicesCallback,
		this,DIEDFL_ATTACHEDONLY);
	return true;
}
bool CInputManager::ConfigureDevices(HWND hWnd)
{
	HRESULT hr=S_OK;
	for(int i=0;i<m_nDIDeviceCount;i++)
	{
		hr=m_pIDIDevice[i]->SetCooperativeLevel(hWnd,DISCL_EXCLUSIVE);
		hr=m_pIDIDevice[i]->SetDataFormat(&c_dfDIJoystick2);
		if(FAILED(hr))
			return false;
	}
	return true;
}
void CInputManager::FinalizeDirectInputSession(void)
{
	if(m_pIDI)
	{
		for(int i=0;i<m_nDIDeviceCount;i++)
		{
			m_pIDIDevice[i]->Release();
			m_pIDIDevice[i]=NULL;
		}
		m_pIDI->Release();
		m_pIDI=NULL;
	}
}
bool CInputManager::ReadState(DIJOYSTATE2& JoyState,int nDeviceIndex)
{
	if(nDeviceIndex<m_nDIDeviceCount) //Bug FIXED. Out of range index device.
	{
		HRESULT hr=m_pIDIDevice[nDeviceIndex]->Acquire();
		if(SUCCEEDED(hr))
		{
			hr=m_pIDIDevice[nDeviceIndex]->GetDeviceState(sizeof(DIJOYSTATE2),&JoyState);
			if(SUCCEEDED(hr))
				return true;
			else
				printf("UK");
		}
		else
			printf("UA");
	}
	return false;
}
int  CInputManager::GetDeviceCount()
{
	return m_nDIDeviceCount;
}
