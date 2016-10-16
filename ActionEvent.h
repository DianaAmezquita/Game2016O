#pragma once
#include "HSM\EventBase.h"
// Definir las acciones de nuestro juego
#define ACTION_EVENT 0x26436a29
#define JOY_BUTTON_A_PRESSED	1
#define JOY_BUTTON_A_RELEASED	2
#define JOY_BUTTON_B_PRESSED	3
#define JOY_BUTTON_B_RELEASED	4
#define JOY_BUTTON_X_PRESSED	5
#define JOY_BUTTON_X_RELEASED	6
#define JOY_BUTTON_Y_PRESSED	7
#define JOY_BUTTON_Y_RELEASED	8
#define JOY_AXIS_LX				9
#define JOY_AXIS_LY				10

class CActionEvent :
	public CEventBase
{
public:
	int m_nSource;
	unsigned long m_ulTime;
	int m_nAction;
	float m_fAxis; // Valor normalizado de -1 a 1 Punto central 0
	CActionEvent(int nSource, unsigned long ulTime, int nAction);
	virtual ~CActionEvent();
};

