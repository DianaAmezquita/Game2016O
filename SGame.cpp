#include "stdafx.h"
#include "SGame.h"
#include "SMain.h"
#include "Graphics\ImageBMP.h"
#include "SIntroduction.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
CSGame::CSGame()
{
}


CSGame::~CSGame()
{
}

void CSGame::OnEntry(void)
{
	//MessageBoxA(NULL, "OnEntry", "SGame", MB_ICONINFORMATION);
	CSMain* p_main = (CSMain*)GetSuperState();
	
	m_pDXManager = p_main->m_pDXManager;
	m_pDXPainter = p_main->m_pDXPainter;
	VECTOR4D EyePos = { 6, 10, 6, 1 };
	VECTOR4D White = { 1, 1, 1, 1 };
	VECTOR4D Target = { 0, 0, 0, 1 };
	VECTOR4D Up = { 0, 0, 1, 0 };

	g_View = View(EyePos, Target, Up);
	g_Projection = PerspectiveWidthHeightLH(0.05, 0.05, 0.1, 100);

	g_Surface.LoadSuzanne();
	g_Surface.BuildTangentSpaceFromTexCoordsIndexed(true);
	g_Surface.SetColor(White, White, White, White);

	g_pSysTexture = CImageBMP::
		CreateBitmapFromFile("..\\Assets\\Koala256.bmp", NULL);
	if (!g_pSysTexture)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
	}
	g_pTexture = g_pSysTexture->CreateTexture(m_pDXManager);
	if (!g_pTexture)
	{
		MessageBox(NULL, L"No se pudo cargar textura al GPU",
			L"Verificar recursos sombreadores", MB_ICONERROR);
	}
	// Abajo se ponia la de ladrillo
	CImageBMP* pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\ladrillo.bmp", NULL);
	if (!pImage)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
	}

	g_pNormalMap = pImage->CreateTexture(m_pDXManager);
	if (!g_pNormalMap)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
	}
	CImageBMP::DestroyBitmap(pImage);

	pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\SanPedro.bmp", NULL);

	if (!pImage)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
	}

	g_pEnvMap = pImage->CreateTexture(m_pDXManager);

	if (!g_pEnvMap)
	{
		MessageBox(NULL, L"No se pudo cargar textura al GPU",
			L"Verificar recursos sombreadores", MB_ICONERROR);
	}

	CImageBMP::DestroyBitmap(pImage);

	pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\Normal.bmp", NULL);

	if (!pImage)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
	}

	g_pNormalMapTrue = pImage->CreateTexture(m_pDXManager);

	if (!g_pNormalMapTrue)
	{
		MessageBox(NULL, L"No se pudo cargar textura al GPU",
			L"Verificar recursos sombreadores", MB_ICONERROR);
	}

	CImageBMP::DestroyBitmap(pImage);

	pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\Emissive.bmp", NULL);

	if (!pImage)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
	}

	g_pEmissiveMap = pImage->CreateTexture(m_pDXManager);

	if (!g_pEmissiveMap)
	{
		MessageBox(NULL, L"No se pudo cargar textura al GPU",
			L"Verificar recursos sombreadores", MB_ICONERROR);
	}

	CImageBMP::DestroyBitmap(pImage);

}


