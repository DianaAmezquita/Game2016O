#include "stdafx.h"
#include "SMainMenu.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
#include "SMain.h"
#include "Graphics\ImageBMP.h"
CSMainMenu::CSMainMenu()
{
	m_pSRVBackGround = NULL;
	m_pSRVMainOption1 = NULL;
	m_pSRVMainOption2 = NULL;
}


CSMainMenu::~CSMainMenu()
{
	m_pSRVBackGround;
	ID3D11ShaderResourceView* m_pSRVMainOption1;
	ID3D11ShaderResourceView* m_pSRVMainOption2;
}

void CSMainMenu::OnEntry()
{
	m_nOption = 0;
	printf("Iniciando menu principal...\n");
	fflush(stdout);
	{
		printf("Cargando recurso de fondo...\n");
		fflush(stdout);
		auto img = CImageBMP::CreateBitmapFromFile("..\\Assets\\MainMenu.bmp", NULL);
		if (!img)
		{
			printf("Recurso no encontrado\n");
			fflush(stdout);
		}
		else
		{
			auto tex = img->CreateTexture(MAIN->m_pDXManager);
			MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRVBackGround);
		}
	}
	{
		printf("Cargando recurso de opcion 1...\n");
		fflush(stdout);
		auto img = CImageBMP::CreateBitmapFromFile("..\\Assets\\MainOption1.bmp", NULL);
		if (!img)
		{
			printf("Recurso no encontrado\n");
			fflush(stdout);
		}
		else
		{
			auto tex = img->CreateTexture(MAIN->m_pDXManager);
			MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRVMainOption1);
		}
	}
	{
		printf("Cargando recurso de Opcion 2...\n");
		fflush(stdout);
		auto img = CImageBMP::CreateBitmapFromFile("..\\Assets\\MainOption2.bmp", NULL);
		if (!img)
		{
			printf("Recurso no encontrado\n");
			fflush(stdout);
		}
		else
		{
			auto tex = img->CreateTexture(MAIN->m_pDXManager);
			MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRVMainOption2);
		}
	}
}

void CSMainMenu::OnExit()
{
	SAFE_RELEASE(m_pSRVBackGround);
	SAFE_RELEASE(m_pSRVMainOption1);
	SAFE_RELEASE(m_pSRVMainOption2);
}

unsigned long CSMainMenu::OnEvent(CEventBase* pEvent)
{
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_CHAR:
			break;
		case WM_PAINT:
		{
			auto Painter = MAIN->m_pDXPainter;
			auto Ctx = MAIN->m_pDXManager->GetContext();
			CDXBasicPainter::VERTEX Frame[4]
			{
				{ { -1,1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } }, // Primer vertice 
				{ { 1,1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
				{ { -1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
				{ { 1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
			};
			unsigned long FrameIndex[6] = { 0,1,2,2,1,3 };
			Ctx->ClearDepthStencilView(MAIN->m_pDXManager->GetMainDTV(), D3D11_CLEAR_DEPTH, 1.0f, 0);
			Painter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV());
			Painter->m_Params.View = Painter->m_Params.World = Painter->m_Params.Projection = Identity(); // Nos aseguramos que este en 2D
			Painter->m_Params.Flags1 = MAPPING_EMISSIVE;
			Ctx->PSSetShaderResources(4, 1, &m_pSRVBackGround);
			Painter->DrawIndexed(Frame, 4, FrameIndex, 6, 1);

			//Dibujar la opcion 1

			CDXBasicPainter::VERTEX Frame1[4]
			{
				{ { -0.5,2.0f/3.0f,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } }, // Primer vertice 
				{ {  0.5,2.0f/3.0f,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
				{ { -0.5,1.0f/3.0f,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
				{ {  0.5,1.0f/3.0f,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
			};

			Ctx->PSSetShaderResources(4, 1, &m_pSRVMainOption1);
			Painter->DrawIndexed(Frame1, 4, FrameIndex, 6, 1);
			//Dibujar Opcion2 
			CDXBasicPainter::VERTEX Frame2[4]
			{
				{ { -0.5,-1.0f / 3.0f,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } }, // Primer vertice 
				{ {  0.5,-1.0f / 3.0f,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
				{ { -0.5,-2.0f / 3.0f,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
				{ {  0.5,-2.0f / 3.0f,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
			};
			Ctx->PSSetShaderResources(4, 1, &m_pSRVMainOption2);
			Painter->DrawIndexed(Frame2, 4, FrameIndex, 6, 1);
			MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
		}
		ValidateRect(MAIN->m_hWnd, NULL);

		}
	}
	return __super::OnEvent(pEvent);
}