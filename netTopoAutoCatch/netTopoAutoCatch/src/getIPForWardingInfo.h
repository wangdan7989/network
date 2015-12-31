#include "my_snmp_bulkwalk.h"
#include <stdio.h>


#define		IP_FORWARDING_INFO			".1.3.6.1.2.1.4.1"



int get_ip_forwarding_info_from_device_by_snmp(const unsigned char *ip, 
											   const unsigned char *comm,
											   unsigned int *ip_forwarding_info);