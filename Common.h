#pragma once
#include <stdint.h>
#include <WinSock2.h>
#include <windows.h>
#include <Ws2tcpip.h>
#include <vector>

#define BROADCAST_MEGIC_NUMBER 0x1c5545b
#define MAX_PACKET_SIZE 1200
#define BROADCAST_PORT 4567

enum CommendId{
	CID_FIND_SERVER=1,
};

struct BroadcastHeader
{
	uint32_t megic;
	uint32_t commend;
	uint32_t reqid;
};


struct BroadcastInfo
{
	uint32_t commend;
	uint32_t reqid;
	std::vector<uint8_t> data;
};







