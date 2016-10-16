#include "StdAfx.h"
#include "SndManager.h"
#include "SndFactory.h"
#include "SndFx.h"
#include <windows.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <dsound.h>
#include <map>
using namespace std;
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(X) if((X)) { (X)->Release(); (X)=0;}
#endif
#define SAFE_DELETE(X)        if((X)) {delete (X); (X)=0;}
#define SAFE_DELETE_ARRAY(X)  if((X)) {delete [] (X); (X)=0;}
#pragma pack(1)
struct WAVEFILE
{
	//RIFF
	DWORD chunkRIFF; //"RIFF" Resource Interchange File Format				0
	DWORD lonR; //Longitud del segmento de la información de recurso	4
	//WAVE
	DWORD chunkWAVE; //"WAVE" Cadena de identificación de recurso.			8
	//FMT
	DWORD chunkfmt; //"fmt " Cadena de indentificación de formato			12
	DWORD lonF; //longitud de los datos del segmento de formato.		16
	//FORMAT DATA
	WORD wFormatTag; //Etiqueta del formato							20
	WORD nChannels; //Canales de salida 1 Mono 2 Estereo				22	
	DWORD nSamplesPerSec; //Numero de muestras por segundo					24
	DWORD nAvgBytesPerSec; //Bytes por segundo							28
	WORD  nBlockAlign; //Alineamiento de bloque de datos
	WORD  wBitsPerSample; //Específico del formato
	//DATA
	DWORD chunkdata; //Cadena de datos
	DWORD lonD;// longitud de los datos
};
#pragma pack()



CSndManager::CSndManager(CSndFactory *pFactory)
{
	m_pFactory=pFactory;
	m_pIDirectSound=NULL;
	m_pIPrimaryBuffer=NULL;
	m_pSndFxPool=new CSndPool;
	m_pSndStreamPool=new CSndPool;
	m_pSndFxPlayingPool=new CSndPool;
	m_pSndStreamPlayingPool=new CSndPool;
	m_hWnd=NULL;
	m_ulMaxFxPlaying=4;
	m_ulPlaybackFxIDGen=0;
	m_ulPlaybackStreamIDGen=0;
}

CSndManager::~CSndManager(void)
{
	//ClearEngine();
	SAFE_DELETE(m_pSndFxPool);
	SAFE_DELETE(m_pSndStreamPool);
	SAFE_DELETE(m_pSndFxPlayingPool);
	SAFE_DELETE(m_pSndStreamPlayingPool);
}
bool CSndManager::InitSoundEngine(HWND hWnd)
{
	m_hWnd=hWnd;
	HRESULT hr=S_OK;
	hr=DirectSoundCreate8(NULL,&m_pIDirectSound,NULL);
	if(FAILED(hr))
		return false;
	m_pIDirectSound->SetCooperativeLevel(m_hWnd,DSSCL_PRIORITY);
	//Preparar la descripción del buffer a crear
	DSBUFFERDESC dsbd;
	memset(&dsbd,0,sizeof(DSBUFFERDESC));
	dsbd.dwSize=sizeof(dsbd);
	dsbd.dwFlags=DSBCAPS_PRIMARYBUFFER; //Invocar al buffer primario
	dsbd.dwBufferBytes=0; //Se trata de un buffer primario
	dsbd.dwReserved=0;
	dsbd.lpwfxFormat=NULL;
	//Extraer la referencia al buffer primario de DirectSound
	hr=m_pIDirectSound->CreateSoundBuffer(&dsbd,&m_pIPrimaryBuffer,NULL);
	if(hr!=DS_OK)
	{
		return true;
	}
	//Configurar el formato de onda
	WAVEFORMATEX wfx;
	memset(&wfx,0,sizeof(WAVEFORMATEX));
	wfx.cbSize=0; //Debe ser 0
	wfx.nBlockAlign=4; //4 bytes por bloque de muestra (2 bytes por canal)
	wfx.nChannels=2; //Sonido estéreo
	wfx.nSamplesPerSec=44100; //Frecuencia de muestreo calidad CD
	wfx.wBitsPerSample=16;
	wfx.wFormatTag=WAVE_FORMAT_PCM;
	wfx.nAvgBytesPerSec=wfx.nBlockAlign*wfx.nSamplesPerSec; //Bytes reproducidos por segundo
	//Establecer el formato del buffer primario
	hr=m_pIPrimaryBuffer->SetFormat(&wfx);
	hr=m_pIPrimaryBuffer->Play(0,0,DSBPLAY_LOOPING);
	return true;
}

