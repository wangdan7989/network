
#include "/usr/include/mysql/mysql.h" 

#define DATABASE_DEBUG_MESSAGE
#undef DATABASE_DEBUG_MESSAGE

/* Define the micro related connection database */
#ifndef DB_CONF_FILE_H
#define DB_CONF_FILE_H
#define DB_IPADDR "localhost"
#define DB_USER   "root"
#define DB_PASSWORD "123"
#define DB_NAME   "netdata"
#endif

#ifndef DATABASE_RETURN_H
#define DATABASE_RETURN_H

#define DATABASE_OK 0
#define DATABASE_ERROR 1
#define DATABASE_INFEASIBLE -1
#define DATABASE_TRUE 2
#define DATABASE_FALSE 3

#endif

#define SQLBUFFLEN 2048
#define MAX_OBJECT_COUNT 1024


#ifndef STATUS
#define STATUS
typedef int Status;
#endif

#ifndef ID_H
#define ID_H
typedef unsigned int SwitchId;
typedef unsigned int RouterID;
typedef unsigned int HostID;
typedef unsigned int LocalRID;
typedef unsigned int PeerRID;
#endif

#ifndef NAME
#define NAME
typedef char *RouterName;
typedef char *HostName;
#endif


#ifndef HOST_INFO_H
#define HOST_INFO_H
typedef struct HOST_INFO {
	char hostID[64];
	char hostName[64];
	unsigned int hostType;  /* 0 is HOST, 1 is SWITCH, 2 IS OTHERS */
	unsigned int parentIfIndex;
	char hostIpAddr[16];
	char hostAddrMask[16];
} HOST_INFO;
#endif

#ifndef SUBNET_INFO_H
#define SUBNET_INFO_H
typedef struct SUBNET_INFO {
	unsigned int routerID;	
	unsigned int subnetIfIndex;
	char subnetIp[16];
	char subnetMask[16];
	unsigned int hostVectorNum;
} SUBNET_INFO;
#endif

#ifndef ROUTER_INFO_H
#define ROUTER_INFO_H
typedef struct ROUTER_INFO {
	unsigned int routerID;
	char *routerName;
	char *flagIpAddr;
	unsigned int ipAddrTableNum;
	unsigned int subnetVectorNum;
} ROUTER_INFO;
#endif

#ifndef ROUTER_CONNECT_INFO_H
#define ROUTER_CONNECT_INFO_H
typedef struct ROUTER_CONNECT_INFO {
	unsigned int localRID;
	char localRAddr[16];
	unsigned int peerRID;
	char peerRAddr[16];
	unsigned int localRIfIndex;
	unsigned int peerRIfIndex;
} ROUTER_CONNECT_INFO;
#endif

#ifndef IPADDRTABLE_INFO_H
#define IPADDETABLE_INFO_H
typedef struct IPADDRTABLE_INFO {
	unsigned int routerID;
	unsigned int ipAddrTableEntIfIndex;
	char ipAddrTableEntIp[16];
	char ipAddrTableEntMask[16];
} IPADDRTABLE_INFO;
#endif

#ifndef COMM_INFO_H
#define COMM_INFO_H
typedef struct COMM_INFO {
	unsigned int ID;
	char ip[16];
	char comm[32];
} COMM_INFO;
#endif

/* Database New Connected Function */
Status Database_Connect(MYSQL *my_connection);
/* Database New DisConnected Function */
Status Database_DisConnect(MYSQL *my_connection);
/* ---------------------------------------------------------- */
/*                Switch_Table   functions                    */
/* ---------------------------------------------------------- */
Status ArpDyn_Info_Table_Insert(MYSQL *my_connection,SwitchId SwitchId,unsigned int ArpDynIfIndex,
					char *ArpDynIpAdd,unsigned int ArpDynVrf,char *ArpDynMacAdd,
						unsigned int ArpDynVlanId,unsigned int ArpDynOutIfInd,unsigned int ArpDynExpireTime);

/* ---------------------------------------------------------- */
/*                Router_Table   functions                    */
/* ---------------------------------------------------------- */

/* Router_Info_Table Insert Function */
Status Router_Info_Table_Insert(MYSQL *my_connection, RouterID RouterID, char *RouterName, 
								char *FlagIpAddr, unsigned int IpAddeTableNum, unsigned int SubnetVectorNum );
/* Functions:  Get RouterInfo */
Status Get_Router_Info(MYSQL *my_connection,ROUTER_INFO *router_info, unsigned int *router_info_nums, const unsigned int RouterID);

/* delete in the Router table where RouterID = ? */
Status Delete_Router_Table_By_RouterID(MYSQL *my_connection,const int RouterID) ;

