#pragma once

/*
	Las aplicaciones derivan de CEventBase para incorporar más información en el evento según las necesidades.

	NO SE DEBEN AGREGAR MAS DATOS MIEMBRO EN ESTA CLASE BASE. Deberá derivar para incorporar más parámetros en el evento.

	Ejemplos: CEventWin32, CEventGame, CEventCustom etc...
	Cada uno de estos tipos de evento deberán tener datos muy específicos.
	Una forma de resolver en especifico de qué clase particular de evento se trata es a través de m_ulEventType, 
	mediante una enumeración definida por el usuario. La enumeración contiene tipos (números) únicos en el contexto de la aplicación.
*/
#define APP_LOOP 0x0
class CEventBase
{
public:
	//Tipo de evento definido por el usuario, ayuda a resolver el tipo de evento.
	unsigned long m_ulEventType;
	CEventBase(void);
	virtual ~CEventBase(void);
};

