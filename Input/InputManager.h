#pragma once
#define MAX_INPUTDEVICES 8
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
//Al momento de importar estos archivos será necesario enlazarlos con dinput8.lib y dxguid.lib
class CInputManager
{
protected:
	IDirectInput8 *m_pIDI;
	IDirectInputDevice8* m_pIDIDevice[MAX_INPUTDEVICES];
	int m_nDIDeviceCount;
	static BOOL CALLBACK MyDIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi,LPVOID pvRef);
public:
	bool InitializeDirectInputSession(HINSTANCE hInstance);
	bool ConfigureDevices(HWND hWnd);
	void FinalizeDirectInputSession(void);
	bool ReadState(DIJOYSTATE2& JoyState,int nDeviceIndex);
	int  GetDeviceCount();
	CInputManager(void);
	~CInputManager(void);
};



