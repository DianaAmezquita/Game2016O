#pragma once
#include "sndobject.h"
#include <Windows.h>
#include <map>         //"dsound.lib"
using namespace std;

struct IDirectSound8;
struct IDirectSoundBuffer;

class CSndFx;
class CSndControl;

class CSndManager :
	public CSndObject
{
	friend class CSndFactory;
private:
	typedef map<unsigned long,CSndControl*> CSndPool;
	//Albercas de sonido
	CSndPool* m_pSndFxPool;
	CSndPool* m_pSndStreamPool;
	unsigned long m_ulMaxFxPlaying;
	//Lista de reproducción actual
	CSndPool* m_pSndFxPlayingPool;
	CSndPool* m_pSndStreamPlayingPool;
	unsigned long m_ulPlaybackFxIDGen;
	unsigned long m_ulPlaybackStreamIDGen;
	//Construcción Destrucción
	CSndManager(CSndFactory* m_pFactory);
	virtual ~CSndManager(void);
public:
	HWND m_hWnd;
	IDirectSound8* m_pIDirectSound;
	IDirectSoundBuffer* m_pIPrimaryBuffer;
	CSndFactory* m_pFactory;
	virtual bool InitSoundEngine(HWND hWnd);
	virtual void UnitializeSoundEngine(void);
	virtual CSndFx* LoadSoundFx(TCHAR* pszFileName,unsigned long ulIDSndFx);
	virtual void RemoveSoundFx(unsigned long ulIDSndFx);
	virtual unsigned long PlayFx(unsigned long ulIDSndFx,float fVolume=1.0f,float fBalance=0.0f,float fSpeed=1.0f,float fPosition=0.0f); //Retona Key de reproducción actual
	virtual CSndControl* GetPlayingFx(unsigned long ulKeySnd);
	virtual bool IsPlayingFx(unsigned long ulKeySnd);
	virtual bool StopFx(unsigned ulKeySnd);
	virtual void RemoveAllSndFxStopped(void);
	virtual void SetMaximumFxPlaying(unsigned long ulMaxFxPlaying);
	virtual void StopAll(void);
	virtual void ClearEngine(void);
};
