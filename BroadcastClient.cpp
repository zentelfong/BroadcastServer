#include "BroadcastClient.h"
#include <process.h>
#include "stdio.h"


BroadcastClient::BroadcastClient(void)
	:m_quit(false),m_broadSocket(INVALID_SOCKET),m_reqid(0)
{
	m_hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	InitializeCriticalSection(&m_reqLock);
}


BroadcastClient::~BroadcastClient(void)
{
	CloseHandle(m_hEvent);
	DeleteCriticalSection(&m_reqLock);
}

void BroadcastClient::ThreadProc(void* p)
{
	BroadcastClient* pThis=(BroadcastClient*)p;
	pThis->DoProc();
	_endthread();
}



void BroadcastClient::DoProc()
{
    //SOCKET s = ::socket(AF_INET, SOCK_DGRAM, 0);
	SOCKET s =WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
    // 有效SO_BROADCAST选项
    BOOL bBroadcast = TRUE;
    ::setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)&bBroadcast, sizeof(BOOL));

	int timeout=3000;
	::setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));


    // 设置广播地址，这里的广播端口号（电台）是4567
    SOCKADDR_IN bcast;
    bcast.sin_family = AF_INET;
    bcast.sin_addr.s_addr =  ::inet_addr("255.255.255.255");;
    bcast.sin_port = htons(BROADCAST_PORT);
	uint8_t buf[1400];
	BroadcastHeader* head=(BroadcastHeader*)buf;
	uint8_t* data=buf+sizeof(BroadcastHeader);


	int nlen=sizeof(bcast);  

    // 发送广播
    while(!m_quit)
    {
		BroadcastInfo* info=PopInfo();
		if(info)
		{
			head->megic=BROADCAST_MEGIC_NUMBER;
			head->commend=info->commend;
			head->reqid=info->reqid;

			int len=min(1200,info->data.size());
			memcpy(data,info->data.data(),len);

			int ret=sendto(s,(char*)buf, len+sizeof(BroadcastHeader),0,(sockaddr*)&bcast,nlen);
			PushInfo(info);

			if(ret==SOCKET_ERROR)  
			{
				printf("%d/n",WSAGetLastError());  
			}  
			else  
			{
				ret = recvfrom( s, (char*)buf, sizeof(buf), 0, (sockaddr*)&bcast, &nlen );
				if (SOCKET_ERROR == ret)
				{
					printf("error on recv after send!\n");
				}
				else
				{
					BroadcastHeader* header=(BroadcastHeader*)buf;
					if (header->megic==BROADCAST_MEGIC_NUMBER)
					{
						EnterCriticalSection(&m_reqLock);
						for (std::vector<BroadcastInfo*>::iterator i=m_reqs.begin();i!=m_reqs.end();++i)
						{
							BroadcastInfo* info=*i;
							if (info->reqid==header->reqid)
							{
								m_reqs.erase(i);
								delete info;
								break;
							}
						}
						LeaveCriticalSection(&m_reqLock);
						OnResponce(header->commend,(char*)buf+sizeof(BroadcastHeader),ret-sizeof(BroadcastHeader));
					}
				}
			}
		}
		Sleep(1000);
    }
	closesocket(s);
	SetEvent(m_hEvent);
}

void BroadcastClient::StartClient()
{
	m_quit=false;
	ResetEvent(m_hEvent);
	_beginthread(ThreadProc,0,this);
}

void BroadcastClient::StopClient()
{
	m_quit=true;
	WaitForSingleObject(m_hEvent,INFINITE); 
}

BroadcastInfo* BroadcastClient::PopInfo()
{
	BroadcastInfo* info=NULL;
	EnterCriticalSection(&m_reqLock);
	if(m_reqs.size()>0)
	{
		info=m_reqs.front();
		m_reqs.erase(m_reqs.begin());
	}
	LeaveCriticalSection(&m_reqLock);
	return info;
}
void BroadcastClient::PushInfo(BroadcastInfo* info)
{
	EnterCriticalSection(&m_reqLock);
	m_reqs.push_back(info);
	LeaveCriticalSection(&m_reqLock);
}


void BroadcastClient::Broadcast(uint32_t cmd,const char* buf,int len)
{
	BroadcastInfo* info2=new BroadcastInfo;
	info2->commend=cmd;
	info2->reqid=++m_reqid;
	info2->data.resize(len);
	for (int i=0;i<len;++i)
	{
		info2->data[i]=(uint8_t)buf[i];
	}
	PushInfo(info2);
}

void BroadcastClient::OnResponce(uint32_t cmd,const char* buf,int len)
{
	switch(cmd)
	{
	case CID_FIND_SERVER:
		{

			printf(buf);
			//::sendto(m_sendSocket,,,addr,addrLen);
		}
		break;
	default:
		break;
	}
}