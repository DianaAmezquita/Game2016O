#pragma once
#include "HSM\EventListener.h"
#include "HSM\EventBase.h"
#include "HSM\StateMachineManager.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <list>
#include <string>
#include <map>
using namespace std;
class CNetProcessor;
struct CLIENT
{
	SOCKET Client;
	// Add data for game
	string NickName;
	CNetProcessor* pOwner;
};

#define EVENT_NETWORK 0xaa55aa55
class CEventNetwork : public CEventBase
{
	friend CNetProcessor;
protected:
	SOCKET Source; // Quien fue la fuente del evento. Socket fuente del evento Server. Este dato no es transmitible
public:

	unsigned long m_nDataSize; // Datagrama de aplicacion
	unsigned char m_Data[512]; // Desde el punto de vista de la aplicacion siempre asigna 512 pero solamente transmite el tamaño usaod
};

class CNetProcessor :
	public CEventListener
{
protected:
	WSADATA m_wsa;
	SOCKET m_Listener; // Socket de escucha por si yo soy el servidor
	map<SOCKET, CLIENT> m_mapClient;
	SOCKET m_Server; // por si yo soy el que me conecta a un servidor
	list<CEventNetwork*> m_lstEventsToSend; // a ser enviados
	list<CEventNetwork*> m_lstEventsReceived;
	CRITICAL_SECTION m_csLock; //Mutex
	CStateMachineManager* m_pHSMOwner;
public:
	void Lock() { EnterCriticalSection(&m_csLock); }
	void Unlock() { LeaveCriticalSection(&m_csLock); }

	unsigned long OnEvent(CEventBase* pEvent);
	bool InitNetwork(void);
	bool Connect(wchar_t* pszAddress); //Conectar a una partida remota
	void DoNetworkTasks(void); // Saca lo que esta en la lista de eventos y recoge los que le enviaron a el
	void Uninitialize(void); // Avisa que te saliste
	static DWORD WINAPI ServiceListener(CNetProcessor* pNetworkProcessor);
	static DWORD WINAPI ClientThread(CLIENT* pClient);
	CNetProcessor(CStateMachineManager* pHSMOwner); // es importante pasar el propietario
	virtual ~CNetProcessor();
};

