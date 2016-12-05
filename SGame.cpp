#include "stdafx.h"
#include "SGame.h"
#include "SMain.h"
#include "Graphics\ImageBMP.h"
#include "SIntroduction.h"
#include "SGameOver.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
#include "ActionEvent.h"
#include "Input\InputManager.h"
#include <sstream>
#include <iostream>
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
	// Se obtiene el CSMain del estado padre
	CSMain* p_main = (CSMain*)GetSuperState();

	// Se establecen los vectores de la posicion de la camara
	// Colores para la malla, altitud y del target.
	VECTOR4D EyePos = { 10, 10, 30, 1 };
	VECTOR4D White = { 1, 1, 1, 1 };
	VECTOR4D Target = { 0, 0, 0, 1 };
	VECTOR4D Up = { 0, 0, 1, 0 };

	// Se obtiene el m_pDXManager y m_pDXPainter del main
	m_pDXManager = p_main->m_pDXManager;
	m_pDXPainter = p_main->m_pDXPainter;

	// Se inicializan las posiciones de los contadores 
	initializeCounterPositions();
	// Se inicializan las posiciones de los jugadores
	initializePositions();

	// Se establece la matrix de vista y de proyeccion para el juego en general
	g_View = View(EyePos, Target, Up);
	g_Projection = PerspectiveWidthHeightLH(0.05, 0.05, 0.1, 100);

	// Se cargan las mallas de todos los jugadores
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		//players[i].g_Surface.LoadSuzanne();
		//players[i].g_Surface.BuildTangentSpaceFromTexCoordsIndexed(true);
		LoadScene("..\\Assets\\Toad.blend", i);
		//players[i].g_Surface.SetColor(White, White, White, White);
		players[i].world =  Translation(players[i].position.x, players[i].position.y, players[i].position.z) * Scaling(.05,.05,.05)*RotationZ(3.9)*Identity();
		players[i].scalationFactor = players[i].world;
	}
	
	// Se carga la imagen bmp en un recurso de fondo de tipo  CImageBMP
	printf("Cargando recurso de fondo...\n");
	fflush(stdout);
	auto img = CImageBMP::CreateBitmapFromFile("..\\Assets\\Bowser_4By4_Blocks_Character04.bmp", NULL);
	if (!img){
		printf("Recurso no encontrado\n");
		fflush(stdout);
	}
	else{
		auto tex = img->CreateTexture(MAIN->m_pDXManager);
		MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRVBackGround);
	}
	img = CImageBMP::CreateBitmapFromFile("..\\Assets\\Bowser_4By4_Blocks_Character04_02.bmp", NULL);
	if (!img){
		printf("Recurso no encontrado\n");
		fflush(stdout);
	}
	else{
		auto tex = img->CreateTexture(MAIN->m_pDXManager);
		MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRVBackGround2);
	}
	img = CImageBMP::CreateBitmapFromFile("..\\Assets\\A_Plus_B.bmp", NULL);
	if (!img) {
		printf("Recurso no encontrado\n");
		fflush(stdout);
	}
	else {
		auto tex = img->CreateTexture(MAIN->m_pDXManager);
		MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRVButtonAB);
	}
	img = CImageBMP::CreateBitmapFromFile("..\\Assets\\A_Plus_B_Pressed.bmp", NULL);
	if (!img) {
		printf("Recurso no encontrado\n");
		fflush(stdout);
	}
	else {
		auto tex = img->CreateTexture(MAIN->m_pDXManager);
		MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRVButtonABPressed);
	}
	img = CImageBMP::CreateBitmapFromFile("..\\Assets\\B_Plus_A.bmp", NULL);
	if (!img) {
		printf("Recurso no encontrado\n");
		fflush(stdout);
	}
	else {
		auto tex = img->CreateTexture(MAIN->m_pDXManager);
		MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRVButtonBA);
	}
	img = CImageBMP::CreateBitmapFromFile("..\\Assets\\B_Plus_A_Pressed.bmp", NULL);
	if (!img) {
		printf("Recurso no encontrado\n");
		fflush(stdout);
	}
	else {
		auto tex = img->CreateTexture(MAIN->m_pDXManager);
		MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRVButtonBAPressed);
	}

	img = CImageBMP::CreateBitmapFromFile("..\\Assets\\02toad.bmp", NULL);
	if (!img) {
		printf("Recurso no encontrado\n");
		fflush(stdout);
	}
	else {
		auto tex = img->CreateTexture(MAIN->m_pDXManager);
		MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRVTexture);
	}
	// Se limpia la mezcladora de sonido ya que vino de otro estado
	// Tambien se carga el nuevo recurso de sonido y se empieza su
	// reproduccion
	MAIN->m_pSndManager->ClearEngine();
	m_pSndBackground = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\OnGameSound.wav", SND_GAME);
	if (m_pSndBackground)
	{
		m_pSndBackground->Play(true);
	}
	// Se carga el recurso de presionado de boton
	MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\Blow.wav", 1);
	m_pDXPainter->m_Params.Brightness = { 0,0,0,0 };
	//m_pDXPainter->m_Params.Material.Ambient = { 255,255,255,0 };
	SetTimer(MAIN->m_hWnd, 1, 1000, NULL);
	SetTimer(MAIN->m_hWnd, 2, 4500, NULL);
}

