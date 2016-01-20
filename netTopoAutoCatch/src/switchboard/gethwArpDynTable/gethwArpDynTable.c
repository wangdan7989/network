#include "gethwArpDynTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int get_ArpDyn_table_info_from_device_by_snmp(const unsigned char *ip, 
											   const unsigned char *comm,
											   IP_ArpDyn_TABLE_INFO *ip_arpdyn_table_info,
											   unsigned int *ip_arpdyn_table_rows) {
	if(ip == NULL || comm == NULL || ip_arpdyn_table_info == NULL || ip_arpdyn_table_rows == NULL) {
		printf("[ERROR] the arguments is invalid\n");
		return 1;
	}
	
	unsigned int count = 0;
	unsigned int ip_count = 0;
	
	
	unsigned int oid_results_nums = 0;
	unsigned int ip_arpdyn_table_nums = 0;
	unsigned char tmp[8];
	unsigned char str[32];
	unsigned char mtmp[12];
	unsigned char mstr[40];
	*ip_arpdyn_table_rows = 0;
	my_oid_result *oid_results = NULL;
	oid_results = (struct my_oid_result *)malloc(sizeof(struct my_oid_result) * MAX_MY_OID_RESULT_LEN);
	memset(oid_results, 0, sizeof(struct my_oid_result) * MAX_MY_OID_RESULT_LEN);
	

	/*get the ArpDynIfIndex,ArpDynVrf,ip_addr */
	if(my_snmp_bulkwalk(ip,comm,Arp_Dyn_MacAdd,oid_results,&oid_results_nums)==0){
		ip_arpdyn_table_nums=-1;

		for(count=0; count < oid_results_nums; count++) {
			ip_arpdyn_table_nums++;
			if(oid_results[count].type == SNMP_NOSUCHOBJECT) {
				printf("[main] No Such Object available on this agent at this OID\n");
			}
	
			if(oid_results[count].type == SNMP_NOSUCHINSTANCE) {
				printf("[main] No Such Instance currently exists at this OID\n");
			}
	/*get the ArpDynIfIdex */
			
			unsigned long indexip=0;
			unsigned int numip=0;
			numip=oid_results[count].oid_name_length-7;
			indexip=oid_results[count].oid_name[numip];
			ip_arpdyn_table_info[ip_arpdyn_table_nums].ArpDynIfIndex=indexip;
				
			
	/*get the ArpDynVrf */		
			unsigned long indexvrf=0;
			unsigned int numvrf=0;
			numvrf=oid_results[count].oid_name_length-2;
			indexvrf=oid_results[count].oid_name[numvrf];
			ip_arpdyn_table_info[ip_arpdyn_table_nums].ArpDynVrf=indexvrf;
		
			
			
			
	/*get the ip_addr */
			unsigned int oid_count_len;
			unsigned int ip_count;

			unsigned len;
			len=oid_results[count].oid_name_length;
			memset(str, 0, 32);					
			memset(tmp, 0, 8);
			for(ip_count= len-6; ip_count < len-2; ip_count++){
				snprintf(tmp, 8, "%d", oid_results[count].oid_name[ip_count]);
				strncat(str, tmp, 32);
				if(ip_count < len-3) {
					strncat(str, ".", 32);
				}				
			}
			strncpy(ip_arpdyn_table_info[ip_arpdyn_table_nums].ArpDynIpAdd, str, 15);		
		}
	}
	
	/* get the MAC addr table entity ip */
	if(my_snmp_bulkwalk(ip, comm, Arp_Dyn_MacAdd, oid_results, &oid_results_nums) == 0) {	
		ip_arpdyn_table_nums = 0;
		unsigned int cou=0;
		
		for(count = 0; count < oid_results_nums; count++) {
			memset(mstr, 0, 40);					
			memset(tmp, 0, 8);

			if(oid_results[count].type == ASN_OCTET_STR || oid_results[count].type == ASN_BIT_STR) {
				int hex = 0, x;
				char *cp;
				for (cp = oid_results[count].val.string, x = 0; x < (int)oid_results[count].val_len; x++, cp++) {
					if (!isprint(*cp) && !isspace(*cp)) {
						hex = 1;
					}
				}		
				if(!hex) {					
					printf("%s\n", oid_results[count].val.string);
				} else {
					for(cou=0; cou<(int)oid_results[count].val_len; cou++) {
						memset(tmp, 0, 8);
						snprintf(tmp, 8, "%x", oid_results[count].val.string[cou]);
						strncat(mstr, tmp, 32);
						if(cou < 5) {
							strncat(mstr, ":", 32);
						}
					}
					strncpy(ip_arpdyn_table_info[ip_arpdyn_table_nums++].ArpDynMacAdd, mstr, 19);
				}
			}
			
		}
	}
	else {
		printf("[ERROR] call my_snmp_bulkwalk function return is wrong for get IP address table entity IP\n");
		free(oid_results);
		return 1;
	}
	
	/* get the Arp_Dyn_VlanId   */
	if(my_snmp_bulkwalk(ip, comm, Arp_Dyn_VlanId, oid_results, &oid_results_nums) == 0) {
		ip_arpdyn_table_nums = 0;
		for(count = 0; count < oid_results_nums; count++) {	
			if(oid_results[count].type == ASN_INTEGER) {
				ip_arpdyn_table_info[ip_arpdyn_table_nums++].ArpDynVlanId = oid_results[count].val.integer;
			} else {
				printf("[ERROR] call my_snmp_bulkwalk function to get IP address table if index type is error\n");
				free(oid_results);
				return 1;
			}
		}
	} else {
		printf("[ERROR] call my_snmp_bulkwalk function return is wrong for get IP address table if index\n");
		free(oid_results);
		return 1;
	}


	/* get the Arp_Dyn_OutIfIndex   */
	if(my_snmp_bulkwalk(ip, comm, Arp_Dyn_OutIfIndex, oid_results, &oid_results_nums) == 0) {
		ip_arpdyn_table_nums = 0;
		for(count = 0; count < oid_results_nums; count++) {	
			if(oid_results[count].type == ASN_INTEGER) {
				ip_arpdyn_table_info[ip_arpdyn_table_nums++].ArpDynOutIfIndex = oid_results[count].val.integer;
			} else {
				printf("[ERROR] call my_snmp_bulkwalk function to get IP address table if index type is error\n");
				free(oid_results);
				return 1;
			}
		}
	} else {
		printf("[ERROR] call my_snmp_bulkwalk function return is wrong for get IP address table if index\n");
		free(oid_results);
		return 1;
	}


/* get the Arp_Dyn_ExpireTime   */
	if(my_snmp_bulkwalk(ip, comm, Arp_Dyn_ExpireTime, oid_results, &oid_results_nums) == 0) {
		ip_arpdyn_table_nums = 0;
		for(count = 0; count < oid_results_nums; count++) {	
			if(oid_results[count].type == ASN_INTEGER) {
				ip_arpdyn_table_info[ip_arpdyn_table_nums++].ArpDynExpireTime = oid_results[count].val.integer;
			} else {
				printf("[ERROR] call my_snmp_bulkwalk function to get IP address table if index type is error\n");
				free(oid_results);
				return 1;
			}
		}
	} else {
		printf("[ERROR] call my_snmp_bulkwalk function return is wrong for get IP address table if index\n");
		free(oid_results);
		return 1;
	}


	/* get the IP route table mask */
/*	if(my_snmp_bulkwalk(ip, comm, IP_ADDR_TABLE_ENT_MASK, oid_results, &oid_results_nums) == 0) {
		ip_arpdyn_table_nums = 0;
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
				strncpy(ip_arpdyn_table_info[ip_arpdyn_table_nums++].ipAddrTableEntMask, str, 15);
			} else {
				printf("[ERROR] call my_snmp_bulkwalk function to get IP address table mask type is error\n");
				free(oid_results);
				return 1;
			}
		}
	} else {
		printf("[ERROR] call my_snmp_bulkwalk function return is wrong for get ip address table mask\n");
		free(oid_results);
		return 1;
	}
	
*/	*ip_arpdyn_table_rows = ip_arpdyn_table_nums;
	
	free(oid_results);
	return 0;
}

