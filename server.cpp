
#include "stdio.h"

#include <WinSock2.h>
#include <windows.h>
#include <Ws2tcpip.h>
#include <stdint.h>

#include "BroadcastClient.h"
#include "BroadcastServer.h"

#pragma comment(lib,"Ws2_32.lib")


void Send()
{
    SOCKET s = ::socket(AF_INET, SOCK_DGRAM, 0);
    // 有效SO_BROADCAST选项
    BOOL bBroadcast = TRUE;
    ::setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)&bBroadcast, sizeof(BOOL));

    // 设置广播地址，这里的广播端口号（电台）是4567
    SOCKADDR_IN bcast;
    bcast.sin_family = AF_INET;
    bcast.sin_addr.s_addr =  ::inet_addr("255.255.255.255");
    bcast.sin_port = htons(4567);

    // 发送广播
    char sz[] = "this is xingoo 123. \r\n";
    while(TRUE)
    {
		//将我的端口广播出去,广播的信息包含，本机端口，ip，计算机名称，megicNumber
        ::sendto(s, sz, strlen(sz), 0, (sockaddr*)&bcast, sizeof(bcast));
        ::Sleep(1000);
    }
	
	closesocket(s);
}

void Recv()
{
   SOCKET s = ::socket(AF_INET, SOCK_DGRAM, 0);

    // 允许其它进程使用绑定的地址
    BOOL bReuse = TRUE;
    ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&bReuse, sizeof(BOOL));

    // 绑定到4567端口
    sockaddr_in si;
    si.sin_family = AF_INET;
    si.sin_port = 4567;
    si.sin_addr.S_un.S_addr = INADDR_ANY;
    ::bind(s, (sockaddr*)&si, sizeof(si));
    
    // 加入多播组
    ip_mreq    mcast;
    mcast.imr_interface.S_un.S_addr = INADDR_ANY;
    mcast.imr_multiaddr.S_un.S_addr = ::inet_addr("192.168.0.1");  // 多播地址为234.5.6.7
    ::setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mcast, sizeof(mcast));


    // 接收多播组数据
    printf(" 开始接收多播组上的数据... \n");
    char buf[1280];
    int nAddrLen = sizeof(si);
    while(TRUE)
    {
        int nRet = ::recvfrom(s, buf, strlen(buf), 0, (sockaddr*)&si, &nAddrLen);
        if(nRet != SOCKET_ERROR)
        {
            buf[nRet] = '\0';
            printf(buf);
        }
        else
        {
            int n = ::WSAGetLastError();
            break;
        }
    }

	closesocket(s);
}

void main()
{
	WSADATA wsaData;
	int err=WSAStartup(0x0202,&wsaData);
	if(err!=0)
	{
		return;
	}

	BroadcastServer server;
	server.StartServer();


	BroadcastClient client;
	client.StartClient();

	const char* test="hello world!";
	client.Broadcast(1,(char*)test,strlen(test)+1);

	//Recv();
	getchar();

	WSACleanup();
}


//
//// UDPBroadcastSend.cpp : Defines the entry point for the console application.
////
//
//#include "stdafx.h"
//#include<stdio.h>  
//#include <iostream>
//#include<winsock2.h>  
//#include<windows.h>  
//#pragma comment(lib,"ws2_32.lib")  
//
//using namespace std;
//
//int _tmain(int argc, _TCHAR* argv[])
//{  
//	SOCKET sock;  
//	sockaddr_in addrto;  
//	WSADATA wsdata;  
//	bool bsocket;  
//	char smsg[100] = {0};
//	cout<<"input what message to send\n:"<<endl;
//	cin>>smsg;
//	//启动SOCKET库，版本为2.0  
//	WSAStartup(0x0202,&wsdata);  
//
//	sock=WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,WSA_FLAG_OVERLAPPED);  
//	bsocket=true;  
//	//然后赋值给地址，用来从网络上的广播地址接收消息；  
//	addrto.sin_family=AF_INET;  
//	addrto.sin_addr.s_addr=INADDR_BROADCAST;  
//	addrto.sin_port=htons(9000);  
//	bool opt=true;  
//	//设置该套接字为广播类型，  
//	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char FAR *)&opt,sizeof(opt));  
//	int nlen=sizeof(addrto);  
//	char buf[100] = {0};
//	while(1)  
//	{  
//		addrto.sin_family=AF_INET;  
//		addrto.sin_addr.s_addr=INADDR_BROADCAST;  
//		addrto.sin_port=htons(9000);  
//
//		Sleep(1000);  
//		//从广播地址发送消息  
//		int ret=sendto(sock,smsg, strlen(smsg) + 1,0,(sockaddr*)&addrto,nlen);  
//		if(ret==SOCKET_ERROR)  
//		{  
//			printf("%d/n",WSAGetLastError());  
//		}  
//		else  
//		{         
//			printf("It'sock OK.\n");  
//		}  
//
//		ret = recvfrom( sock, buf, sizeof(buf), 0, (sockaddr*)&addrto, &nlen );
//		if (SOCKET_ERROR == ret)
//		{
//			printf("error on recv after send!\n");
//		}
//		else
//		{
//			printf("recv after send: %s\n", buf);
//		}
//	}  
//	return 0;  
//}  
//
//
//
//
//// UDPBroadcastRecv.cpp : Defines the entry point for the console application.
////
//
//#include "stdafx.h"
//#include<stdio.h>  
//#include<winsock2.h>  
//#include<windows.h>  
//#pragma comment(lib,"ws2_32.lib")  
//
//int _tmain(int argc, _TCHAR* argv[])
//{  
//	SOCKET sock;  
//	sockaddr_in from,a;  
//	WSADATA wsdata;  
//	bool optval;  
//	//启动SOCKET库，版本为2.0  
//	WSAStartup(0x0202,&wsdata);  
//	optval=true;  
//	//然后赋值给两个地址，一个用来绑定套接字，一个用来从网络上的广播地址接收消息；  
//	a.sin_family=AF_INET;  
//	a.sin_addr.s_addr=INADDR_ANY;  
//	a.sin_port=htons(9000);  
//	from.sin_family=AF_INET;  
//	from.sin_addr.s_addr=INADDR_BROADCAST;  
//	from.sin_port=htons(5050);  
//
//	int fromlength=sizeof(SOCKADDR);  
//	//用UDP初始化套接字  
//	sock=socket(AF_INET,SOCK_DGRAM,0);  
//	// 设置该套接字为广播类型，  
//	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char FAR *)&optval,sizeof(optval));  
//	// 把该套接字绑定在一个具体的地址上  
//	bind(sock,(sockaddr *)&a,sizeof(sockaddr_in));  
//	char buf[256];  
//	while(1)  
//	{//从广播地址接收消息，注意用来绑定的地址和接收消息的地址是不一样的  
//		recvfrom(sock,buf,256,0,(struct sockaddr FAR *)&from,(int FAR *)&fromlength);  
//		//Sleep(2000);  
//		printf("%sock\n",buf);  
//		sendto( sock, buf, strlen(buf) + 1, 0, (struct sockaddr FAR *)&from, fromlength );
//		ZeroMemory(buf,256);  
//	}  
//	return 0;
//}
