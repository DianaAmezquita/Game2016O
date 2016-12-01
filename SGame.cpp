#include "stdafx.h"
#include "SGame.h"
#include "SMain.h"
#include "Graphics\ImageBMP.h"
#include "SIntroduction.h"
#include "SGameOver.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
#include "ActionEvent.h"
#include <sstream>

/* assimp include files. */
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
CSGame::CSGame()
{
}

CSGame::~CSGame()
{
}

void CSGame::OnEntry(void)
{
	//MessageBoxA(NULL, "OnEntry", "SGame", MB_ICONINFORMATION);
	players[0].counterClicks = 0;
	players[1].counterClicks = 0;
	players[2].counterClicks = 0;
	players[3].counterClicks = 0;
	counterCLicks1 = 0;
	counterCLicks1 = 1;
	m_combination[0] = false;
	m_combination[1] = false;
	m_combination[2] = false;
	m_combination[3] = false;
	m_combination[4] = false;
	m_combination[5] = false;

	CSMain* p_main = (CSMain*)GetSuperState();

	
	m_pDXManager = p_main->m_pDXManager;
	m_pDXPainter = p_main->m_pDXPainter;
	VECTOR4D EyePos = { 6, 10, 15, 1 };
	VECTOR4D White = { 1, 1, 1, 1 };
	VECTOR4D Target = { 0, 0, 0, 1 };
	VECTOR4D Up = { 0, 0, 1, 0 };
	VECTOR4D Color = { 0, 0, 0, 0 };
	g_View = View(EyePos, Target, Up) * RotationZ(2.2);
	g_Projection = PerspectiveWidthHeightLH(0.05, 0.05, 0.1, 100);

	g_Surface[0].LoadSuzanne();
	g_Surface[0].BuildTangentSpaceFromTexCoordsIndexed(true);
	g_Surface[0].SetColor(White, White, White, White);

	g_Surface[1].LoadSuzanne();
	g_Surface[1].BuildTangentSpaceFromTexCoordsIndexed(true);
	g_Surface[1].SetColor(White, White, White, White);
	
	
	//g_BrigthnessP1.Brightness = { 1,1,1,1 };
	//g_BrigthnessP2.Brightness = { 1,1,1,1 };
	g_WorldPlayer1 = Translation(4, -3, -10)*Identity();
	g_WorldPlayer2 = Translation(-3, -3, 0)*Identity();

	players[0].world = Translation(4, -3, -10)*Identity();
	players[1].world = Translation(-3, -3, 0)*Identity();

	//g_Surface[0].BuildTangentSpaceFromTexCoordsIndexed(true);
	//g_Surface[1].BuildTangentSpaceFromTexCoordsIndexed(true);
	printf("Cargando recurso de fondo...\n");
	fflush(stdout);
	auto img = CImageBMP::CreateBitmapFromFile("..\\Assets\\Bowser.bmp", NULL);
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

	MAIN->m_pSndManager->ClearEngine();
	m_pSndBackground = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\OnGameSound.wav", SND_GAME);
	if (m_pSndBackground)
	{
		m_pSndBackground->Play(true);
	}
	MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\Blow.wav", 1);
	m_pDXPainter->m_Params.Brightness = { 0,0,0,0 };

}