void CSndManager::UnitializeSoundEngine()
{
	ClearEngine();
	if(m_pIPrimaryBuffer)
	{
		m_pIPrimaryBuffer->Stop();
	}
	SAFE_DELETE(m_pSndFxPool);
	SAFE_DELETE(m_pSndStreamPool);
	SAFE_DELETE(m_pSndFxPlayingPool);
	SAFE_DELETE(m_pSndStreamPlayingPool);
	SAFE_RELEASE(m_pIPrimaryBuffer);
	SAFE_RELEASE(m_pIDirectSound);
	SAFE_DELETE(m_pFactory);
	
}

CSndFx* CSndManager::LoadSoundFx(TCHAR *pszFileName, unsigned long ulIDSndFx)
{
	HRESULT hr=E_FAIL;
	CSndPool::iterator it=m_pSndFxPool->find(ulIDSndFx);
	if(it!=m_pSndFxPool->end())
		return NULL;  //El sonido ya existe.
	DWORD dwDummy;
	HANDLE hFile;
	hFile=CreateFile(pszFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	//Read the file header of wave file
	WAVEFILE wf;
	long FileDataPointer=0;
	memset(&wf,0,sizeof(WAVEFILE));
	long lFileOffset=0;
	ReadFile(hFile,&wf,8,&dwDummy,NULL);
	if(wf.chunkRIFF!=MAKEFOURCC('R','I','F','F'))
	{
		CloseHandle(hFile);
		return NULL;
	}
	ReadFile(hFile,&wf.chunkWAVE,4,&dwDummy,NULL);
	if(wf.chunkWAVE!=MAKEFOURCC('W','A','V','E'))
	{
		CloseHandle(hFile);
		return NULL;
	}
	ReadFile(hFile,&wf.chunkfmt,8,&dwDummy,NULL);
	if(wf.chunkfmt!=MAKEFOURCC('f','m','t',' '))
	{
		CloseHandle(hFile);
		return NULL;
	}
	ReadFile(hFile,&wf.wFormatTag,24,&dwDummy,NULL);

	DWORD dwPosition=SetFilePointer(hFile,0,0,FILE_BEGIN);

	while(1)
	{
		if(INVALID_SET_FILE_POINTER!=SetFilePointer(hFile,dwPosition,NULL,FILE_BEGIN))
		{
			ReadFile(hFile,&wf.chunkdata,8,&dwDummy,NULL);
			if(wf.chunkdata!=MAKEFOURCC('d','a','t','a'))
			{
				dwPosition++;
				continue;
			}
			break;
		}
	}

	DSBUFFERDESC dsbd;
	WAVEFORMATEX wfx;
	wfx.cbSize=0;
	wfx.nAvgBytesPerSec=wf.nAvgBytesPerSec;
	wfx.nBlockAlign=wf.nBlockAlign;
	wfx.nChannels=wf.nChannels;
	wfx.nSamplesPerSec=wf.nSamplesPerSec;
	wfx.wBitsPerSample=wf.wBitsPerSample;
	wfx.wFormatTag=wf.wFormatTag;

	memset(&dsbd,0,sizeof(DSBUFFERDESC));
	dsbd.dwSize=sizeof(DSBUFFERDESC);
	dsbd.dwBufferBytes=max(wf.lonD,DSBSIZE_MIN);
	dsbd.dwFlags=DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_LOCDEFER  |DSBCAPS_GLOBALFOCUS ;
	dsbd.lpwfxFormat=&wfx;
	dsbd.dwBufferBytes=wf.lonD;
	dsbd.guid3DAlgorithm=DS3DALG_DEFAULT;

	IDirectSoundBuffer* pISoundBuffer=NULL;
	IDirectSoundBuffer8* pISoundBuffer8=NULL;
	hr=m_pIDirectSound->CreateSoundBuffer(&dsbd,&pISoundBuffer,NULL);
	if(FAILED(hr))
	{
		CloseHandle(hFile);
		return NULL;
	}
	pISoundBuffer->QueryInterface(IID_IDirectSoundBuffer8,(void**)&pISoundBuffer8);
	SAFE_RELEASE(pISoundBuffer);
	//Bloquear el sound buffer para cargar las muestras de audio.
	void *pSoundBlock0=NULL,*pSoundBlock1=NULL;
	DWORD Size0=0,Size1=0;
	pISoundBuffer8->Lock(0,dsbd.dwBufferBytes,&pSoundBlock0,&Size0,
		&pSoundBlock1,&Size1,DSBLOCK_ENTIREBUFFER);
	ReadFile(hFile, pSoundBlock0,dsbd.dwBufferBytes,&dwDummy,NULL);
	pISoundBuffer8->Unlock(pSoundBlock0,Size0,pSoundBlock1,Size1);
	CloseHandle(hFile);
	CSndFx* pNewSndFx=(CSndFx*)m_pFactory->CreateObject(TEXT("CSndFx"));
	pNewSndFx->m_pIDirectSoundBuffer=pISoundBuffer8;
	m_pSndFxPool->insert(make_pair(ulIDSndFx,pNewSndFx));
	return pNewSndFx;
}

void CSndManager::RemoveSoundFx(unsigned long ulIDSndFx)
{
	CSndPool::iterator it=m_pSndFxPool->find(ulIDSndFx);
	if(it!=m_pSndFxPool->end())
	{
		m_pFactory->DestroyObject(it->second);
		m_pSndFxPool->erase(it);
	}
}

void CSndManager::SetMaximumFxPlaying(unsigned long ulMaxFxPlaying)
{
	m_ulMaxFxPlaying=ulMaxFxPlaying;
}
// Intenta reproducir un sonido
unsigned long CSndManager::PlayFx(
			unsigned long ulIDSndFx,
			float fVolume,
			float fBalance,
			float fSpeed,float fPosition)//Retona Key de reproducción actual
{
	CSndPool::iterator itSoundFx=m_pSndFxPool->find(ulIDSndFx);
	if(itSoundFx==m_pSndFxPool->end())
		return -1;
	CSndControl* pSndDuplicatedSound=itSoundFx->second->DuplicateSound();
	m_pSndFxPlayingPool->insert(make_pair(m_ulPlaybackFxIDGen,pSndDuplicatedSound));
	pSndDuplicatedSound->SetVolume(fVolume);
	pSndDuplicatedSound->SetPan(fBalance);
	pSndDuplicatedSound->SetSpeed(fSpeed);
	pSndDuplicatedSound->SetPlayPosition(fPosition);
	pSndDuplicatedSound->Play(false);
	// Si esta llena la cola entonces destruyo al mas viejito (HAY QUE DETENERLO PRIIMERO)
	if(m_pSndFxPlayingPool->size()>m_ulMaxFxPlaying) // Si la alberca supera la maxima cantidad de sonidos posibles los destruye
	{
		CSndPool::iterator it=m_pSndFxPlayingPool->begin();
		it->second->Stop();
		m_pFactory->DestroyObject(it->second);
		m_pSndFxPlayingPool->erase(it);
	}
	return m_ulPlaybackFxIDGen++;
}
bool CSndManager::IsPlayingFx(unsigned long ulKeySnd)
{
	CSndPool::iterator itSoundFx=this->m_pSndFxPlayingPool->find(ulKeySnd);
	if(itSoundFx==m_pSndFxPlayingPool->end())
		return false;
	if(itSoundFx->second->IsPlaying())
		return true;
	return false;
}
CSndControl* CSndManager::GetPlayingFx(unsigned long ulKeySnd)
{
	CSndPool::iterator itSoundFx=this->m_pSndFxPlayingPool->find(ulKeySnd);
	if(itSoundFx==m_pSndFxPlayingPool->end())
		return NULL;
	return itSoundFx->second;
}
bool CSndManager::StopFx(unsigned int ulKeySnd)
{
	CSndPool::iterator itSoundFx=this->m_pSndFxPlayingPool->find(ulKeySnd);
	if(itSoundFx==m_pSndFxPlayingPool->end())
		return false;
	itSoundFx->second->Stop();
	return true;
}

void CSndManager::RemoveAllSndFxStopped(void)
{
	CSndPool::iterator itA,itB;
	for(itA=m_pSndFxPlayingPool->begin();itA!=m_pSndFxPlayingPool->end();)
	{
		if(!itA->second->IsPlaying())
		{
			itB=itA;
			itB++;
			m_pFactory->DestroyObject(itA->second);
			m_pSndFxPlayingPool->erase(itA);
			itA=itB;
		}
		else
		{
			itA++;
		}
	}
}
void CSndManager::StopAll()
{
	CSndPool::iterator itA,itB;
	while(!m_pSndFxPlayingPool->empty())
	{
		itA=m_pSndFxPlayingPool->begin();
		m_pFactory->DestroyObject(itA->second);	
		m_pSndFxPlayingPool->erase(itA);
	}
}

void CSndManager::ClearEngine(void)
{
	StopAll();
	CSndPool::iterator itA,itB;
	while(!m_pSndFxPool->empty())
	{
		itA=m_pSndFxPool->begin();
		m_pFactory->DestroyObject(itA->second);	
		m_pSndFxPool->erase(itA);
	}
}

