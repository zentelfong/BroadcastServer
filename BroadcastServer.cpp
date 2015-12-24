#include "BroadcastServer.h"
#include <process.h>
#include "stdio.h"


BroadcastServer::BroadcastServer(void)
	:m_quit(false),m_broadSocket(INVALID_SOCKET)
{
	m_hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
}


BroadcastServer::~BroadcastServer(void)
{
	CloseHandle(m_hEvent);
}

void BroadcastServer::ThreadProc(void* p)
{
	BroadcastServer* pThis=(BroadcastServer*)p;
	pThis->DoProc();
	_endthread();
}


void BroadcastServer::DoProc()
{
    m_broadSocket = ::socket(AF_INET, SOCK_DGRAM, 0);


    // 允许其它进程使用绑定的地址
    bool bReuse = true;
    ::setsockopt(m_broadSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&bReuse, sizeof(BOOL));

	int timeout=1000;
	::setsockopt(m_broadSocket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));

    // 绑定端口
    sockaddr_in si;
    si.sin_family = AF_INET;
    si.sin_port = htons(BROADCAST_PORT);
    si.sin_addr.S_un.S_addr = INADDR_ANY;
    ::bind(m_broadSocket, (sockaddr*)&si, sizeof(si));
    
    // 加入多播组
    ip_mreq mcast;
    mcast.imr_interface.S_un.S_addr = INADDR_ANY;
    mcast.imr_multiaddr.S_un.S_addr = ::inet_addr("192.168.0.1");  // 多播地址为234.5.6.7
    ::setsockopt(m_broadSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mcast, sizeof(mcast));
	

    char buf[MAX_PACKET_SIZE];
    int nAddrLen = sizeof(si);
    while(!m_quit)
    {
        int nRet = ::recvfrom(m_broadSocket, buf, strlen(buf), 0, (sockaddr*)&si, &nAddrLen);
        if(nRet != SOCKET_ERROR)
        {
			BroadcastHeader* header=(BroadcastHeader*)buf;
			if (header->megic==BROADCAST_MEGIC_NUMBER)
			{
				OnRequest(header->commend,header->reqid,buf+sizeof(BroadcastHeader),nRet-sizeof(BroadcastHeader),(sockaddr*)&si);
			}
			else
			{
				printf("未知广播不处理");
			}
        }
    }
	closesocket(m_broadSocket);
	m_broadSocket=INVALID_SOCKET;
	SetEvent(m_hEvent);
}

void BroadcastServer::StartServer()
{
	m_quit=false;
	ResetEvent(m_hEvent);
	_beginthread(ThreadProc,0,this);
}

void BroadcastServer::StopServer()
{
	m_quit=true;
	WaitForSingleObject(m_hEvent,INFINITE); 
}

void BroadcastServer::Responce(uint32_t cmd,uint32_t reqid,const char* buf,int len,sockaddr* addr)
{
	char temp[MAX_PACKET_SIZE];
	BroadcastHeader *responce=(BroadcastHeader*)temp;
	responce->megic=BROADCAST_MEGIC_NUMBER;
	responce->commend=cmd;
	responce->reqid=reqid;

	memcpy(temp+sizeof(BroadcastHeader),buf,len);
	::sendto(m_broadSocket,temp,len+sizeof(BroadcastHeader),0,(sockaddr*)addr,sizeof(sockaddr_in));
}


void BroadcastServer::OnRequest(uint32_t cmd,uint32_t reqid,char* buf,int len,sockaddr* addr)
{
	switch(cmd)
	{
	case CID_FIND_SERVER:
		{
			printf(buf);
			const char* test="abcdefg";
			Responce(cmd,reqid,test,strlen(test)+1,addr);
		}
		break;
	default:
		break;
	}
}










