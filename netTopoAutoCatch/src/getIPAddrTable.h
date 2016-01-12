#include "./common/my_snmp_bulkwalk.h"
#include <stdio.h>


#define		IP_ADDR_TABLE_ENT_IP			".1.3.6.1.2.1.4.20.1.1"
#define		IP_ADDR_TABLE_ENT_IF_INDEX		".1.3.6.1.2.1.4.20.1.2"
#define		IP_ADDR_TABLE_ENT_MASK			".1.3.6.1.2.1.4.20.1.3"


#ifndef	IP_ADDR_TABLE_INFO_DEFINE
#define IP_ADDR_TABLE_INFO_DEFINE
typedef struct IP_ADDR_TABLE_INFO {
	unsigned char ipAddrTableEntIp[16];
	unsigned long ipAddrTableEntIfIndex;
	unsigned char ipAddrTableEntMask[16];
}IP_ADDR_TABLE_INFO;
#endif

#ifndef 	MAX_MY_OID_RESULT_LEN_DEFINE
#define		MAX_MY_OID_RESULT_LEN		32
#endif

int get_ip_addr_table_info_from_device_by_snmp(const unsigned char *ip, 
											   const unsigned char *comm,
											   IP_ADDR_TABLE_INFO *ip_route_table_info,
											   unsigned int *ip_route_table_rows);