unsigned long CSGame::OnEvent(CEventBase * pEvent)
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
				m_pSMOwner->Transition(CLSID_CSIntroduction);
				return 0; // nunca se nosolvide retornar 0 para que no existe access violation
			}
			break;
		case WM_PAINT:
		{
			if (m_pDXManager->GetSwapChain())
			{
				// Crear Render Target Auxiliar
				ID3D11Texture2D* pBackBuffer = 0;

				// Lo corecto es preguntar a la cadena de intercambio (backbuffer)
				// cual es el tama;o correcto
				m_pDXManager->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);

				D3D11_TEXTURE2D_DESC dtd;
				// En estos momentos ya tengo la resolucion y ipo exactos
				pBackBuffer->GetDesc(&dtd);
				dtd.BindFlags |= (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

				g_iWidth = dtd.Width;
				g_iHeight = dtd.Height;
				// Ahora creamos la textura 
				
				SAFE_RELEASE(pBackBuffer);


				MATRIX4D AC = Scaling((float)g_iHeight / g_iWidth, 1, 1);
				VECTOR4D DarkGray = { 0.25, 0.25, 0.25, 1 };
				VECTOR4D White = { 1, 1, 1, 1 };
				VECTOR4D Gray = { .5, .5, .5, 0 };


				m_pDXPainter->SetRenderTarget(m_pDXManager->GetMainRTV());
				m_pDXPainter->m_Params.Material.Diffuse = Gray;
				m_pDXPainter->m_Params.Material.Ambient = Gray;

				VECTOR4D NightBlue = { 0.3, 0, .5, 0 };

				m_pDXManager->GetContext()->ClearRenderTargetView(m_pDXManager->GetMainRTV(), (float*)&NightBlue);//g_Manager.GetContext()->ClearRenderTargetView(g_pRTV0, (float*)&NightBlue); //(g_Manager.GetMainRTV(), (float*)&NightBlue);
				m_pDXManager->GetContext()->ClearDepthStencilView(m_pDXManager->GetMainDTV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
				unsigned long TriangleIndices[3] = { 0, 1, 2 };
				g_World = Identity();// RotationZ(theta);
				m_pDXPainter->m_Params.World = g_World; //* RotationY(theta);
				m_pDXPainter->m_Params.View = g_View;
				m_pDXPainter->m_Params.Projection = g_Projection*AC;



				VECTOR4D Color = { 0, 0, 0, 0 };
				m_pDXPainter->m_Params.Brightness = Color;
				m_pDXPainter->m_Params.Flags1 = 0;//MAPPING_NORMAL_TRUE | MAPPING_DIFFUSE | MAPPING_EMISSIVE;
				
				m_pDXManager->GetDevice()->CreateShaderResourceView(g_pTexture, NULL, &pSRV);
				m_pDXManager->GetContext()->PSSetShaderResources(0, 1, &pSRV);
				
				m_pDXManager->GetDevice()->CreateShaderResourceView(g_pNormalMap, NULL, &pSRVNormalMap);
				m_pDXManager->GetContext()->PSSetShaderResources(1, 1, &pSRVNormalMap);

				
				m_pDXManager->GetDevice()->CreateShaderResourceView(g_pEnvMap, NULL, &pSRVEnvMap);
				m_pDXManager->GetContext()->PSSetShaderResources(2, 1, &pSRVEnvMap);

				
				m_pDXManager->GetDevice()->CreateShaderResourceView(g_pNormalMapTrue, NULL, &pSRVNormalMapTrue);
				m_pDXManager->GetContext()->PSSetShaderResources(3, 1, &pSRVNormalMapTrue);

				
				m_pDXManager->GetDevice()->CreateShaderResourceView(g_pEmissiveMap, NULL, &pSRVEmissiveMap);
				m_pDXManager->GetContext()->PSSetShaderResources(4, 1, &pSRVEmissiveMap);

				// Necesitamos un lano para la vitrina refejante ubicado 4 unidades hacia abajo
				VECTOR4D Plane = { 0, 0, 1, 5 };
				//VECTOR4D Plane = { 0, 0, sin(2), cos(5) };
				MATRIX4D Mirror = ReflectionMatrix(Plane); //Una matrix que me represente al espejo
														   // Voy a rescatar la vieja matrix de vista. 
														   // Porque la voy a necesitar desppues y la voy a modificar ahora
				MATRIX4D OldView = m_pDXPainter->m_Params.View;
				VECTOR4D LightPos = { 0, 0, 40, 1 };
				VECTOR4D Target = { 0, 0, 0, 1 };
				VECTOR4D Up = { 0, 1, 0, 0 };
				m_pDXPainter->m_Params.LightView = View(LightPos, Target, Up);
				m_pDXPainter->m_Params.LightProjection = PerspectiveWidthHeightLH(1, 1, 1, 100);

				m_pDXPainter->m_Params.View = OldView;
				m_pDXManager->GetContext()->RSSetState(m_pDXPainter->GetDrawLHRState());


				// Dibujar mundo real
				m_pDXPainter->DrawIndexed(&g_Surface.m_Vertices[0], g_Surface.m_Vertices.size(), &g_Surface.m_Indices[0], g_Surface.m_Indices.size(), PAINTER_DRAW);
				m_pDXManager->GetSwapChain()->Present(1, 0);


			}
			//ValidateRect(hWnd, NULL);
			break;
		}
		case WM_CLOSE:
			m_pSMOwner->Transition(CLSID_CStateNull);
			return 0;
		}
	}
	return __super::OnEvent(pEvent);
}

void CSGame::OnExit(void)
{
	SAFE_RELEASE(pSRVNormalMap);
	SAFE_RELEASE(pSRVEnvMap);
	SAFE_RELEASE(pSRVNormalMapTrue);
	SAFE_RELEASE(pSRVEmissiveMap);
}
