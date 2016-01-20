
#include "database_ArpDynCatch.h"
#include "gethwArpDynTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SWITCH_ID0 0
#define SWITCH_ID1 1
#define SWITCH_ID2 2
#define SWITCH_ID3 3

#define SWITCHIPADDR0             "192.168.200.253"
#define SWITCHIPADDR1             "192.168.200.254"
#define SWITCHIPADDR2             "192.168.200.252"
#define SWITCHIPADDR3             "192.168.100.254"

#define PASSWORD                   "P@ssw0rd"

#ifndef Host_INFO_H
#define Host_INFO_H
typedef struct Host_INFO_H {
	unsigned char *ArpDynIpAdd;
	unsigned char *ArpDynMacAdd;

} HOST_INFO;
#endif
int Judge_IpAddr_in_database_ArpDyn_Table(const unsigned char *switchip,unsigned char *comm,
						IP_ArpDyn_TABLE_INFO *src_arpdyn_table_info,
						ArpDynTable_INFO *des_arpdyn_table_info);
