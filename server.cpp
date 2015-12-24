
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
    // ��ЧSO_BROADCASTѡ��
    BOOL bBroadcast = TRUE;
    ::setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)&bBroadcast, sizeof(BOOL));

    // ���ù㲥��ַ������Ĺ㲥�˿ںţ���̨����4567
    SOCKADDR_IN bcast;
    bcast.sin_family = AF_INET;
    bcast.sin_addr.s_addr =  ::inet_addr("255.255.255.255");
    bcast.sin_port = htons(4567);

    // ���͹㲥
    char sz[] = "this is xingoo 123. \r\n";
    while(TRUE)
    {
		//���ҵĶ˿ڹ㲥��ȥ,�㲥����Ϣ�����������˿ڣ�ip����������ƣ�megicNumber
        ::sendto(s, sz, strlen(sz), 0, (sockaddr*)&bcast, sizeof(bcast));
        ::Sleep(1000);
    }
	
	closesocket(s);
}

void Recv()
{
   SOCKET s = ::socket(AF_INET, SOCK_DGRAM, 0);

    // ������������ʹ�ð󶨵ĵ�ַ
    BOOL bReuse = TRUE;
    ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&bReuse, sizeof(BOOL));

    // �󶨵�4567�˿�
    sockaddr_in si;
    si.sin_family = AF_INET;
    si.sin_port = 4567;
    si.sin_addr.S_un.S_addr = INADDR_ANY;
    ::bind(s, (sockaddr*)&si, sizeof(si));
    
    // ����ಥ��
    ip_mreq    mcast;
    mcast.imr_interface.S_un.S_addr = INADDR_ANY;
    mcast.imr_multiaddr.S_un.S_addr = ::inet_addr("192.168.0.1");  // �ಥ��ַΪ234.5.6.7
    ::setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mcast, sizeof(mcast));


    // ���նಥ������
    printf(" ��ʼ���նಥ���ϵ�����... \n");
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
//	//����SOCKET�⣬�汾Ϊ2.0  
//	WSAStartup(0x0202,&wsdata);  
//
//	sock=WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,WSA_FLAG_OVERLAPPED);  
//	bsocket=true;  
//	//Ȼ��ֵ����ַ�������������ϵĹ㲥��ַ������Ϣ��  
//	addrto.sin_family=AF_INET;  
//	addrto.sin_addr.s_addr=INADDR_BROADCAST;  
//	addrto.sin_port=htons(9000);  
//	bool opt=true;  
//	//���ø��׽���Ϊ�㲥���ͣ�  
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
//		//�ӹ㲥��ַ������Ϣ  
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
//	//����SOCKET�⣬�汾Ϊ2.0  
//	WSAStartup(0x0202,&wsdata);  
//	optval=true;  
//	//Ȼ��ֵ��������ַ��һ���������׽��֣�һ�������������ϵĹ㲥��ַ������Ϣ��  
//	a.sin_family=AF_INET;  
//	a.sin_addr.s_addr=INADDR_ANY;  
//	a.sin_port=htons(9000);  
//	from.sin_family=AF_INET;  
//	from.sin_addr.s_addr=INADDR_BROADCAST;  
//	from.sin_port=htons(5050);  
//
//	int fromlength=sizeof(SOCKADDR);  
//	//��UDP��ʼ���׽���  
//	sock=socket(AF_INET,SOCK_DGRAM,0);  
//	// ���ø��׽���Ϊ�㲥���ͣ�  
//	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char FAR *)&optval,sizeof(optval));  
//	// �Ѹ��׽��ְ���һ������ĵ�ַ��  
//	bind(sock,(sockaddr *)&a,sizeof(sockaddr_in));  
//	char buf[256];  
//	while(1)  
//	{//�ӹ㲥��ַ������Ϣ��ע�������󶨵ĵ�ַ�ͽ�����Ϣ�ĵ�ַ�ǲ�һ����  
//		recvfrom(sock,buf,256,0,(struct sockaddr FAR *)&from,(int FAR *)&fromlength);  
//		//Sleep(2000);  
//		printf("%sock\n",buf);  
//		sendto( sock, buf, strlen(buf) + 1, 0, (struct sockaddr FAR *)&from, fromlength );
//		ZeroMemory(buf,256);  
//	}  
//	return 0;
//}
