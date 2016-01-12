#include "getIPAddrTable.h"
#include "getIPRouteTable.h"
#include "getIPForWardingInfo.h"
#include "getRouterName.h"
#include "database_netTopoAutoCatch.h"

#define		MAX_ROUTE_QUEUE_LEN		1024
#define		MAX_ROUTE_CONNECT_LEN	1024
#define		MAX_IP_ADDRESS_LEN		16
#define 	MAX_ROUTER_NAME_LEN		64
#define		MAX_SUBNET_LEN			128

#define		UNDO_ROUTER_FLAG		0
#define		DONE_ROUTER_FLAG		1


typedef struct HOST {
	char hostID[64];
	char hostName[64];
	unsigned int hostType;  /* 0 is HOST, 1 is SWITCH, 2 IS OTHERS */
	unsigned int parentIfIndex;
	char hostIpAddr[16];
	char hostAddrMask[16];
} HOST;

typedef struct SUBNET {
	unsigned int subnetIfIndex;
	char sunnetIP[16];
	char subnetMask[16];
	HOST *hostVector;
	unsigned int hostVectorNum;
} SUBNET;

typedef struct ROUTER {
	unsigned int routerID;
	char *routerName;
	char *flagIPAddr;
	IP_ADDR_TABLE_INFO *ipAddrTable;
	SUBNET *subnetVector;
	unsigned int processFlag;
	unsigned int ipAddrTableNum;
	unsigned int subnetVectorNum;
} ROUTER;
#ifndef ROUTER_CONNECT
typedef struct ROUTER_CONNECT {
	unsigned int localRID;
	char localRaddr[16];
	unsigned int peerRID;
	char peerRAddr[16];
} ROUTER_CONNECT;
#endif