unsigned long CSGame::OnEvent(CEventBase * pEvent)
{
	if (ACTION_EVENT == pEvent->m_ulEventType)
	{
		CActionEvent *Action = (CActionEvent*)pEvent;

		//if (Action->m_nSource == 0)
		//{
			if (JOY_BUTTON_A_PRESSED == Action->m_nAction)
			{
				players[Action->m_nSource].combination[0] = true;
				players[Action->m_nSource].combination[2] = true;
				m_combination[0] = true;
				m_combination[2] = true;
			}
			if (players[Action->m_nSource].combination[2] && JOY_BUTTON_B_PRESSED == Action->m_nAction)
			{
				players[Action->m_nSource].combination[2] = false;
				players[Action->m_nSource].combination[1] = true;
			}
		//}
		//if (Action->m_nSource == 1)
		//{
		//	if (JOY_BUTTON_A_PRESSED == Action->m_nAction)
		//	{
		//		m_combination[3] = true;
		//		m_combination[5] = true;
		//	}
		//	if (m_combination[5] && JOY_BUTTON_B_PRESSED == Action->m_nAction)
		//	{
		//		m_combination[5] = false;
		//		m_combination[4] = true;
		//	}
		//}
	}
	// Todo lo que hagamos dentro de este if es nuestro tiempo libre
	
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		if (MAIN->m_pSndManager)
		{
			MAIN->m_pSndManager->RemoveAllSndFxStopped();
		}
		if (m_pDXManager->GetSwapChain())
		{
			//// Crear Render Target Auxiliar
			//ID3D11Texture2D* pBackBuffer = 0;

			//// Lo corecto es preguntar a la cadena de intercambio (backbuffer)
			//// cual es el tamaño correcto
			//m_pDXManager->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);

			//D3D11_TEXTURE2D_DESC dtd;
			//// En estos momentos ya tengo la resolucion y ipo exactos
			//pBackBuffer->GetDesc(&dtd);
			//dtd.BindFlags |= (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

			//g_iWidth = dtd.Width;
			//g_iHeight = dtd.Height;
			//// Ahora creamos la textura 

			//SAFE_RELEASE(pBackBuffer);
			

			MATRIX4D AC = Scaling((float)g_iHeight / g_iWidth, 1, 1);
			VECTOR4D DarkGray = { 0.25, 0.25, 0.25, 1 };
			VECTOR4D White = { 1, 1, 1, 1 };
			VECTOR4D Gray = { .5, .5, .5, 0 };

			//m_pDXPainter->SetRenderTarget(m_pDXManager->GetMainRTV());
			//m_pDXPainter->m_Params.Material.Diffuse = Gray;
			//m_pDXPainter->m_Params.Material.Ambient = Gray;


			unsigned long FrameIndex[6] = { 0,1,2,2,1,3 };
			CDXBasicPainter::VERTEX Frame[4]
			{
				{ { -1,1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } }, // Primer vertice 
				{ { 1,1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
				{ { -1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
				{ { 1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
			};
			m_pDXManager->GetContext()->ClearDepthStencilView(m_pDXManager->GetMainDTV(), D3D11_CLEAR_DEPTH  , 1.0f, 0);
			m_pDXPainter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV());
			m_pDXPainter->m_Params.View = m_pDXPainter->m_Params.World = m_pDXPainter->m_Params.Projection = Identity(); // Dibujamos en 2d
			
			m_pDXPainter->m_Params.Flags1 = MAPPING_EMISSIVE;
			m_pDXManager->GetContext()->PSSetShaderResources(4, 1, &m_pSRVBackGround);


			VECTOR4D One = { 1, 1, 1, 1 };
			m_pDXManager->GetContext()->ClearRenderTargetView(MAIN->m_pDXManager->GetMainRTV(),(float*)&One);
			m_pDXPainter->DrawIndexed(Frame, 4, FrameIndex, 6, 0);


			m_pDXPainter->m_Params.View = g_View;
			m_pDXPainter->m_Params.Projection = g_Projection*AC;			

			//m_pDXPainter->m_Params.Flags1 = 0;//MAPPING_NORMAL_TRUE | MAPPING_DIFFUSE | MAPPING_EMISSIVE;


			//CDXBasicPainter::PARAMS old;
			//old = m_pDXPainter->m_Params;
			////VECTOR4D LightPos = { 0, 0, 40, 1 };
			////VECTOR4D Target = { 0, 0, 0, 1 };
			////VECTOR4D Up = { 0, 1, 0, 0 };

			//
			//// Dibujar mundo real
			////m_pDXPainter->m_Params.World = g_WorldPlayer1;
			//m_pDXPainter->m_Params.World = players[0].world;
			////m_pDXPainter->m_Params.Brightness = players[0].brightness;//g_BrigthnessP1.Brightness;
			////m_pDXPainter->DrawIndexed(&g_Surface[0].m_Vertices[0], g_Surface[0].m_Vertices.size(), &g_Surface[0].m_Indices[0], g_Surface[0].m_Indices.size(), PAINTER_DRAW);

			//
			//m_pDXPainter->m_Params.World = players[1].world;
			////m_pDXPainter->m_Params.Brightness = players[1].brightness;//g_BrigthnessP2.Brightness;
			////m_pDXPainter->DrawIndexed(&g_Surface[1].m_Vertices[0], g_Surface[1].m_Vertices.size(), &g_Surface[1].m_Indices[0], g_Surface[1].m_Indices.size(), PAINTER_DRAW);
			//
			//std::string s = std::to_string(players[0].counterClicks);
			//std::string s2 = std::to_string(players[1].counterClicks);
			//MATRIX4D ST = Translation(0.5, -0.5, 0)* // Centro de caracter
			//			  Scaling(0.05, 0.06, 1)*    // Tamaño del caracter
			//			  Translation(-1, 1, 0);     // Posicion del texto

			//
			//MATRIX4D ST2 = Translation(0.5, -0.5, 0)* // Centro de caracter
			//	Scaling(0.05, 0.06, 1)*				  // Tamaño del caracter
			//	Translation(0, 1, 0);                 //Posicion del texto
			//						   
			//const char *ss = s.c_str();
			//const char *ss2 = s2.c_str();
			////MAIN->m_pTextRender->RenderText(ST, ss);
			////MAIN->m_pTextRender->RenderText(ST2, ss2);
			//m_pDXPainter->m_Params = old;


			//if (players[0].combination[0] && players[0].combination[1])
			//{
			//	players[0].counterClicks++;
			//	//++counterCLicks1;
			//	MAIN->m_pSndManager->PlayFx(1);
			//	//g_BrigthnessP1.Brightness.x -= .04;
			//	//g_BrigthnessP1.Brightness.y -= .04;
			//	//g_BrigthnessP1.Brightness.z -= .04;

			//	//players[0].brightness.x -= .04;
			//	//players[0].brightness.y -= .04;
			//	//players[0].brightness.z -= .04;

			//	//m_pDXPainter->m_Params.Brightness = g_BrigthnessP1.Brightness;
			//	//m_pDXPainter->m_Params.Brightness = players[0].brightness;
			//	m_pDXPainter->m_Params.World = g_WorldPlayer1;
			//	players[0].world = Translation(4, -3, -10)*Scaling(players[0].world.m00 + 0.07, players[0].world.m11 + 0.04, players[0].world.m22 + 0.01);
			//	m_pDXPainter->m_Params.World = players[0].world;
			////	m_pDXPainter->DrawIndexed(&g_Surface[0].m_Vertices[0], g_Surface[0].m_Vertices.size(), &g_Surface[0].m_Indices[0], g_Surface[0].m_Indices.size(), PAINTER_DRAW);
			////	m_pDXManager->GetSwapChain()->Present(1, 0);

			//	players[0].combination[0] = false;
			//	players[0].combination[1] = false;
			//}

			//if (m_combination[3] && m_combination[4])
			//{
			//	++counterCLicks2;
			//	MAIN->m_pSndManager->PlayFx(1);
			//	g_BrigthnessP2.Brightness.x -= .04;
			//	g_BrigthnessP2.Brightness.y -= .04;
			//	g_BrigthnessP2.Brightness.z -= .04;

			//	m_pDXPainter->m_Params.Brightness = g_BrigthnessP2.Brightness;
			//	m_pDXPainter->m_Params.World = g_WorldPlayer2;
			//	g_WorldPlayer2 = Translation(-3, -3, 0)*Scaling(m_pDXPainter->m_Params.World.m00 + 0.14, m_pDXPainter->m_Params.World.m11 + 0.08, m_pDXPainter->m_Params.World.m22 + 0.04);
			//	m_pDXPainter->m_Params.World = g_WorldPlayer2;
			//	m_pDXPainter->DrawIndexed(&g_Surface[1].m_Vertices[0], g_Surface[1].m_Vertices.size(), &g_Surface[1].m_Indices[0], g_Surface[1].m_Indices.size(), PAINTER_DRAW);
			////	m_pDXManager->GetSwapChain()->Present(1, 0);
			//	m_combination[3] = false;
			//	m_combination[4] = false;
			//}

			//m_pDXPainter->m_Params = old;

			m_pDXManager->GetSwapChain()->Present(1, 0);
		}

		

		//m_pDXPainter->m_Params.World = g_WorldPlayer1;
		if (players[0].counterClicks == 21 || players[1].counterClicks == 21)
		{
			printf("Ya ganaste!!!");
			MAIN->win = true;
			m_pSMOwner->Transition(CLSID_CSGameOver);
			return 0; // nunca se nosolvide retornar 0 para que no existe access violation
		}

	}
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_CHAR:
			if ('b' == pWin32->m_wParam) // si es igual hacemos una transcicion
			{
				m_pSMOwner->Transition(CLSID_CSIntroduction);
				return 0; // nunca se nos olvide retornar 0 para que no existe access violation
			}
			if ('v' == pWin32->m_wParam)
			{
				++counterCLicks1;
				MAIN->m_pSndManager->PlayFx(1);
				m_pDXPainter->m_Params.Brightness.x -= .04;
				m_pDXPainter->m_Params.Brightness.y -= .04;
				m_pDXPainter->m_Params.Brightness.z -= .04;
				m_pDXPainter->m_Params.World = Scaling(m_pDXPainter->m_Params.World.m00 + 0.07, m_pDXPainter->m_Params.World.m11 + 0.04, m_pDXPainter->m_Params.World.m22 + 0.01);
				m_pDXPainter->DrawIndexed(&g_Surface[0].m_Vertices[0], g_Surface[0].m_Vertices.size(), &g_Surface[0].m_Indices[0], g_Surface[0].m_Indices.size(), PAINTER_DRAW);
				m_pDXManager->GetSwapChain()->Present(1, 0);
				m_combination[0] = false;
				m_combination[1] = false;
			}
			break;
		
		case WM_CLOSE:
			m_pSMOwner->Transition(CLSID_CStateNull);
			return 0;
		}
	}
	return __super::OnEvent(pEvent);
}
//
//void CSGame::LoadScene(char * filename)
//{
//	/* the global Assimp scene object */
//	const struct aiScene* scene = aiImportFile(filename, aiProcessPreset_TargetRealtime_Fast); //  aiProcessPreset_TargetRealtime_MaxQuality
//	
//	//g_Surface[].resize(scene->mNumMeshes);
//
//	for (unsigned long i = 0; i < scene->mNumMeshes; i++)
//	{
//		float maxX, maxY, maxZ;
//		float minX, minY, minZ;
//
//		maxX = maxY = maxZ = FLT_MIN;
//		minX = minY = minZ = FLT_MAX;
//
//		g_Surface[i].m_Vertices.resize(scene->mMeshes[i]->mNumVertices);
//		for (unsigned long j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
//		{
//			g_Surface[i].m_Vertices[j].Position = {
//				scene->mMeshes[i]->mVertices[j].x,
//				scene->mMeshes[i]->mVertices[j].y,
//				scene->mMeshes[i]->mVertices[j].z,
//				1 };
//			if (scene->mMeshes[i]->mVertices[j].x > maxX)
//				maxX = scene->mMeshes[i]->mVertices[j].x;
//			if (scene->mMeshes[i]->mVertices[j].y > maxY)
//				maxY = scene->mMeshes[i]->mVertices[j].y;
//			if (scene->mMeshes[i]->mVertices[j].z > maxZ)
//				maxZ = scene->mMeshes[i]->mVertices[j].z;
//
//			if (scene->mMeshes[i]->mVertices[j].x < minX)
//				minX = scene->mMeshes[i]->mVertices[j].x;
//			if (scene->mMeshes[i]->mVertices[j].y < minY)
//				minY = scene->mMeshes[i]->mVertices[j].y;
//			if (scene->mMeshes[i]->mVertices[j].z < minZ)
//				minZ = scene->mMeshes[i]->mVertices[j].z;
//		}
//
//
//
//		MATRIX4D t;
//		t.m00 = scene->mRootNode->mChildren[i]->mTransformation.a1;
//		t.m01 = scene->mRootNode->mChildren[i]->mTransformation.a2;
//		t.m02 = scene->mRootNode->mChildren[i]->mTransformation.a3;
//		t.m03 = scene->mRootNode->mChildren[i]->mTransformation.a4;
//		t.m10 = scene->mRootNode->mChildren[i]->mTransformation.b1;
//		t.m11 = scene->mRootNode->mChildren[i]->mTransformation.b2;
//		t.m12 = scene->mRootNode->mChildren[i]->mTransformation.b3;
//		t.m13 = scene->mRootNode->mChildren[i]->mTransformation.b4;
//		t.m20 = scene->mRootNode->mChildren[i]->mTransformation.c1;
//		t.m21 = scene->mRootNode->mChildren[i]->mTransformation.c2;
//		t.m22 = scene->mRootNode->mChildren[i]->mTransformation.c3;
//		t.m23 = scene->mRootNode->mChildren[i]->mTransformation.c4;
//		t.m30 = scene->mRootNode->mChildren[i]->mTransformation.d1;
//		t.m31 = scene->mRootNode->mChildren[i]->mTransformation.d2;
//		t.m32 = scene->mRootNode->mChildren[i]->mTransformation.d3;
//		t.m33 = scene->mRootNode->mChildren[i]->mTransformation.d4;
//
//		//g_Surface[i].m_World = Transpose(t);
//
//		g_Surface[i].m_Indices.resize(scene->mMeshes[i]->mNumFaces * scene->mMeshes[i]->mFaces[0].mNumIndices);
//		for (unsigned long j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
//		{
//			for (unsigned long k = 0; k < scene->mMeshes[i]->mFaces[j].mNumIndices; k++)
//			{
//				g_Surface[i].m_Indices[j*scene->mMeshes[i]->mFaces[j].mNumIndices + k] = scene->mMeshes[i]->mFaces[j].mIndices[k];
//			}
//		}
//
//		for (unsigned long j = 0; j < g_Surface[i].m_Vertices.size(); j++)
//		{
//			VECTOR4D TexCoord = { 0,0,0,0 };
//			TexCoord.x = g_Surface[i].m_Vertices[j].Position.x;
//			TexCoord.y = g_Surface[i].m_Vertices[j].Position.z;
//			TexCoord.z = g_Surface[i].m_Vertices[j].Position.y;
//			TexCoord = Normalize(TexCoord);
//			TexCoord.x = TexCoord.x * 0.5 + 0.5;
//			TexCoord.y = TexCoord.y * 0.5 + 0.5;
//
//			g_Surface[i].m_Vertices[j].TexCoord = TexCoord;
//		}
//		//g_Scene[i].Optimize();
//		g_Surface[i].BuildTangentSpaceFromTexCoordsIndexed(true);
//		//g_Surface[i].GenerarCentroides();
//
//		/* Set id */
//		//g_Surface[i].m_lID = i;
//		//strcpy(g_Surface[i].m_cName, scene->mMeshes[i]->mName.C_Str());
//	}
//}

void CSGame::OnExit(void)
{
	SAFE_RELEASE(pSRVNormalMap);
	SAFE_RELEASE(pSRVEnvMap);
	SAFE_RELEASE(pSRVNormalMapTrue);
	SAFE_RELEASE(pSRVEmissiveMap);
}
