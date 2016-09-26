#pragma once
#include "StateBase.h"
#include "EventBase.h"
#include "ObjectStock.h"
#include <map>
#include <list>
using namespace std;

/*

  ADMINISTRADOR DE MÁQUINA DE ESTADOS FINITO JERÁRQUICA

  Objetivo: Dar soporte para la implementación de aplicaciones basadas en paradigma de estados.
  Ayudando principalmente en aplicaciones que hacen uso de grandes cantidades de memoria y número de recursos
  como lo son las aplicaciones interactivas. Es claro que no todos los recursos son necesarios en todos los estados de la aplicación.
  También es auxiliar en el diseño de las respuestas a eventos que la aplicación deberá dar en función del estado en qué se encuentre.

  Las aplicaciones, basan su diseño en el concepto de estado, en donde un estado inicia (CStateBase::OnEntry)
  procesa eventos (CStateBase::OnEvent) de manera periodica, y eventualmente termina (CStateBase::OnExit) para así transitar hacia otro estado.

  Los juegos de video son un ejemplo de aplicaciones cuyo diseño se facilita en base a éste paradigma. 

  Los desarrolladores distribuyen su esfuerzo al definir e implementar estados. La integración de los estados también resulta trivial en un autómata
  que conformará un todo. Los sub estados o estados comparten y transfieren información a través del contexto de sus superestados. Por lo que los desarrolladores
  pueden quedar de acuerdo al definir qué recursos estarán disponibles para los sub-estados. Los recursos dejan de estár disponibles cuando el superestado ya no esté vigente.

  Los equipos de desarrolladores o un desarrollador, pueden concentrar sus esfuerzo en la implementación de los métodos OnEntry, OnExit y OnEvent 
  para definir el comportamiento de la aplicación, siempre y cuando se sigan respetando los acuerdos establecidos para definir los superestados 

  El administrador de la máquina (CStateMachineManager) controla correctamente y de manera jerárquica las secuencias OnEntry y OnExit de cada estado y superestados
  de tal menera que la administración de recursos sea automática.

  Un estado puede abandonarse y trasitar hacia otro estado, solicitando al administrador a qué estado se desea realizar la transición. (CStateMachineManager::Transition)

  El administrador envia los eventos (mensajes) hacia el estado vigente o activo. Si el estado opta por no procesar dicho mensaje, el administrador reenviará
  el evento hacia su superestado para que éste lo procese, y así sucesivamente de ser necesario. 

  Para mayor información estudie referencias acerca de Automátas Jerárquicos.
  http://www.accu-usa.org/slides/samek0311.pdf


  M. en C. Pedro Arturo Cornejo Torres

  
*/

#define UPDATE 0xff4cf5a3
class CStateMachineManager:public CObjectStock
{
private:
	//Identificador del estado inicial (Cualquier estado de la máquina, el cuál será el estado activo tras ejecutar Start()
	unsigned long m_ulIDInitialState;
	//Identificador del estado actual 
	unsigned long m_ulIDCurrentState;
	//Identificador del estado que solicita la transición.
	unsigned long m_ulIDSourceState; 

	unsigned long m_ulTime;  //Diezmilésimas de segundo.
	unsigned long m_uldTime; //Diezmilesimas de segundo

