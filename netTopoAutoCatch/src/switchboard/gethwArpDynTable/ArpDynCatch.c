
#include "ArpDynCatch.h"

/*  judge IpAddr in database_ArpDyn_TABLE DES=DATABASE*/
/*--------------------------------------------------------*/
/**
		falg=0:illegal
		flag=1:legal
**/
/*--------------------------------------------------------*/
Flag Judge_IpAddr_is_Legal(const unsigned char *switchip,unsigned char *comm,
						IP_ArpDyn_TABLE_INFO *src_arpdyn_table_info,
						ArpDynTable_INFO *des_arpdyn_table_info){
	MYSQL mysql;
	Status i,j;
	i=Database_Connect(&mysql);
	Flag illegal_flag;

//print on the Blog the time
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo=localtime(&rawtime);

//get switchId from database SwitchInfoTable by SwitchIAddr	
	SWITCH_INFO switch_info[64];
	int switch_info_nums=0;
	Status get_switch_info=Get_Switch_Info(&mysql,switch_info,&switch_info_nums,switchip);
	SwitchId switchid=switch_info[0].SwitchId;

//judge host_ip in Basic_Check_Pool
	unsigned int src_num = 0;
	unsigned int des_num = 0;
	unsigned int *NIBcount;
	unsigned int *IBcount;
	IBcount = 0;
	NIBcount = 0;

	src_arpdyn_table_info = (IP_ArpDyn_TABLE_INFO *)malloc(sizeof(IP_ArpDyn_TABLE_INFO) * MAX_MY_OID_RESULT_LEN);
	memset(src_arpdyn_table_info, 0, sizeof(IP_ArpDyn_TABLE_INFO) * MAX_MY_OID_RESULT_LEN);
	unsigned int ret=get_ArpDyn_table_info_from_device_by_snmp(switchip,comm,src_arpdyn_table_info, &src_num);

	des_arpdyn_table_info = (ArpDynTable_INFO *)malloc(sizeof(ArpDynTable_INFO) * MAX_MY_OID_RESULT_LEN);
	memset(des_arpdyn_table_info, 0, sizeof(ArpDynTable_INFO) * MAX_MY_OID_RESULT_LEN);
	Status get_arpdyn_info=Get_Basic_Check_Pool_Info(&mysql,des_arpdyn_table_info,&des_num,switchid);	

		
	HOST_INFO *legal=NULL;
	legal=(HOST_INFO *)malloc(sizeof(HOST_INFO) * MAX_MY_OID_RESULT_LEN);
	memset(legal,0,sizeof(HOST_INFO) * MAX_MY_OID_RESULT_LEN);
	HOST_INFO *NIBase=NULL;
	NIBase=(HOST_INFO *)malloc(sizeof(HOST_INFO) * MAX_MY_OID_RESULT_LEN);
	memset(NIBase,0,sizeof(HOST_INFO) * MAX_MY_OID_RESULT_LEN);
	Flag flag = Judge_IpAddr_from_Src_And_Des(src_arpdyn_table_info,src_num,
						des_arpdyn_table_info,des_num,legal,NIBase,&IBcount,&NIBcount);
	
	
	
//if host is not in Basic_Check_Pool,then search in topo_host_node
	if(flag==0){
		src_num = NIBcount;
		des_num = 0;
		unsigned int *illcount;
		unsigned int *legcount;
		illcount=0;
		legcount=0;

		int p=0;
		for(p = 0; p < NIBcount; p++){
			strncpy(src_arpdyn_table_info[p].ArpDynIpAdd, NIBase[p].ArpDynIpAdd, 15);
			strncpy(src_arpdyn_table_info[p].ArpDynMacAdd, NIBase[p].ArpDynMacAdd, 19);
		}
					

		memset(des_arpdyn_table_info, 0, sizeof(ArpDynTable_INFO) * MAX_MY_OID_RESULT_LEN);
		get_arpdyn_info=Get_topo_host_node_Info(&mysql,des_arpdyn_table_info,&des_num,switchid);	
		
		memset(legal,0,sizeof(HOST_INFO) * MAX_MY_OID_RESULT_LEN);
		HOST_INFO *illegal=NULL;
		illegal=(HOST_INFO *)malloc(sizeof(HOST_INFO) * MAX_MY_OID_RESULT_LEN);
		memset(illegal,0,sizeof(HOST_INFO) * MAX_MY_OID_RESULT_LEN);
		illegal_flag = Judge_IpAddr_from_Src_And_Des(src_arpdyn_table_info,src_num,des_arpdyn_table_info,
							des_num,legal,illegal,&legcount,&illcount);

	
		//if host in topo_host_node: flag=1(HOST_INFO legal is not null),insert the host into Basic_Check_Pool
		// the indx of HOST_info is des_arpdyn_table_info index;
		if(illegal_flag=1){
			unsigned int a;
			unsigned int b;
			printf("\007[TIME]:%s\n",asctime(timeinfo));
			for(a = 0; a < legcount;a++){
				b = legal[a].index;
				//insert into Basic_Check_Pool
				Status Basic_Check_Pool_Info_Table=Basic_Check_Pool_Info_Table_Insert(&mysql,switchid,
						des_arpdyn_table_info[b].ArpDynIfIndex,
						des_arpdyn_table_info[b].ArpDynIpAdd,
						des_arpdyn_table_info[b].ArpDynVrf,
						des_arpdyn_table_info[b].ArpDynMacAdd,
						des_arpdyn_table_info[b].ArpDynVlanId,
						des_arpdyn_table_info[b].ArpDynOutIfIndex,
						des_arpdyn_table_info[b].ArpDynExpireTime);
				
				printf("NEW NET ELEMENT JOIN! [IPADDR]:%s    [MACADDR]:%s\n",
					des_arpdyn_table_info[b].ArpDynIpAdd,
					des_arpdyn_table_info[b].ArpDynMacAdd,);
			}
			
			
		}
	
		//if host not in topo_host_node : flag =0(HOST_INFO illegal is not null ),judge the host is illegal
		if(illegal_flag=0){
			int q; 
			printf("\007[TIME]:%s\n",asctime(timeinfo));
			for(q = 0;q < illcount; q++){
				printf("[ILLGEAL]:ILLGEAL NET ELEMENT JOIN! :\n IPAddr:%s  MACAddr:%s\n",
						illegal[q].ArpDynIpAdd,illegal[q].ArpDynMacAdd);
			}
		}
	
	}
	return illegal_flag;
}



