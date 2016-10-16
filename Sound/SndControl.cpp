#include "StdAfx.h"
#include <windows.h>
#include <mmreg.h>
#include <dsound.h>
#include "SndControl.h"
#include "SndFactory.h"
#include "SndManager.h"
CSndControl::CSndControl(CSndManager* pManagerOwner)
{
	m_pManagerOwner=pManagerOwner;
}

CSndControl::~CSndControl(void)
{
	SAFE_RELEASE(m_pIDirectSoundBuffer);
}

CSndControl* CSndControl::DuplicateSound(void)
{
	LONG lVolume;
	IDirectSoundBuffer* pIDirectSoundBuffer=NULL;
	CSndControl* pSnd=(CSndControl*)m_pManagerOwner->m_pFactory->CreateObject(TEXT("CSndFx"));
	m_pIDirectSoundBuffer->GetVolume(&lVolume);
	m_pManagerOwner->m_pIDirectSound->DuplicateSoundBuffer(
		m_pIDirectSoundBuffer,&pIDirectSoundBuffer);

	pIDirectSoundBuffer->QueryInterface(IID_IDirectSoundBuffer8,(void**)&pSnd->m_pIDirectSoundBuffer);
	pIDirectSoundBuffer->Release();
	
	//Microsoft Issue.... y está documentado... Workaraund
	pSnd->m_pIDirectSoundBuffer->SetVolume(lVolume-1);
	pSnd->m_pIDirectSoundBuffer->SetVolume(lVolume);
	return pSnd;
}

void CSndControl::Play(bool bLooping)
{
	m_pIDirectSoundBuffer->Play(0,0,(bLooping?DSBPLAY_LOOPING:0));
}
void CSndControl::Stop(void)
{
	m_pIDirectSoundBuffer->Stop();
}

bool CSndControl::IsPlaying()
{
	DWORD dwStatus;
	HRESULT hr;
	hr=m_pIDirectSoundBuffer->GetStatus(&dwStatus);
	if(FAILED(hr))
		return false;
	return 0!=(dwStatus&(DSBSTATUS_PLAYING|DSBSTATUS_LOOPING));
}

void CSndControl::SetPan(float fBalance)
{
	m_pIDirectSoundBuffer->SetPan(DSBPAN_CENTER+(long)(fBalance*(DSBPAN_RIGHT)));
}
void CSndControl::SetVolume(float fVolume)
{
	m_pIDirectSoundBuffer->SetVolume((long)((1.0f-fVolume)*DSBVOLUME_MIN));
}
void CSndControl::SetSpeed(float fSpeed)
{
	WAVEFORMATEX wfx;
	m_pIDirectSoundBuffer->GetFormat(&wfx,sizeof(WAVEFORMATEX),NULL);
	m_pIDirectSoundBuffer->SetFrequency((DWORD)(wfx.nSamplesPerSec*fSpeed));
}