#include "getIPRouteTable.h"



int get_ip_route_table_info_from_device_by_snmp(const unsigned char *ip, 
												const unsigned char *comm,
												IP_ROUTE_TABLE_INFO *ip_route_table_info,
												unsigned int *ip_route_table_rows) {
	if(ip == NULL || comm == NULL || ip_route_table_info == NULL || ip_route_table_rows == NULL) {
		printf("[ERROR] the arguments is invalid\n");
		return 1;
	}
	
	unsigned int count = 0;
	unsigned int ip_count = 0;
	unsigned int oid_results_nums = 0;
	unsigned int ip_route_table_nums = 0;
	unsigned char tmp[8];
	unsigned char str[32];
	*ip_route_table_rows = 0;
	my_oid_result *oid_results = NULL;
	oid_results = (struct my_oid_result *)malloc(sizeof(struct my_oid_result) * MAX_MY_OID_RESULT_LEN);
	
	memset(oid_results, 0, sizeof(struct my_oid_result) * MAX_MY_OID_RESULT_LEN);

	
	/* get the IP route table destination */
	if(my_snmp_bulkwalk(ip, comm, IP_ROUTE_TABLE_DEST, oid_results, &oid_results_nums) == 0) {
		ip_route_table_nums = 0;
		for(count = 0; count < oid_results_nums; count++) {
			memset(str, 0, 32);
			if(oid_results[count].type == ASN_IPADDRESS) {
				for(ip_count = 0; ip_count < 4; ip_count++) {
					memset(tmp, 0, 8);
					snprintf(tmp, 8, "%d", oid_results[count].val.string[ip_count]);
					strncat(str, tmp, 32);
					if(ip_count < 3) {
						strncat(str, ".", 32);
					}
				}
				strncpy(ip_route_table_info[ip_route_table_nums++].ipRouteTableDest, str, 15);
			} else {
				printf("[ERROR] call my_snmp_bulkwalk function to get IP route table dest type is error\n");
				free(oid_results);
				return 1;
			}
		}
	} else {
		printf("[ERROR] call my_snmp_bulkwalk function return is wrong for get IP route table desc\n");
		free(oid_results);
		return 1;
	}

	/* get the IP route table if index */
	if(my_snmp_bulkwalk(ip, comm, IP_ROUTE_TABLE_IF_INDEX, oid_results, &oid_results_nums) == 0) {
		ip_route_table_nums = 0;
		for(count = 0; count < oid_results_nums; count++) {	
			if(oid_results[count].type == ASN_INTEGER) {
				ip_route_table_info[ip_route_table_nums++].ipRouteTableIfIndex = oid_results[count].val.integer;
			} else {
				printf("[ERROR] call my_snmp_bulkwalk function to get IP route table if index type is error\n");
				free(oid_results);
				return 1;
			}
		}
	} else {
		printf("[ERROR] call my_snmp_bulkwalk function return is wrong for get IP route table if index\n");
		free(oid_results);
		return 1;
	}
	
	/* get the IP route table destination */
	if(my_snmp_bulkwalk(ip, comm, IP_ROUTE_TABLE_NEXT_HOP, oid_results, &oid_results_nums) == 0) {
		ip_route_table_nums = 0;
		for(count = 0; count < oid_results_nums; count++) {
			memset(str, 0, 32);
			if(oid_results[count].type == ASN_IPADDRESS) {
				for(ip_count = 0; ip_count < 4; ip_count++) {
					memset(tmp, 0, 8);
					snprintf(tmp, 8, "%d", oid_results[count].val.string[ip_count]);
					strncat(str, tmp, 32);
					if(ip_count < 3) {
						strncat(str, ".", 32);
					}
				}
				strncpy(ip_route_table_info[ip_route_table_nums++].ipRouteTableNextHop, str, 15);
			} else {
				printf("[ERROR] call my_snmp_bulkwalk function to get IP route table next hop type is error\n");
				free(oid_results);
				return 1;
			}
		}
	} else {
		printf("[ERROR] call my_snmp_bulkwalk function return is wrong for get IP route table next hop\n");
		free(oid_results);
		return 1;
	}	
	
	/* get the IP route table type */
	if(my_snmp_bulkwalk(ip, comm, IP_ROUTE_TABLE_TYPE, oid_results, &oid_results_nums) == 0) {
		ip_route_table_nums = 0;
		for(count = 0; count < oid_results_nums; count++) {	
			if(oid_results[count].type == ASN_INTEGER) {
				ip_route_table_info[ip_route_table_nums++].ipRouteTableType = oid_results[count].val.integer;
			} else {
				printf("[ERROR] call my_snmp_bulkwalk function to get IP route table if type is error\n");
				free(oid_results);
				return 1;
			}
		}
	} else {
		printf("[ERROR] call my_snmp_bulkwalk function return is wrong for get IP route table if type\n");
		free(oid_results);
		return 1;
	}

	/* get the IP route table mask */
	if(my_snmp_bulkwalk(ip, comm, IP_ROUTE_TABLE_MASK, oid_results, &oid_results_nums) == 0) {
		ip_route_table_nums = 0;
		for(count = 0; count < oid_results_nums; count++) {
			memset(str, 0, 32);
			if(oid_results[count].type == ASN_IPADDRESS) {
				for(ip_count = 0; ip_count < 4; ip_count++) {
					memset(tmp, 0, 8);
					snprintf(tmp, 8, "%d", oid_results[count].val.string[ip_count]);
					strncat(str, tmp, 32);
					if(ip_count < 3) {
						strncat(str, ".", 32);
					}
				}
				strncpy(ip_route_table_info[ip_route_table_nums++].ipRouteTableMask, str, 15);
			} else {
				printf("[ERROR] call my_snmp_bulkwalk function to get IP route table mask type is error\n");
				free(oid_results);
				return 1;
			}
		}
	} else {
		printf("[ERROR] call my_snmp_bulkwalk function return is wrong for get ip route table mask\n");
		free(oid_results);
		return 1;
	}
	
	*ip_route_table_rows = ip_route_table_nums;
	
	free(oid_results);
	return 0;
}

/*
int main(int argc, char *argv[]) {
	
	if(argc < 3) {
		printf("please input the ip and community\n");
		return 1;
	}
	
	IP_ROUTE_TABLE_INFO *ip_route_table_info = NULL;
	unsigned int num = 0;
	unsigned int count = 0;
	ip_route_table_info = (IP_ROUTE_TABLE_INFO *)malloc(sizeof(IP_ROUTE_TABLE_INFO) * MAX_MY_OID_RESULT_LEN);
	memset(ip_route_table_info, 0, sizeof(IP_ROUTE_TABLE_INFO) * MAX_MY_OID_RESULT_LEN);
	
	unsigned int ret = get_ip_route_table_info_from_device_by_snmp(argv[1], argv[2], ip_route_table_info, &num);
	
	if(ret != 0) {
		printf("[ERROR] call get_ip_route_table_info_from_device_by_snmp error\n");
		return 1;
	}

	printf("dest            nexthop         mask            ifindex type\n");
	
	for(count = 0; count < num; count++) {
		printf("%-16s%-16s%-16s%-8d%-8d\n", ip_route_table_info[count].ipRouteTableDest,
															 ip_route_table_info[count].ipRouteTableNextHop,
															 ip_route_table_info[count].ipRouteTableMask,
															 ip_route_table_info[count].ipRouteTableIfIndex,
															 ip_route_table_info[count].ipRouteTableType);
	}
	
	return 0;
}
*/