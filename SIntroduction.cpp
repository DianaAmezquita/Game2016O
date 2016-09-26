#include "stdafx.h"
#include "SIntroduction.h"
#include "SMain.h"
#include "Graphics\DXBasicPainter.h"
#include "Graphics\DXManager.h"
#include "SGame.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
#include "Graphics\ImageBMP.h"
#include "SMainMenu.h"

CSIntroduction::CSIntroduction()
{
}


CSIntroduction::~CSIntroduction()
{
}

void CSIntroduction::OnEntry(void)
{
	CSMain* p_main = (CSMain*)GetSuperState();
	
	m_pDXManager = p_main->m_pDXManager;
	m_pDXPainter = p_main->m_pDXPainter;
	CImageBMP*      g_pSysTexture; //CPU


	m_pEffects = new CFX(m_pDXManager);

	if (!m_pEffects->Initialize())
	{
		MessageBox(NULL, L"No se pudo iniciar el FX",
			L"No se pudo Iniciar", MB_ICONERROR);
		// Preguntar al corni si nos regreamos al estado nulo
	}
	g_pSysTexture = CImageBMP::CreateBitmapFromFile("..\\Assets\\Koala256.bmp", NULL);

	m_pImagIntroduction = g_pSysTexture->CreateTexture(m_pDXManager);

	SetTimer(MAIN->m_hWnd, 1, 5000, NULL); // el 1 es el identificador del timer es local entre ventanas
	/*
	//Aqui lo tenemos en memoria del sistema
	CImageBMP* pIntro = CImageBMP::CreateBitmapFromFile("..\\Assets\\Koala256.bmp", NULL);
	//CreateTexture carga al GPU
	m_pSplash = pIntro->CreateTexture(MAIN->m_pDXManager);
	//Ya no queremos la memoria del sistema
	CImageBMP::DestroyBitmap(pIntro);
	
	MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(m_pSplash, NULL, &m_pSRVSplash);*/
	// Me asegurp de que no existan luces prendidas de otro estado

}


unsigned long CSIntroduction::OnEvent(CEventBase * pEvent)
{
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_CHAR:
			if ('b' == pWin32->m_wParam) // si es igual hacemos una transcicion
			{
				//MessageBoxA(NULL, "Vamos a transitar de S11 a S121", GetClassName(), MB_ICONINFORMATION);
				m_pSMOwner->Transition(CLSID_CSGame);
				return 0; // nunca se nosolvide retornar 0 para que no existe access violation
			}
			break;
		case WM_PAINT:
		{
			/*
			// ESTO LO HICIMOS EN CLASE
			CDXBasicPainter* pPainter = MAIN->m_pDXPainter;
			pPainter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV(),NULL);
			CDXBasicPainter::VERTEX Frame[4]
			{
				{ { -1,1,0,1},{ 0,0,0,0},{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } }, // Primer vertice 
				{ { 1,1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
				{ { -1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
				{ { 1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
			};
			unsigned long FrameIndex[6] = { 0,1,2,2,1,3 };
			pPainter->m_Params.Projection = pPainter->m_Params.View = pPainter->m_Params.World = Identity();
			MAIN->m_pDXManager->GetContext()->PSSetShader(3, 1, &m_pSRVSplash);
			pPainter->m_Params.Flags1 = MAPPING_EMISSIVE ;
			pPainter->DrawIndexed(Frame, 4, FrameIndex, 6);
			MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
			*/


			ID3D11Texture2D* pBackBuffer = 0;
			m_pDXManager->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);

			D3D11_TEXTURE2D_DESC dtd;
			// En estos momentos ya tengo la resolucion y ipo exactos
			pBackBuffer->GetDesc(&dtd);

			ID3D11ShaderResourceView* pSRV = NULL;
			m_pDXManager->GetDevice()->CreateShaderResourceView(m_pImagIntroduction, NULL, &pSRV);
			m_pDXManager->GetContext()->PSSetShaderResources(0, 1, &pSRV);

			m_pEffects->SetRenderTarget(m_pDXManager->GetMainRTV());
			m_pEffects->SetInput(pSRV);
			m_pEffects->Process(0, dtd.Width, dtd.Height);

			m_pDXManager->GetSwapChain()->Present(1, 0);

			SAFE_RELEASE(pSRV);
			
		}
		return 0;
		case WM_TIMER:
			if (1 == pWin32->m_wParam)
			{
				m_pSMOwner->Transition(CLSID_CSMainMenu);
				InvalidateRect(MAIN->m_hWnd, NULL, false);
				return 0;
			}

		case WM_CLOSE:
			m_pSMOwner->Transition(CLSID_CStateNull);
			return 0;
		}
	}
	return __super::OnEvent(pEvent);
}

void CSIntroduction::OnExit(void)
{
	KillTimer(MAIN->m_hWnd, 1);
	//m_pDXPainter->Uninitialize();
	//m_pDXManager->Uninitialize();
	//SAFE_DELETE(m_pDXPainter);
	//SAFE_DELETE(m_pDXManager);
	//MessageBoxA(NULL, "OnExit", "SIntroduction", MB_ICONINFORMATION);
	SAFE_DELETE(m_pEffects);
}