/*
int main(int argc, char *argv[]) {
	
	
	IP_ArpDyn_TABLE_INFO *ip_arpdyn_table_info = NULL;
	unsigned int num = 0;
	unsigned int count = 0;

	ip_arpdyn_table_info = (IP_ArpDyn_TABLE_INFO *)malloc(sizeof(IP_ArpDyn_TABLE_INFO) * MAX_MY_OID_RESULT_LEN);
	memset(ip_arpdyn_table_info, 0, sizeof(IP_ArpDyn_TABLE_INFO) * MAX_MY_OID_RESULT_LEN);
	char *ip = "192.168.200.253";
	char *comm = "P@ssw0rd";
	
	unsigned int ret=get_ArpDyn_table_info_from_device_by_snmp(ip,comm,ip_arpdyn_table_info, &num);
	printf("ret:%d\n",ret);
	printf("num:%d\n",num);
	if(ret != 0) {
		printf("[ERROR] call get_ip_addr_table_info_from_device_by_snmp error\n");
		return 1;
	}

	printf("IfIndex        Vrf        VlanId          OutIfIndex     ExpireTime            IpAdd                    MacAdd\n");
	
	for(count = 0; count < num; count++) {
		//printf("%-16s\n", ip_arpdyn_table_info[count].ipAddrTableEntMask);
		printf("%-16ld%-16ld%-16ld%-16ld%-16ld%-16s%-16s\n", ip_arpdyn_table_info[count].hwArpDynIfIndex,
					ip_arpdyn_table_info[count].hwArpDynVrf,
					ip_arpdyn_table_info[count].hwArpDynVlanId,
					ip_arpdyn_table_info[count].hwArpDynOutIfIndex,
					ip_arpdyn_table_info[count].hwArpDynExpireTime,
					ip_arpdyn_table_info[count].hwArpDynIpAdd,
					ip_arpdyn_table_info[count].hwArpDynMacAdd
					);
						
									
	}
	

}
*/

