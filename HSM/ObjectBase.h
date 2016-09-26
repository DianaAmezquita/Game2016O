#pragma once
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(X) if((X)) { (X)->Release(); (X)=0;}
#endif
#define SAFE_DELETE(X)        if((X)) {delete (X); (X)=0;}
#define SAFE_DELETE_ARRAY(X)  if((X)) {delete [] (X); (X)=0;}
class CObjectBase
{
private:
	//Object's ID
	unsigned long m_ulObjectID;
	//If count reach zero, self delete must be performed
	// Esta tecnica se llama reference counting
	unsigned long m_ulRefCount;
	//Global instance counter
	static unsigned long s_ulInstanceCount;
public:
	CObjectBase(void);
	virtual ~CObjectBase(void);
	//Cada clase derivada deberá forzosamente implementar identificación de clase y nombre de clase
	//El identificador de clase deberá ser único en el contexto de la aplicación
	virtual unsigned long GetClassID()=0;
	virtual const char* GetClassString()=0;
public:
	//Control de ciclo de vida y control de referencias.
	virtual unsigned long AddRef(void);
	//Si el contador de referencias llega a cero, se elimina automáticamente el objeto. NO USAR operator delete, en vez de eso, invocar Release
	virtual unsigned long Release(void);
	//Identificador de objeto o instancia  (No confundir con identificador de clase)
	virtual unsigned long& ID();
	//Estadística de objetos de clases derivadas de CObjectBase instanciados hasta el momento.
	static unsigned long  GetInstanceCount();
};


/* Reglas de manejo de referencias hacia objetos derivados de CObjectBase */

/*
	1.- Un objeto recién creado tiene su contador de referencias en 1
	2.- Si una función retorna o instala una referencia en un parámetro de salida, la función deberá invocar a AddRef antes de salir.
	3.- Si una función u objeto colecta una referencia proveniente de otra función deberá invocar al método Release para liberar la referencia cuando ya no la necesite
	4.- Si una función colecta una referencia y la almacena no es necesario invocar al método Release, tampoco al método AddRef
	5.- Si una función recibe como parámetro a una referencia y la almacena, deberá invocar al método AddRef
*/

