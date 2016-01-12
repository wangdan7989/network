#include "getSwitchName.h"



int get_ip_switch_name_from_device_by_snmp(const unsigned char *ip, 
										  const unsigned char *comm,
										  unsigned char *ip_switch_name) {
	if(ip == NULL || comm == NULL || ip_switch_name == NULL) {
		printf("[ERROR] the arguments is invalid\n");
		return 1;
	}
	
	unsigned int oid_results_nums = 0;
	my_oid_result oid_results;

	/* get the ip switch name info */
	if(my_snmp_bulkwalk(ip, comm, IP_SWITCH_NAME_INFO, &oid_results, &oid_results_nums) == 0) {
		if(oid_results.type == ASN_OCTET_STR) {
			strncpy(ip_switch_name, oid_results.val.string, 64);
		} else {
			printf("[ERROR] call my_snmp_bulkwalk function to get IP switch name is error\n");
			return 1;
		}
	} else {
		printf("[ERROR] call my_snmp_bulkwalk function return is wrong for get IP switch name\n");
		return 1;
	}

	return 0;
}



int main(int argc, char *argv[]) {
	
	if(argc < 3) {
		printf("please input the ip and community\n");
		return 1;
	}
	
	unsigned char switch_name[64];
	unsigned int ret = get_ip_switch_name_from_device_by_snmp(argv[1], argv[2], switch_name);
	
	if(ret != 0) {
		printf("[ERROR] call get_ip_switch_name_from_device_by_snmp error\n");
		return 1;
	}

	printf("switch_name: %s\n", switch_name);
	return 0;
}

