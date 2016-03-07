
#include "database_ArpDynCatch.h"
#include "gethwArpDynTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SWITCH_ID0 0
#define SWITCH_ID1 1
#define SWITCH_ID2 2
#define SWITCH_ID3 3

#define SWITCHIPADDR0             "192.168.200.253"
#define SWITCHIPADDR1             "192.168.200.254"
#define SWITCHIPADDR2             "192.168.200.252"
#define SWITCHIPADDR3             "192.168.100.254"

#define PASSWORD                   "P@ssw0rd"

#ifndef FLAG
#define FLAG
typedef int Flag;
#endif

#ifndef Host_INFO_H
#define Host_INFO_H
typedef struct Host_INFO_H {
	unsigned int index;
	unsigned int flag;
	unsigned char *ArpDynIpAdd;
	unsigned char *ArpDynMacAdd;

} HOST_INFO;
#endif
int Judge_IpAddr_in_database_ArpDyn_Table(const unsigned char *switchip,unsigned char *comm,
						IP_ArpDyn_TABLE_INFO *src_arpdyn_table_info,
						ArpDynTable_INFO *des_arpdyn_table_info);

/*  judge IpAddr in database_ArpDyn_TABLE DES=DATABASE*/
/*--------------------------------------------------------*/
/**
		falg=0:illegal
		flag=1:legal
**/
/*--------------------------------------------------------*/
HOST_INFO* Judge_IpAddr_is_Legal(const unsigned char *switchip,unsigned char *comm,
						IP_ArpDyn_TABLE_INFO *src_arpdyn_table_info,
						ArpDynTable_INFO *des_arpdyn_table_info,
						HOST_INFO *Resultdata,unsigned int *relength);
/**--------------------------------------------------------------------**/
/**             Judge_IpAddr is illegal from two table src and des     **/
/**
		falg=0:illegal
		flag=1x:legal    flag=11:ip and mac is legal
				 flag=10:ip is legal and mac is illegal    
**/
/**--------------------------------------------------------------------**/

Flag Judge_IpAddr_from_Src_And_Des(IP_ArpDyn_TABLE_INFO *src_arpdyn_table_info,unsigned int *src_num,
						ArpDynTable_INFO *des_arpdyn_table_info,unsigned int *des_num,
							HOST_INFO *legal,HOST_INFO *illegal,unsigned int *legcount,unsigned int *illcount);
/**--------------------------------------------------------------------**/
/**             copy HOST_INFO src to des,and length is src length,index is the des first     **/
/**--------------------------------------------------------------------**/
void Copy_Hostinfo_Arry(int index,int length,HOST_INFO *src,HOST_INFO *des);