	map<unsigned long,unsigned long> m_mapSuperStates;
	list<CEventBase*> m_lstQueuedEvents;
protected:
	/*
		El estado nulo, muy útil como estado pozo. (La máquina de estados se paraliza). El estado nulo siempre está en la máquina, y cualquier estado puede realizar
		una transición hacia éste estado, en caso de excepciones no manejadas.
		CStateNull consume todos los eventos si se realiza una transición hacia éste estado. 

		Debe invocarse a Reset() si se desea volver a iniciar ésta máquina desde el comienzo. No olvde invocar a Start.
	*/
	#define CLSID_CStateNull 0xffff846f
	class CStateNull:public CStateBase
	{
	public:
		unsigned long GetClassID(){ return CLSID_CStateNull;}
		const char*   GetClassString() { return "CStateMachineManager::CStateNull";}
	protected:
		virtual void OnEntry();
		virtual void OnExit();
		virtual unsigned long OnEvent(CEventBase* pEvent);
	};
	//Se utiliza para invocar la inicialización recurrente de un estado, y sus sub-estados, de manera descendente
	void InitState(unsigned long ulStateID);	
public:
	//El estado pozo
	CStateNull  m_StateNull;
	//Registra un estado e indica cual sub estado es el inicial, si es que tiene subestados. Si el estado no contiene sub-estados, ulIDInitialSubState debe ser 0
	void		RegisterState(CStateBase* pState,unsigned long ulIDState,unsigned long ulIDInitialSubState);
	//Registrados los estados, LinkToSuperState, establece la relación de parentezco entre el estado y super-estado
	void		LinkToSuperState(unsigned long ulIDState,unsigned long ulIDSuperState);
	//Rompe la relación de parentezco del estado indicado. Así el estado con ulIDState no pertenecerá a un super-estado
	void		UnlinkFromSuperState(unsigned long ulIDState);
	//Rompe todas las relaciones de parentezco de los estados. (No elimina los estados). Tras ejecutar éste método todos los estados son hermanos.
	void		ClearLinks(void);
	//Elimina del registro a un estado. Esto decrementará el contador de referencias del estado posiblemente eliminándolo.
	CStateBase* UnregisterState(unsigned long ulIDState);
	//Consulta el identificador del super-estado que contiene al estado indicado por ulIDState
	unsigned long QuerySuperStateOf(unsigned long ulIDState);
	//Realiza una transición hacia cualquier estado en la HSM, realizando la liberación e inicialización de estados necesarios. El estado activo será ulIDState y todos sus ancestros
	void		Transition(unsigned long ulIDState);
	//Establece cual de los estados de la HSM será el estado inicial. Cualquier estado puede ser estado inicial.
	void		SetInitialState(unsigned long ulIDState);
	//Inicializa la máquina de estados y la prepara para despachar eventos.
	void		Start();
	//Reinicia la máquina de estados, desactivando los estados que estaban activos. No elimina estados ni relación jerárquica. Debe invocarse a Start para comenzar de nuevo en un estado activo.
	void		Reset();
	//Despacha un evento hacia los estados activos, de manera jerarquica ascendente.
	void		Dispatch(CEventBase* pEvent);
	//Coloca un evento en la cola de mensajes. Los eventos acumulados en la cola, pueden ser despachados mediante ProcessEvents, tal y como fueron colocados en la cola. Todo evento que se encole, debe ser creado dinámicamente (new)
	void		PostEvent(CEventBase* pEvent);
	//Procesa todos los eventos en la cola de eventos. Los eventos son eliminados automáticamente. Todos los eventos encolados serán eliminados tras ser despachados (delete)
	void		ProcessEvents(void);
	//Obtiene el tiempo local de esta máquina en segundos en formato flotante
	float GetLocalTime(){return m_ulTime/10000.0f;}; //Segundos....
	//Obtiene el tiempo incremental en segundos de ésta máquina de estados en formato flotante
	float GetDeltaTime(){return m_uldTime/10000.0f;};  //Segundos
	//Tiempo incrementa uldTime diezmilésimas de segundo
	void  Time(unsigned long uldTime)
	{
		/*
			Es importante recordar, que las variables flotantes absorven los números pequeños, impidiendo que incrementen, por lo que 
			es exacto usar enteros que expresen partes de un segundo, en este caso diezmilésimas de segundo.

			Al usar enteros, estos siempre incrementan, evitándonos serios problemas de precisión.
		*/
		m_ulTime+=uldTime;
		m_uldTime=uldTime;
	};                 

	CStateMachineManager(void);
	virtual ~CStateMachineManager(void);
};

