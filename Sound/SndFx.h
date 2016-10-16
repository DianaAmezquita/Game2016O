#pragma once
#include "SndControl.h"

class CSndFx :
	public CSndControl
{
	friend class CSndFactory;
	friend class CSndManager;
protected:
	CSndFx(CSndManager* pManagerOwner);
	virtual ~CSndFx(void);
	//Obtiene la posición normalizada actual del cursor de reproducción 0:Inicio 1:Final
	virtual float GetPlayPosition(void);
	//Establece la posición normalizada del cursor de reproducción 0: Inicio 1:Final
	virtual void SetPlayPosition(float fPosition);
	//Obtiene la duración total en segundos del recurso de sonido. 
	virtual float GetPlayTime();
};
