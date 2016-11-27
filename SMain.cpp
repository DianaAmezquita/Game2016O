#include "stdafx.h"
#include "SMain.h"
#include "stdio.h"
#include <iostream>
#include "InputProcessor.h"

CSMain::CSMain()
{
	m_hWnd = NULL;
	m_pDXManager = NULL;
	m_pDXPainter = NULL;
	m_pInputManager = NULL;
	m_pSndManager = NULL;
	m_pNetProcessor = NULL;
	m_pTextRender = NULL;
	win = false;
	m_bInitializationCorrect = true;
	printf("Iniciando Motores....\n");
	fflush(stdout);
	
}


CSMain::~CSMain()
{
}

void CSMain::OnEntry(void)
{
	m_pInputProcessor = new CInputProcessor(m_pSMOwner);
	m_pDXManager = new CDXManager();
	m_pDXPainter = new CDXBasicPainter(m_pDXManager);
	// Ahora tenemos que inicializar
	printf("Graphics Init");
	fflush(stdout);
	if(!m_pDXManager->Initialize(m_hWnd, CDXManager::EnumAndChooseAdapter(NULL)))
	{
		m_bInitializationCorrect = false;
		MessageBox(m_hWnd, L"No se pudo iniciar DirecX 11", L"Error", MB_ICONERROR);
	}
	if (!m_pDXPainter->Initialize())
	{
		m_bInitializationCorrect = false;
		MessageBox(m_hWnd, L"No se pudo iniciar Shaders", L"Error", MB_ICONERROR);
	}
	if (m_bInitializationCorrect)
	{
		printf("Success..");
		fflush(stdout);
	}
	printf("Sound Init");
	fflush(stdout);
	CSndFactory* pFactory = new CSndFactory();
	m_pSndManager = (CSndManager*)pFactory->CreateObject(L"CSndManager");
	if (!m_pSndManager->InitSoundEngine(m_hWnd))
	{
		m_bInitializationCorrect = false;
	}
	if (m_bInitializationCorrect)
	{
		printf("Success\n");
		fflush(stdout);
	}
	else
	{
		printf("BAD ):....");
		fflush(stdout);
	}
	printf("Input init ...\n");
	fflush(stdout);
	m_pInputManager = new CInputManager();
	if (!m_pInputManager->InitializeDirectInputSession(m_hInstance))
	{
		printf("Bad input init...\n");
		m_bInitializationCorrect = false;
	}
	else if (!m_pInputManager->ConfigureDevices(m_hWnd))
	{
		printf("Unable to aquire input devices ... \n");
		m_bInitializationCorrect = false;
	}
	else
	{
		printf("OK\n");
	}
	printf("Initializing network\n");
	m_pNetProcessor = new CNetProcessor(m_pSMOwner);
	if (!m_pNetProcessor->InitNetwork())
	{
		printf("Unable to work as server, connections are allowed instead\n");
	}
	fflush(stdout);
	printf("Initializing Text Render...\n");
	m_pTextRender = new CDXTextRenderer(m_pDXManager, m_pDXPainter);
	if (!m_pTextRender->Initialize())
	{
		printf("Unable to initialize text renderizer..\n");
	}
	else
	{
		printf("Initialization of text correct =)\n");
	}
	fflush(stdout);

	
}
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
unsigned long CSMain::OnEvent(CEventBase * pEvent)
{
	// Todo lo que hagamos dentro de este if es nuestro tiempo libre
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		if (m_pSndManager)
		{
			m_pSndManager->RemoveAllSndFxStopped();
		}
		for (int iSource = 0; iSource < m_pInputManager->GetDeviceCount(); iSource++)
		{
			// Ya no vamos hacer distpach porque ya no lo queremos hacer directamente
			DIJOYSTATE2 js2;
			if (m_pInputManager->ReadState(js2, iSource))
			{
				CInputEvent *pInput = new CInputEvent(iSource, 0, js2); // 0 como estampa de tiempo
				m_pSMOwner->PostEvent(pInput); // Ya no vamos hacer dispatch
				m_pInputProcessor->OnEvent(pInput);
			}
		}
	}
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(m_hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(m_hWnd, &ps);
			break;
		}
		case WM_CLOSE:
			m_pSMOwner->Transition(CLSID_CStateNull);
			return 0;
		case WM_CHAR:
			switch (pWin32->m_lParam)
			{
			case 'c':
			case 'C':
				MAIN->m_pNetProcessor->Connect(L"127.0.0.1");
			}
			break;
		}
		
	}
	return __super::OnEvent(pEvent);
}

void CSMain::OnExit(void)
{
	m_pDXPainter->Uninitialize();
	m_pDXManager->Uninitialize();
	m_pSndManager->UnitializeSoundEngine();
	m_pInputManager->FinalizeDirectInputSession();
	m_pTextRender->Uninitialize();
	SAFE_DELETE(m_pDXPainter);
	SAFE_DELETE(m_pDXManager);
	CSndFactory Factory;
	Factory.DestroyObject(m_pSndManager);
	SAFE_DELETE(m_pInputManager);
	SAFE_DELETE(m_pInputProcessor);
	SAFE_DELETE(m_pTextRender);
}
