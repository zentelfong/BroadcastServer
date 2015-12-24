#pragma once
#include "Common.h"

class BroadcastClient
{
public:
	BroadcastClient(void);
	~BroadcastClient(void);

	void StartClient();
	void StopClient();

	//发送广播消息
	void Broadcast(uint32_t cmd,const char* buf,int len);
	virtual void OnResponce(uint32_t cmd,const char* buf,int len);
private:
	static void ThreadProc(void*);
	void DoProc();

	BroadcastInfo* PopInfo();
	void PushInfo(BroadcastInfo*);
	bool m_quit;
	HANDLE m_hEvent;
	SOCKET m_broadSocket;
	uint32_t m_reqid;
	CRITICAL_SECTION m_reqLock;
	std::vector<BroadcastInfo*> m_reqs;
};

