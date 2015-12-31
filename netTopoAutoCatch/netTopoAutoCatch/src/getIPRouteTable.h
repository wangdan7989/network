#include "my_snmp_bulkwalk.h"
#include <stdio.h>


#define		IP_ROUTE_TABLE_DEST			".1.3.6.1.2.1.4.21.1.1"
#define		IP_ROUTE_TABLE_IF_INDEX		".1.3.6.1.2.1.4.21.1.2"
#define		IP_ROUTE_TABLE_NEXT_HOP		".1.3.6.1.2.1.4.21.1.7"
#define		IP_ROUTE_TABLE_TYPE			".1.3.6.1.2.1.4.21.1.8"
#define		IP_ROUTE_TABLE_MASK			".1.3.6.1.2.1.4.21.1.11"



typedef struct IP_ROUTE_TABLE_INFO {
	unsigned char ipRouteTableDest[16];
	unsigned long ipRouteTableIfIndex;
	unsigned char ipRouteTableNextHop[16];
	unsigned int  ipRouteTableType;
	unsigned char ipRouteTableMask[16];
}IP_ROUTE_TABLE_INFO;

#ifndef 	MAX_MY_OID_RESULT_LEN_DEFINE
#define		MAX_MY_OID_RESULT_LEN		32
#endif

int get_ip_route_table_info_from_device_by_snmp(const unsigned char *ip, 
												const unsigned char *comm,
												IP_ROUTE_TABLE_INFO *ip_route_table_info,
												unsigned int *ip_route_table_rows);