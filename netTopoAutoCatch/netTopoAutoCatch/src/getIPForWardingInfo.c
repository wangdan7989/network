#include "getIPForWardingInfo.h"



int get_ip_forwarding_info_from_device_by_snmp(const unsigned char *ip, 
											   const unsigned char *comm,
											   unsigned int *ip_forwarding_info) {
	if(ip == NULL || comm == NULL || ip_forwarding_info == NULL) {
		printf("[ERROR] the arguments is invalid\n");
		return 1;
	}
	
	*ip_forwarding_info = 0;
	unsigned int oid_results_nums = 0;
	my_oid_result oid_results;

	/* get the IP forwarding info */
	if(my_snmp_bulkwalk(ip, comm, IP_FORWARDING_INFO, &oid_results, &oid_results_nums) == 0) {
		if(oid_results.type == ASN_INTEGER) {
			*ip_forwarding_info = oid_results.val.integer;
		} else {
			printf("[ERROR] call my_snmp_bulkwalk function to get IP forwarding is error\n");
			return 1;
		}
	} else {
		printf("[ERROR] call my_snmp_bulkwalk function return is wrong for get IP forwarding\n");
		return 1;
	}

	return 0;
}

/*
int main(int argc, char *argv[]) {
	
	if(argc < 3) {
		printf("please input the ip and community\n");
		return 1;
	}
	
	unsigned int ipForWarding = 0;
	unsigned int ret = get_ip_forwarding_info_from_device_by_snmp(argv[1], argv[2], &ipForWarding);
	
	if(ret != 0) {
		printf("[ERROR] call get_ip_forwarding_info_from_device_by_snmp error\n");
		return 1;
	}

	printf("ipForWarding: %d\n", ipForWarding);
	return 0;
}
*/