/**--------------------------------------------------------------------**/
/**             Judge_IpAddr is illegal from two table src and des     **/
/**
		falg=0:illegal
		flag=1:legal
**/
/**--------------------------------------------------------------------**/

Flag Judge_IpAddr_from_Src_And_Des(IP_ArpDyn_TABLE_INFO *src_arpdyn_table_info,unsigned int *src_num,
						ArpDynTable_INFO *des_arpdyn_table_info,unsigned int *des_num,
							HOST_INFO *legal,HOST_INFO *illegal,unsigned int *legcount,unsigned int *illcount){
	unsigned int srcindex = 0;
	unsigned int desindex = 0;
	Flag flag=0;
	unsigned int legcon=0;
	unsigned int illcon=0;

	for(srcindex= 0; srcindex < src_num; srcindex++){

		for(desindex = 0; desindex < des_num; desindex++){
			if(strcmp(src_arpdyn_table_info[srcindex].ArpDynIpAdd,des_arpdyn_table_info[desindex].ArpDynIpAdd)==0){	
				if(strcmp(src_arpdyn_table_info[srcindex].ArpDynMacAdd,
							des_arpdyn_table_info[desindex].ArpDynMacAdd)==0){
							legal[legcon].index = desindex;
							legal[legcon].ArpDynIpAdd = src_arpdyn_table_info[srcindex].ArpDynIpAdd;
							legal[legcon].ArpDynMacAdd = src_arpdyn_table_info[srcindex].ArpDynMacAdd;
							legcon++;
							
					printf("ip:%s is ok!\n",src_arpdyn_table_info[srcindex].ArpDynIpAdd);
					
					flag=1;	
				}
				else {
					printf("ip:%s 's macaddr is conflicting!\n",src_arpdyn_table_info[srcindex].ArpDynIpAdd);
					flag=1;						
				}						
			}
		}
		if(!flag){
			illegal[illcon].index = desindex;
			illegal[illcon].ArpDynIpAdd = src_arpdyn_table_info[srcindex].ArpDynIpAdd;
			illegal[illcon].ArpDynMacAdd = src_arpdyn_table_info[srcindex].ArpDynMacAdd;
			illcon++;
			printf("ip:%s is illegal!\n",src_arpdyn_table_info[srcindex].ArpDynIpAdd);
		}		
	}
	*illcount=illcon;
	*legcount=legcon;
	return flag;
			
}



int main(int argc,char *argv[]){
/*
	char *ip = SWITCHIPADDR0 ;
	char *comm = PASSWORD;
	IP_ArpDyn_TABLE_INFO *src_arpdyn_table_info=NULL;
	ArpDynTable_INFO *des_arpdyn_table_info=NULL;

	int result=Judge_IpAddr_is_Legal(ip,comm,src_arpdyn_table_info,des_arpdyn_table_info);
*/
	
	
}
