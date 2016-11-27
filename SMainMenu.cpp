#include "stdafx.h"
#include "SMainMenu.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
#include "SMain.h"
#include "Graphics\ImageBMP.h"
#include "SGame.h"

CSMainMenu::CSMainMenu()
{
	m_pSRVBackGround = NULL;
	m_pSRVMainOption1 = NULL;
	m_pSRVMainOption2 = NULL;
	m_pSRVSelectionBar = NULL;
	//m_pTextRender = NULL;
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
	{
		printf("Cargando recurso de Opcion 2...\n");
		fflush(stdout);
		auto img = CImageBMP::CreateBitmapFromFile("..\\Assets\\SelectionBar.bmp", NULL);
		if (!img)
		{
			printf("Recurso no encontrado\n");
			fflush(stdout);
		}
		else
		{
			auto tex = img->CreateTexture(MAIN->m_pDXManager);
			MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRVSelectionBar);
		}
	}
	MAIN->m_pSndManager->ClearEngine();
	m_pSndBackground = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\MenuSoundSmash.wav", SND_BACKGROUND);
	if (m_pSndBackground)
	{
		m_pSndBackground->Play(true);
	}
	MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\Explosion.wav", 1);
	MAIN->m_pDXPainter->m_Params.Brightness = { 0,0,0,0 };
	m_fOffsetX = 0.0f;
}

void CSMainMenu::OnExit()
{
	SAFE_RELEASE(m_pSRVBackGround);
	SAFE_RELEASE(m_pSRVMainOption1);
	SAFE_RELEASE(m_pSRVMainOption2);
	SAFE_RELEASE(m_pSRVSelectionBar);
}

