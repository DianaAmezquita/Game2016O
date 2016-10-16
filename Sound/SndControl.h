#pragma once
#include "sndobject.h"

struct IDirectSoundBuffer8;

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) {if((x)){ (x)->Release();(x)=NULL;}}
#endif

class CSndControl :
	public CSndObject
{
	friend class CSndManager;
protected:
	CSndControl(CSndManager* pManagerOwner);
	virtual ~CSndControl(void);
	CSndManager* m_pManagerOwner;
	IDirectSoundBuffer8* m_pIDirectSoundBuffer;
public:
	virtual CSndControl* DuplicateSound(void);
	virtual void SetVolume(float fVolume);
	virtual void SetPan(float fBalance); // Balance -1 es a la izquierda 0 es en el medio y +1 en la bocina derecha
	virtual void SetSpeed(float fSpeed); //cambiar el tiempo real, la velocidad de reproduccion (velocidad con la que se reproduce el buffer)
	virtual void Play(bool bLooping);
	virtual void Stop(void);
	virtual bool IsPlaying(void);
	//Obtiene la posición normalizada actual del cursor de reproducción 0:Inicio 1:Final
	virtual float GetPlayPosition(void)=0;
	//Establece la posición normalizada del cursor de reproducción 0: Inicio 1:Final
	virtual void SetPlayPosition(float fPosition)=0;
	//Obtiene la duración total en segundos del recurso de sonido. 
	virtual float GetPlayTime()=0;
};
