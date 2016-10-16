#include "StdAfx.h"
#include "SndFx.h"
#include <windows.h>
#include <mmreg.h>
#include <dsound.h>
CSndFx::CSndFx(CSndManager* pManager):CSndControl(pManager)
{
}
CSndFx::~CSndFx(void)
{
	
}
float CSndFx::GetPlayPosition()
{
	DSBCAPS dsc;
	dsc.dwSize=sizeof(DSBCAPS);
	
	DWORD dwPlay;
	m_pIDirectSoundBuffer->GetCurrentPosition(&dwPlay,NULL);
	m_pIDirectSoundBuffer->GetCaps(&dsc);
	return (float)dwPlay/dsc.dwBufferBytes;
}
void CSndFx::SetPlayPosition(float fPosition)
{
	WAVEFORMATEX wfx;
	DSBCAPS dsc;
	dsc.dwSize=sizeof(DSBCAPS);
	m_pIDirectSoundBuffer->GetCaps(&dsc);
	m_pIDirectSoundBuffer->GetFormat(&wfx,sizeof(WAVEFORMATEX),NULL);
	m_pIDirectSoundBuffer->SetCurrentPosition((DWORD)((dsc.dwBufferBytes-wfx.nBlockAlign)*fPosition));
}
float CSndFx::GetPlayTime(void)
{
	WAVEFORMATEX wfx;
	DSBCAPS dsc;
	dsc.dwSize=sizeof(DSBCAPS);
	m_pIDirectSoundBuffer->GetCaps(&dsc);
	m_pIDirectSoundBuffer->GetFormat(&wfx,sizeof(WAVEFORMATEX),NULL);
	return (float)dsc.dwBufferBytes/wfx.nAvgBytesPerSec;	
}