unsigned long CSMainMenu::OnEvent(CEventBase* pEvent)
{
	if (ACTION_EVENT == pEvent->m_ulEventType)
	{
		CActionEvent* pInput = (CActionEvent*)pEvent;
		if (pInput->m_nAction == JOY_BUTTON_A_PRESSED)
		{
			//MAIN->m_pSndManager->PlayFx(1);
			auto Painter = MAIN->m_pDXPainter;
			auto Ctx = MAIN->m_pDXManager->GetContext();
			CDXBasicPainter::VERTEX Frame1[4]
			{
				{ { -0.5,2.0f / 5.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } }, // Primer vertice 
				{ { 0.5,2.0f / 5.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
				{ { -0.5,1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
				{ { 0.5,1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
			};
			unsigned long FrameIndex[6] = { 0,1,2,2,1,3 };
			Ctx->ClearDepthStencilView(MAIN->m_pDXManager->GetMainDTV(), D3D11_CLEAR_DEPTH, 1.0f, 0);
			Painter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV());
			Painter->m_Params.View = Painter->m_Params.World = Painter->m_Params.Projection = Identity(); // Nos aseguramos que este en 2D
			Painter->m_Params.Flags1 = MAPPING_EMISSIVE;
			Ctx->PSSetShaderResources(4, 1, &m_pSRVSelectionBar);
			Painter->DrawIndexed(Frame1, 4, FrameIndex, 6, 0);
			MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
			m_pSMOwner->Transition(CLSID_CSGame);
			InvalidateRect(MAIN->m_hWnd, NULL, false);
			return 0; // nunca se nosolvide retornar 0 para que no existe access violation
		}
		if (pInput->m_nAction == JOY_BUTTON_B_PRESSED)
		{
			auto Painter = MAIN->m_pDXPainter;
			auto Ctx = MAIN->m_pDXManager->GetContext();
			CDXBasicPainter::VERTEX Frame1[4]
			{
				{ { -0.5,-1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } }, // Primer vertice 
				{ { 0.5,-1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
				{ { -0.5,-2.0f / 5.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
				{ { 0.5,-2.0f / 5.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
			};
			unsigned long FrameIndex[6] = { 0,1,2,2,1,3 };
			Ctx->ClearDepthStencilView(MAIN->m_pDXManager->GetMainDTV(), D3D11_CLEAR_DEPTH, 1.0f, 0);
			Painter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV());
			Painter->m_Params.View = Painter->m_Params.World = Painter->m_Params.Projection = Identity(); // Nos aseguramos que este en 2D
			Painter->m_Params.Flags1 = MAPPING_EMISSIVE;
			Ctx->PSSetShaderResources(4, 1, &m_pSRVSelectionBar);
			Painter->DrawIndexed(Frame1, 4, FrameIndex, 6, 0);
			MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
			DestroyWindow(MAIN->m_hWnd);
		}
		if (JOY_AXIS_LX == pInput->m_nAction)
		{
			m_fOffsetX =  m_fOffsetX + (pInput->m_fAxis - m_fOffsetX)*0.01;
		}
		if (JOY_AXIS_LY == pInput->m_nAction)
		{
			m_fOffsetY = m_fOffsetY+(pInput->m_fAxis-m_fOffsetY)*0.01;
		}
	}
	// Todo lo que hagamos dentro de este if es nuestro tiempo libre
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		if (MAIN->m_pSndManager)
		{
			MAIN->m_pSndManager->RemoveAllSndFxStopped();
		}

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
		Painter->m_Params.World = Translation(m_fOffsetX, m_fOffsetY, 0);

		Painter->m_Params.Flags1 = MAPPING_EMISSIVE;
		Ctx->PSSetShaderResources(4, 1, &m_pSRVBackGround);
		Painter->DrawIndexed(Frame, 4, FrameIndex, 6, 0);

		Painter->m_Params.World = Identity();
		//Dibujar la opcion 1

		CDXBasicPainter::VERTEX Frame1[4]
		{
			{ { -0.5,2.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } }, // Primer vertice 
			{ { 0.5,2.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
			{ { -0.5,1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
			{ { 0.5,1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
		};

		Ctx->PSSetShaderResources(4, 1, &m_pSRVMainOption1);
		Painter->DrawIndexed(Frame1, 4, FrameIndex, 6, 0);


		//Dibujar Opcion2 
		CDXBasicPainter::VERTEX Frame2[4]
		{
			{ { -0.5,-1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } }, // Primer vertice 
			{ { 0.5,-1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
			{ { -0.5,-2.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
			{ { 0.5,-2.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
		};
		Ctx->PSSetShaderResources(4, 1, &m_pSRVMainOption2);
		Painter->DrawIndexed(Frame2, 4, FrameIndex, 6, 0);


		MATRIX4D ST = Translation(0.5,-0.5,0)* // Centro de caracter
			Scaling(0.05,0.06,1)* // Tamaño del caracter
			//RotationZ(3.141592/4)* //Orientacion del texto
			Translation(-1,1,0); //Posicion del texto

		//MAIN->m_pDXManager->GetContext()->OMSetBlendState(NULL, NULL, -1);
		MAIN->m_pTextRender->RenderText(ST, "Andres");
		MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);

		////return 0;

	}
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_CHAR:
			if ( 'o' == pWin32->m_wParam) // si es igual hacemos una transcicion
			{
				//MessageBoxA(NULL, "Vamos a transitar de S11 a S121", "hola", MB_ICONINFORMATION);
				auto Painter = MAIN->m_pDXPainter;
				auto Ctx = MAIN->m_pDXManager->GetContext();
				CDXBasicPainter::VERTEX Frame1[4]
				{
					{ { -0.5,2.0f / 5.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } }, // Primer vertice 
					{ { 0.5,2.0f / 5.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
					{ { -0.5,1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
					{ { 0.5,1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
				};
				unsigned long FrameIndex[6] = { 0,1,2,2,1,3 };
				Ctx->ClearDepthStencilView(MAIN->m_pDXManager->GetMainDTV(), D3D11_CLEAR_DEPTH, 1.0f, 0);
				Painter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV());
				Painter->m_Params.View = Painter->m_Params.World = Painter->m_Params.Projection = Identity(); // Nos aseguramos que este en 2D
				Painter->m_Params.Flags1 = MAPPING_EMISSIVE;
				Ctx->PSSetShaderResources(4, 1, &m_pSRVSelectionBar);
				Painter->DrawIndexed(Frame1, 4, FrameIndex, 6, 0);
				MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
				m_pSMOwner->Transition(CLSID_CSGame);
				InvalidateRect(MAIN->m_hWnd, NULL, false);
				return 0; // nunca se nosolvide retornar 0 para que no existe access violation
			}
			if ('l' == pWin32->m_wParam)
			{
				auto Painter = MAIN->m_pDXPainter;
				auto Ctx = MAIN->m_pDXManager->GetContext();
				CDXBasicPainter::VERTEX Frame1[4]
				{
					{ { -0.5,-1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } }, // Primer vertice 
					{ { 0.5,-1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
					{ { -0.5,-2.0f / 5.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
					{ { 0.5,-2.0f / 5.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
				};
				unsigned long FrameIndex[6] = { 0,1,2,2,1,3 };
				Ctx->ClearDepthStencilView(MAIN->m_pDXManager->GetMainDTV(), D3D11_CLEAR_DEPTH, 1.0f, 0);
				Painter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV());
				Painter->m_Params.View = Painter->m_Params.World = Painter->m_Params.Projection = Identity(); // Nos aseguramos que este en 2D
				Painter->m_Params.Flags1 = MAPPING_EMISSIVE;
				Ctx->PSSetShaderResources(4, 1, &m_pSRVSelectionBar);
				Painter->DrawIndexed(Frame1, 4, FrameIndex, 6, 0);
				MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
				DestroyWindow(MAIN->m_hWnd);
			}
			break;
		case WM_PAINT:
		{			
		}
		ValidateRect(MAIN->m_hWnd, NULL);

		}
	}
	return __super::OnEvent(pEvent);
}