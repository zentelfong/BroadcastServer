#pragma once
#include "Common.h"

//�ಥ�������������㲥

class BroadcastServer
{
public:
	BroadcastServer(void);
	~BroadcastServer(void);

	void StartServer();
	void StopServer();

	//�յ��㲥����
	virtual void OnRequest(uint32_t cmd,uint32_t reqid,char* buf,int len,sockaddr* addr);

	void Responce(uint32_t cmd,uint32_t reqid,const char* buf,int len,sockaddr* addr);
private:
	static void ThreadProc(void*);
	void DoProc();
	bool m_quit;
	HANDLE m_hEvent;
	SOCKET m_broadSocket;
};

