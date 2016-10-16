#include "stdafx.h"
#include "InputProcessor.h"
#include "ActionEvent.h"



CInputProcessor::CInputProcessor(CStateMachineManager * pHSMOwner)
{
	m_pHSMOwner = pHSMOwner;
}

unsigned long CInputProcessor::OnEvent(CEventBase * pEvent)
{
	if (INPUT_EVENT == pEvent->m_ulEventType)
	{
		// Paso1: Construir copia dle estado que ingresa
		CInputEvent* pInput = (CInputEvent*)pEvent;
		CInputEvent* pCopy = new CInputEvent(pInput->m_nSource, pInput->m_ulTime, pInput->m_js2);
		auto GamePad = m_mapLastEvent.find(pInput->m_nSource); // Voy a buscarlo en el mapa de los ultimos eventos porque pudiera no existir 
		

		// Paso2: Procesar diferencias
		if (GamePad != m_mapLastEvent.end())
		{
			auto Last = GamePad->second->m_js2;
			auto Current = pInput->m_js2;
			if (!Last.rgbButtons[0] && Current.rgbButtons[0]) // Si el anterior era 0 y el actual 1
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_BUTTON_A_PRESSED);
				m_pHSMOwner->PostEvent(Action); // Programa ese nuevo evento para el futuro
			}
			if (Last.rgbButtons[0] && !Current.rgbButtons[0]) // Si el anterior era 0 y el actual 1
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_BUTTON_A_RELEASED);
				m_pHSMOwner->PostEvent(Action); // Programa ese nuevo evento para el futuro
			}
			if (!Last.rgbButtons[1] && Current.rgbButtons[1]) // Si el anterior era 0 y el actual 1
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_BUTTON_B_PRESSED);
				m_pHSMOwner->PostEvent(Action); // Programa ese nuevo evento para el futuro
			}
			if (Last.rgbButtons[1] && !Current.rgbButtons[1]) // Si el anterior era 0 y el actual 1
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_BUTTON_B_RELEASED);
				m_pHSMOwner->PostEvent(Action); // Programa ese nuevo evento para el futuro
			}
			if (!Last.rgbButtons[2] && Current.rgbButtons[2]) // Si el anterior era 0 y el actual 1
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_BUTTON_X_PRESSED);
				m_pHSMOwner->PostEvent(Action); // Programa ese nuevo evento para el futuro
			}
			if (Last.rgbButtons[2] && !Current.rgbButtons[2]) // Si el anterior era 0 y el actual 1
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_BUTTON_X_RELEASED);
				m_pHSMOwner->PostEvent(Action); // Programa ese nuevo evento para el futuro
			}
			
			// Reportar el estado de los ejes
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_AXIS_LX);
				Action->m_fAxis = (Current.lX - 0x7fff)/32768.0f; // lX viene codificado en complemento a 2. Me queod a la mitad para quedar polarizado
				m_pHSMOwner->PostEvent(Action);
				Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_AXIS_LY);
				Action->m_fAxis = -(Current.lY - 0x7fff) / 32768.0f; // lX viene codificado en complemento a 2. Me queod a la mitad para quedar polarizado
				m_pHSMOwner->PostEvent(Action);
			}
		}
		
		// Paso3: Almacenar estado anterior, que es el actual
		if (GamePad == m_mapLastEvent.end())
		{
			m_mapLastEvent.insert(make_pair(pInput->m_nSource, pCopy));
		} 
		else // ya lo encontramos, si lo encuentras
		{
			delete GamePad->second;
			GamePad->second = pCopy;
		}
	}
	return 0;
}


CInputProcessor::~CInputProcessor()
{
}