unsigned long CSGame::OnEvent(CEventBase * pEvent)
{
	// Evento de JoyStick
	if (ACTION_EVENT == pEvent->m_ulEventType)
	{
		CActionEvent *Action = (CActionEvent*)pEvent;
		if (JOY_BUTTON_A_PRESSED == Action->m_nAction)
		{
			players[Action->m_nSource].buttons.comChar[players[Action->m_nSource].buttons.counter] = 'a';
			players[Action->m_nSource].buttons.counter++;
		}
		if (JOY_BUTTON_B_PRESSED == Action->m_nAction)
		{
			players[Action->m_nSource].buttons.comChar[players[Action->m_nSource].buttons.counter] = 'b';
			players[Action->m_nSource].buttons.counter++;
		}
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
			// Crear Render Target Auxiliar
			ID3D11Texture2D* pBackBuffer = 0;

			// Lo corecto es preguntar a la cadena de intercambio (backbuffer)
			// cual es el tamaño correcto
			m_pDXManager->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);

			D3D11_TEXTURE2D_DESC dtd;
			// En estos momentos ya tengo la resolucion y ipo exactos
			pBackBuffer->GetDesc(&dtd);
			dtd.BindFlags |= (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

			g_iWidth = dtd.Width;
			g_iHeight = dtd.Height;
			// Ahora creamos la textura 
			SAFE_RELEASE(pBackBuffer);
			
			
			// Se adapta al tamaño de la pantalla
			MATRIX4D AC = Scaling((float)g_iHeight / g_iWidth, 1, 1);

			
			// Se crean los indices y el cuadro para presentar la imagen precargada
			// del fondo del juego.
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
			// Para el fondo necesitamos dibujar en 2D por esta
			// razon necesitamos igualar las matrices de vista,
			// mundo y proyeccion a la identidad.
			m_pDXPainter->m_Params.View = m_pDXPainter->m_Params.World = m_pDXPainter->m_Params.Projection = Identity(); 
			m_pDXPainter->m_Params.Flags1 = MAPPING_EMISSIVE;
			if (changeBackground)
			{
				m_pDXManager->GetContext()->PSSetShaderResources(4, 1, &m_pSRVBackGround);
			}
			else
			{
				m_pDXManager->GetContext()->PSSetShaderResources(4, 1, &m_pSRVBackGround2);
			}
			m_pDXPainter->DrawIndexed(Frame, 4, FrameIndex, 6, 0);

			CDXBasicPainter::VERTEX FrameButton[4]
			{
				{ { -0.25,0.4,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } }, // Primer vertice 
				{ { 0.25,0.4,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
				{ { -0.25,0,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
				{ { 0.25,0,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
			};
			if (AB)
			{
				if (freqButBackground)
					m_pDXManager->GetContext()->PSSetShaderResources(4, 1, &m_pSRVButtonAB);
				else
					m_pDXManager->GetContext()->PSSetShaderResources(4, 1, &m_pSRVButtonABPressed);
			}
			else
			{
				if (freqButBackground)
					m_pDXManager->GetContext()->PSSetShaderResources(4, 1, &m_pSRVButtonBA);
				else
					m_pDXManager->GetContext()->PSSetShaderResources(4, 1, &m_pSRVButtonBAPressed);
			}
			
			m_pDXPainter->DrawIndexed(FrameButton, 4, FrameIndex, 6, 0);


			// Ya se cambia a la matriz de vista y de proyeccion para
			// el juego en 3D
			m_pDXPainter->m_Params.View = g_View;
			m_pDXPainter->m_Params.Projection = g_Projection*AC;			
			m_pDXPainter->m_Params.Flags1 = MAPPING_DIFFUSE;//MAPPING_NORMAL_TRUE | MAPPING_DIFFUSE | MAPPING_EMISSIVE;
			VECTOR4D Gray = { .5,.5,.5,0 };
			//m_pDXPainter->m_Params.Material.Diffuse = Gray;
			//m_pDXPainter->m_Params.Material.Ambient = Gray;
			
			CDXBasicPainter::PARAMS old;
			old = m_pDXPainter->m_Params;
			//m_pDXPainter->m_Params.Flags1 = MAPPING_DIFFUSE;
			m_pDXManager->GetContext()->PSSetShaderResources(0, 1, &m_pSRVTexture);
			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				m_pDXPainter->m_Params.Brightness = players[i].brightness;
				m_pDXPainter->m_Params.World = players[i].world;
		
				for (int j = 0; j < players[i].g_Surface.m_Vertices.size(); j++)
				{
					players[i].g_Surface.m_Vertices[j].Color = players[i].color;
				}
				
				m_pDXPainter->DrawIndexed(&players[i].g_Surface.m_Vertices[0], players[i].g_Surface.m_Vertices.size(), &players[i].g_Surface.m_Indices[0], players[i].g_Surface.m_Indices.size(), PAINTER_DRAW);
			}

			//m_pDXPainter->m_Params = old;
			printCounter(players[0].counterClicks, players[0].counterPosition);
			printCounter(players[1].counterClicks, players[1].counterPosition);
			printCounter(players[2].counterClicks, players[2].counterPosition);
			printCounter(players[3].counterClicks, players[3].counterPosition);
			m_pDXPainter->m_Params = old;
			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				if (players[i].buttons.comChar[0] == correctCombination[0] &&
					players[i].buttons.comChar[1] == correctCombination[1] &&
					players[i].buttons.comChar[2] == correctCombination[2])//(players[i].combination[0] && players[i].combination[1])
				{
					players[i].counterClicks++;
					MAIN->m_pSndManager->PlayFx(1);

					players[i].brightness.x += .015;
					players[i].brightness.y += .015;
					players[i].brightness.z += .015;


					players[i].world = Scaling(1.02, 1.02, 1.02)*players[i].world;
					m_pDXPainter->m_Params.World = players[i].world;
					m_pDXPainter->m_Params.Brightness = players[i].brightness;
					m_pDXPainter->DrawIndexed(&players[i].g_Surface.m_Vertices[0], players[i].g_Surface.m_Vertices.size(), &players[i].g_Surface.m_Indices[0], players[i].g_Surface.m_Indices.size(), PAINTER_DRAW);

					for (int k = 0; k < 3; k++)
						players[i].buttons.comChar[k] = ' ';
					players[i].buttons.counter = 0;
					changeBackground == true ? changeBackground = false : changeBackground = true;
				}
				else if (players[i].buttons.counter == 2)
				{
					for (int k = 0; k < 3; k++)
						players[i].buttons.comChar[k] = ' ';
					players[i].buttons.counter = 0;
				}
			}
			


			m_pDXPainter->m_Params = old;
			m_pDXManager->GetSwapChain()->Present(1, 0);
		}

		// Verificamos si hay algun ganador
		if (players[0].counterClicks == SCORE || players[1].counterClicks == SCORE ||
			players[2].counterClicks == SCORE || players[3].counterClicks == SCORE)
		{
			printf("Ya ganaste!!!");
			players[0].counterClicks == SCORE ? MAIN->winner = 0 :
				players[1].counterClicks == SCORE ? MAIN->winner = 1 :
				players[2].counterClicks == SCORE ? MAIN->winner = 2 :
				players[3].counterClicks == SCORE ? MAIN->winner = 3 : MAIN->winner = 1000;

			MAIN->win = true;
			m_pSMOwner->Transition(CLSID_CSGameOver);
			return 0; // nunca se nos olvide retornar 0 para que no existe access violation
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

			}
			break;
		case WM_TIMER:
		{
			if (1 == pWin32->m_wParam)
			{
				freqButBackground == true ? freqButBackground = false : freqButBackground = true;
			}
			if (2 == pWin32->m_wParam)
			{
				AB == true ? AB = false : AB = true;
				if (AB)
				{
					correctCombination[0] = 'a';
					correctCombination[1] = 'b';
					correctCombination[2] = ' ';
				}
				else
				{
					correctCombination[0] = 'b';
					correctCombination[1] = 'a';
					correctCombination[2] = ' ';
				}
			}

		}
			break;
		case WM_CLOSE:
			m_pSMOwner->Transition(CLSID_CStateNull);
			return 0;
		}
	}
	return __super::OnEvent(pEvent);
}

void CSGame::LoadScene(char * filename, const unsigned int p)
{
	/* the global Assimp scene object */
	const struct aiScene* scene = aiImportFile(filename, aiProcessPreset_TargetRealtime_Fast); //  aiProcessPreset_TargetRealtime_MaxQuality
	
	int aux = scene->mNumMeshes;
	for (unsigned long i = 0; i < scene->mNumMeshes; i++)
	{
		players[p].g_Surface.m_Vertices.resize(scene->mMeshes[i]->mNumVertices);
		for (unsigned long j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
		{
			players[p].g_Surface.m_Vertices[j].Position = {
				scene->mMeshes[i]->mVertices[j].x,
				scene->mMeshes[i]->mVertices[j].y,
				scene->mMeshes[i]->mVertices[j].z,
				1 };
		}
		MATRIX4D t;
		t.m00 = scene->mRootNode->mChildren[i]->mTransformation.a1;
		t.m01 = scene->mRootNode->mChildren[i]->mTransformation.a2;
		t.m02 = scene->mRootNode->mChildren[i]->mTransformation.a3;
		t.m03 = scene->mRootNode->mChildren[i]->mTransformation.a4;
		t.m10 = scene->mRootNode->mChildren[i]->mTransformation.b1;
		t.m11 = scene->mRootNode->mChildren[i]->mTransformation.b2;
		t.m12 = scene->mRootNode->mChildren[i]->mTransformation.b3;
		t.m13 = scene->mRootNode->mChildren[i]->mTransformation.b4;
		t.m20 = scene->mRootNode->mChildren[i]->mTransformation.c1;
		t.m21 = scene->mRootNode->mChildren[i]->mTransformation.c2;
		t.m22 = scene->mRootNode->mChildren[i]->mTransformation.c3;
		t.m23 = scene->mRootNode->mChildren[i]->mTransformation.c4;
		t.m30 = scene->mRootNode->mChildren[i]->mTransformation.d1;
		t.m31 = scene->mRootNode->mChildren[i]->mTransformation.d2;
		t.m32 = scene->mRootNode->mChildren[i]->mTransformation.d3;
		t.m33 = scene->mRootNode->mChildren[i]->mTransformation.d4;

		players[p].world = Transpose(t);

		players[p].g_Surface.m_Indices.resize(scene->mMeshes[i]->mNumFaces * scene->mMeshes[i]->mFaces[0].mNumIndices);
		for (unsigned long j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
		{
			for (unsigned long k = 0; k < scene->mMeshes[i]->mFaces[j].mNumIndices; k++)
			{
				players[p].g_Surface.m_Indices[j*scene->mMeshes[i]->mFaces[j].mNumIndices + k] = scene->mMeshes[i]->mFaces[j].mIndices[k];
			}
		}

		for (unsigned long j = 0; j < players[p].g_Surface.m_Vertices.size(); j++)
		{
			VECTOR4D TexCoord = { 0,0,0,0 };
			TexCoord.x = players[p].g_Surface.m_Vertices[j].Position.x;
			TexCoord.y = players[p].g_Surface.m_Vertices[j].Position.z;
			TexCoord.z = players[p].g_Surface.m_Vertices[j].Position.y;
			TexCoord = Normalize(TexCoord);
			TexCoord.x = TexCoord.x * 0.5 + 0.5;
			TexCoord.y = TexCoord.y * 0.5 + 0.5;

			players[p].g_Surface.m_Vertices[j].TexCoord = TexCoord;
		}

		players[p].g_Surface.BuildTangentSpaceFromTexCoordsIndexed(true);

	}
}

void CSGame::printCounter(const unsigned int nCounter, MATRIX4D& ST)
{
	std::string s = std::to_string(nCounter);
	const char *ss = s.c_str();
	MAIN->m_pTextRender->RenderText(ST, ss);

}

void CSGame::initializeCounterPositions()
{
	// Se establece los contadores iniciales en 0
	players[0].counterClicks = 0;
	players[1].counterClicks = 0;
	players[2].counterClicks = 0;
	players[3].counterClicks = 0;

	// Se establece la posicion de los contadores de cada jugador
	players[0].counterPosition = Translation(0.5, -0.5, 0)* // Centro de caracter
		Scaling(0.05, 0.06, 1)*    // Tamaño del caracter
		Translation(-1, 1, 0);     // Posicion del texto

	players[1].counterPosition = Translation(0.5, -0.5, 0)* // Centro de caracter
		Scaling(0.05, 0.06, 1)*			// Tamaño del caracter
		Translation(0.1, 1, 0);			//Posicion del texto


	players[2].counterPosition = Translation(0.5, -0.5, 0)* // Centro de caracter
		Scaling(0.05, 0.06, 1)*			// Tamaño del caracter
		Translation(-1, -0.1, 0);		// Posicion del texto

	players[3].counterPosition = Translation(0.5, -0.5, 0)* // Centro de caracter
		Scaling(0.05, 0.06, 1)*			// Tamaño del caracter
		Translation(0.1, -0.1, 0);      //Posicion del texto
}

void CSGame::initializePositions()
{
	players[0].position.x = 380;
	players[0].position.y = 50;
	players[0].position.z = 0;
	players[0].color = { 1,0,0,0 };

	players[1].position.x = -420;
	players[1].position.y = 70;
	players[1].position.z = 0;
	players[1].color = { 1,0,1,0 };

	players[2].position.x = 380;
	players[2].position.y = -270;
	players[2].position.z = -140;
	players[2].color = { 0,0,1,0 };

	players[3].position.x = -400;
	players[3].position.y = -270;
	players[3].position.z = -140;
	players[3].color = { 0,1,1,0 };
}

void CSGame::OnExit(void)
{
	SAFE_RELEASE(pSRVNormalMap);
	SAFE_RELEASE(pSRVEnvMap);
	SAFE_RELEASE(pSRVNormalMapTrue);
	SAFE_RELEASE(pSRVEmissiveMap);
}
