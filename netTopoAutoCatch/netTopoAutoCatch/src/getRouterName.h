#include "my_snmp_bulkwalk.h"
#include <stdio.h>


#define		IP_ROUTE_NAME_INFO			".1.3.6.1.2.1.1.5"


int get_ip_route_name_from_device_by_snmp(const unsigned char *ip, 
										  const unsigned char *comm,
										  unsigned char *ip_route_name);