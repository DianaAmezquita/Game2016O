#include "stdafx.h"
#include "NetProcessor.h"

unsigned long CNetProcessor::OnEvent(CEventBase * pEvent)
{
	((CEventNetwork*)pEvent)->Source = 0; // soy yo cquien lo emite,, lo que venga diferente de 0  es otro
	Lock();
	m_lstEventsToSend.push_back((CEventNetwork*)pEvent); // yo lo genere no lo puedo confundir con otros
	Unlock();
	return 0;
}

bool CNetProcessor::InitNetwork(void)
{
	WSAStartup(MAKEWORD(2, 2), &m_wsa);
	DWORD dwThreadID;
	SOCKADDR_IN Service;
	m_Listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	InetPton(AF_INET, L"0.0.0.0", &Service.sin_addr);
	Service.sin_port = htons(6112);
	Service.sin_family = AF_INET;
	if (SOCKET_ERROR == bind(m_Listener, (SOCKADDR*)&Service, sizeof(SOCKADDR_IN)))
	{
		// No pudiste arrancar como servidor pero no implica que no puedes conectar 
		closesocket(m_Listener);
		m_Listener = 0;
		return false;
	}
	// Si eres servidor entonces activas el hilo de escucha
	listen(m_Listener, SOMAXCONN);
	CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)CNetProcessor::ServiceListener, this, 0, &dwThreadID);
	return true;
}

bool CNetProcessor::Connect(wchar_t * pszAddress)
{
	SOCKET Server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN ServerAddress;
	memset(&ServerAddress, 0, sizeof(ServerAddress));
	InetPton(AF_INET, pszAddress, &ServerAddress.sin_addr); // Estamos pidiendo la direccion de conexion
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(6112); // NOs conectamos al puerto del server
	// Ahora me conecto
	if (SOCKET_ERROR == connect(Server, (SOCKADDR*)&ServerAddress, sizeof(ServerAddress)))
	{
		// Cerrar si no se conecta 
		closesocket(Server);
		return false;
	}

	// Como si me conecte 
	CLIENT Cliente;
	Cliente.NickName = "Anonimo";
	Cliente.pOwner = this;
	Cliente.Client = Server; // CUando nos conectamos mi servidor es mi cliente
	Lock();
	m_mapClient.insert_or_assign(Server, Cliente);
	Unlock();
	return true;
}

void CNetProcessor::DoNetworkTasks(void)
{
	// Si recibi eventos los voy a preparar para enviar excepto el socket. Hay que copiarños
	// La lista de recibidos les hago una copia
	Lock();
	for (auto s : m_lstEventsReceived)
	{
		auto d = new CEventNetwork();
		*d = *s;
		m_lstEventsToSend.push_back(d);
	}
	while (!m_lstEventsToSend.empty())
	{
		CEventNetwork* pEvent = m_lstEventsToSend.front();
		// a toda la lista subscrita a mi de clientes
		
		for (auto c : m_mapClient)
		{
			// El cliente tiene que ser distinto a quien lo envio
			if (pEvent->Source != c.first)
			{
				// Eventos a enviar
				send(c.second.Client, (char*)&pEvent->m_nDataSize, sizeof(unsigned long), 0);
				send(c.second.Client, (char*)pEvent->m_Data, pEvent->m_nDataSize, 0);
			}
			

		}
		m_lstEventsToSend.pop_front();
		
		delete pEvent; // ya fue transferido

	}

	//procesar los eventos. Tengo que postear los eventos NO HACER DISPATCH!!
	for (auto e : m_lstEventsReceived)
	{
		m_pHSMOwner->PostEvent(e);
	}
	m_lstEventsReceived.clear();
	Unlock();
}

DWORD CNetProcessor::ServiceListener(CNetProcessor * pNetworkProcessor)
{
	while (1)
	{
		DWORD dwThreadID;
		SOCKADDR_IN ClientAddress;
		int nClientAddrSize = sizeof(SOCKADDR_IN);
		//int nClientAddrSize = 0;
		SOCKET Client = accept(pNetworkProcessor->m_Listener, (SOCKADDR*)&ClientAddress, &nClientAddrSize);
		CLIENT &NewClient = *new CLIENT;
		NewClient.Client = Client;
		NewClient.NickName = "Anonimo";
		NewClient.pOwner = pNetworkProcessor;
		pNetworkProcessor->Lock();
		pNetworkProcessor->m_mapClient.insert_or_assign(Client, NewClient);
		pNetworkProcessor->Unlock();
		
		//Ahora otro hilo para escuchar al cliente
		CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)CNetProcessor::ClientThread, &NewClient, 0, &dwThreadID);


	}
	return 0;
}
// Nada mas recibe en segundo plano
DWORD CNetProcessor::ClientThread(CLIENT * pClient)
{
	// Mi unico trabajo de escuchar a mis clients para todas sus solicitudes
	while (1)
	{
		CEventNetwork* pEventNetwork = new CEventNetwork;
		pEventNetwork->m_ulEventType = EVENT_NETWORK;
		if (SOCKET_ERROR == recv(pClient->Client, (char*)pEventNetwork->m_nDataSize, sizeof(unsigned long), 0))
		{
			delete pEventNetwork;
			break;
		}
			
		if (SOCKET_ERROR == recv(pClient->Client, (char*)pEventNetwork->m_Data, pEventNetwork->m_nDataSize, 0))
		{
			delete pEventNetwork;
			break;
		}
		pClient->pOwner->Lock();
		pEventNetwork->Source = pClient->Client;
		pClient->pOwner->m_lstEventsReceived.push_back(pEventNetwork);
		pClient->pOwner->Unlock();
	}
	// Unsubscription
	pClient->pOwner->Lock();
	pClient->pOwner->m_mapClient.erase(pClient->Client);
	pClient->pOwner->Unlock();
	closesocket(pClient->Client);
	delete pClient;
	return 0;
}

CNetProcessor::CNetProcessor(CStateMachineManager* pHSMOwner)
{
	m_pHSMOwner = pHSMOwner;
	InitializeCriticalSection(&m_csLock);
}


CNetProcessor::~CNetProcessor()
{
	DeleteCriticalSection(&m_csLock);
}