Status Update_Router_info_int(MYSQL *my_connection, unsigned int RouterID , char *Rowname,unsigned int value) ;
Status Update_Router_info_char(MYSQL *my_connection, unsigned int RouterID , char *Rowname,char *value) ;


/* ---------------------------------------------------------- */
/*              IpAddrTable_Table   functions                 */
/* ---------------------------------------------------------- */

/*Insert information into IpAddrTable table*/
Status IpAddrTable_Info_Table_Insert(MYSQL *my_connection, RouterID RouterID, unsigned int IpAddrTableEntIfIndex, char *IpAddrTableEntIp, 
								char *IpAddrTableEntMask );
/*Get IpAddrTable_Info*/
Status Get_IpAddrTable_Info(MYSQL *my_connection,IPADDRTABLE_INFO *ipaddrtable_info, unsigned int *ipaddrtable_info_nums, const unsigned int RouterID);

/* delete in the IpAddrTable table where RouterID = ? */
Status Delete_IpAddrTable_Table_By_RouterID(MYSQL *my_connection,const int RouterID) ;
/*update IpAddrTable Table (int)*/
Status Update_IpAddrTable_info_int(MYSQL *my_connection, unsigned int RouterID , char *Rowname,unsigned int value) ;
/*update IpAddrTable Table (char)*/
Status Update_IpAddrTable_info_char(MYSQL *my_connection, unsigned int RouterID , char *Rowname,char *value) ;

/* ---------------------------------------------------------- */
/*                 RouterConnect    functions                 */
/* ---------------------------------------------------------- */

/*Insert information into RouterConnect table*/
Status RouterConnect_Info_Table_Insert(MYSQL *my_connection, LocalRID LocalRID, PeerRID PeerRID, char *LocalRAddr, 
								char *PeerRAddr);
Status Get_RouterConnect_Info(MYSQL *my_connection,ROUTER_CONNECT_INFO *router_connect, unsigned int *routerconnect_info_nums, const unsigned int LocalRID);


/* delete in the RouterConnect table where LocalRID and PeerRID */
Status Delete_RouterConnect_Table_By_LocalRID_PeerRID(MYSQL *my_connection,const int LocalRID,const int PeerRID) ;
/*update RouterConnect Table (int)*/
Status Update_RouterConnect_info_int(MYSQL *my_connection, unsigned int LocalRID ,unsigned int PeerRID, char *Rowname,unsigned int value) ;
/*update RouterConnect Table (char)*/
Status Update_RouterConnect_info_char(MYSQL *my_connection, unsigned int LocalRID , unsigned int PeerRID, char *Rowname,char *value) ;
/* ---------------------------------------------------------- */
/*                      Subnet    functions                   */
/* ---------------------------------------------------------- */

/*Insert information into Subnet table*/
Status Subnet_Info_Table_Insert(MYSQL *my_connection, RouterID RouterID, unsigned int SubnetIfIndex,char *SubnetIp, 
								char *SubnetMask,  unsigned int HostVectorNum);

/*Get Subnet_Info*/
Status Get_Subnet_Info(MYSQL *my_connection,SUBNET_INFO *subnet_info, unsigned int *subnet_info_nums, const unsigned int RouterID);

/* delete in the Subnet table where RouterID and SubnetIfIndex */
Status Delete_IpAddrTable_Table_By_RouterID_SubnetIfIndex(MYSQL *my_connection,const int RouterID,int SubnetIfIndex) ;
/*update Subnet Table (int)*/
Status Update_Subnet_info_int(MYSQL *my_connection, unsigned int RouterID ,int SubnetIfIndex, char *Rowname,unsigned int value) ;
/*update Subnet Table (char)*/
Status Update_Subnet_info_char(MYSQL *my_connection, unsigned int RouterID ,int SubnetIfIndex, char *Rowname,char *value) ;
/* ---------------------------------------------------------- */
/*                        Host    functions                   */
/* ---------------------------------------------------------- */

/*Insert information into Host table*/
Status Host_Info_Table_Insert(MYSQL *my_connection, HostID HostID, unsigned int SubnetIfIndex,char *HostName, 
								unsigned int HostType,char *HostIpAddr,  char *HostAddrMask);

/*Get Host_Info*/
Status Get_Host_Info(MYSQL *my_connection,HOST_INFO *host_info, unsigned int *host_info_nums, const unsigned int SubnetIfIndex);

/* delete in the Host table where RouterID = ? */
Status Delete_Host_Table_By_RouterID(MYSQL *my_connection,const int HostID) ;
/*update Host Table (int)*/
Status Update_Host_info_int(MYSQL *my_connection, unsigned int HostID , char *Rowname,unsigned int value) ;
/*update Host Table (char)*/
Status Update_Host_info_char(MYSQL *my_connection, unsigned int HostID , char *Rowname,char *value) ;
