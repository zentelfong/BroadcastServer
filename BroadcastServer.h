#pragma once
#include "Common.h"

//多播服务器，监听广播

class BroadcastServer
{
public:
	BroadcastServer(void);
	~BroadcastServer(void);

	void StartServer();
	void StopServer();

	//收到广播请求
	virtual void OnRequest(uint32_t cmd,uint32_t reqid,char* buf,int len,sockaddr* addr);

	void Responce(uint32_t cmd,uint32_t reqid,const char* buf,int len,sockaddr* addr);
private:
	static void ThreadProc(void*);
	void DoProc();
	bool m_quit;
	HANDLE m_hEvent;
	SOCKET m_broadSocket;
};

