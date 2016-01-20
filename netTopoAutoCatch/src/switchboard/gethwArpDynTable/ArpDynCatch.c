
#include "ArpDynCatch.h"

/*  judge IpAddr in database_ArpDyn_TABLE DES=DATABASE*/
int Judge_IpAddr_is_Legal(const unsigned char *switchip,unsigned char *comm,
						IP_ArpDyn_TABLE_INFO *src_arpdyn_table_info,
						ArpDynTable_INFO *des_arpdyn_table_info){
	MYSQL mysql;
	Status i,j;
	i=Database_Connect(&mysql);
	

//get switchId from database SwitchInfoTable by SwitchIAddr	
	SWITCH_INFO switch_info[64];
	int switch_info_nums=0;
	Status get_switch_info=Get_Switch_Info(&mysql,switch_info,&switch_info_nums,switchip);
	SwitchId switchid=switch_info[0].SwitchId;

//judge host_ip in Basic_Check_Pool
	unsigned int src_num = 0;
	unsigned int des_num = 0;
	unsigned int *NIBcount;
	NIBcount=0;

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
	int flag = Judge_IpAddr_from_Src_And_Des(src_arpdyn_table_info,src_num,
						des_arpdyn_table_info,des_num,legal,NIBase,&NIBcount);
	
	printf("flag:%d  NIBcount:%d\n",flag,NIBcount);
	
	
//if host is not in Basic_Check_Pool,then search in topo_host_node
	if(flag==0){
		src_num = NIBcount;
		des_num = 0;
		unsigned int *illcount;
		illcount=0;

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
		int flag = Judge_IpAddr_from_Src_And_Des(src_arpdyn_table_info,src_num,des_arpdyn_table_info,
							des_num,legal,illegal,&illcount);

		printf("illegal:%d\n",flag);

	}
//if host in topo_host_node: flag=1(HOST_INFO legal is not null)
	if(){
	
	}
	
//if host not in topo_host_node : flag =0(HOST_INFO illegal is not null )
	if(flag=0){
	
	}
	

}



/**--------------------------------------------------------------------**/
/**             Judge_IpAddr is illegal from two table src and des     **/
/**
		falg=0:illegal
		flag=1:legal
**/
/**--------------------------------------------------------------------**/

int Judge_IpAddr_from_Src_And_Des(IP_ArpDyn_TABLE_INFO *src_arpdyn_table_info,unsigned int *src_num,
						ArpDynTable_INFO *des_arpdyn_table_info,unsigned int *des_num,
							HOST_INFO *legal,HOST_INFO *illegal,unsigned int *illcount){
	unsigned int srcindex = 0;
	unsigned int desindex = 0;
	unsigned int flag=0;
	unsigned int legcon=0;
	unsigned int illcon=0;

	for(srcindex= 0; srcindex < src_num; srcindex++){

		for(desindex = 0; desindex < des_num; desindex++){
			if(strcmp(src_arpdyn_table_info[srcindex].ArpDynIpAdd,des_arpdyn_table_info[desindex].ArpDynIpAdd)==0){	
				if(strcmp(src_arpdyn_table_info[srcindex].ArpDynMacAdd,
							des_arpdyn_table_info[desindex].ArpDynMacAdd)==0){

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
			illegal[illcon].ArpDynIpAdd = src_arpdyn_table_info[srcindex].ArpDynIpAdd;
			illegal[illcon].ArpDynMacAdd = src_arpdyn_table_info[srcindex].ArpDynMacAdd;
			illcon++;
			printf("ip:%s is illegal!\n",src_arpdyn_table_info[srcindex].ArpDynIpAdd);
		}		
	}
	*illcount=illcon;
	return flag;
			
}



int main(int argc,char *argv[]){

/*	MYSQL mysql;
	Status i,j;

	i=Database_Connect(&mysql);
	SWITCH_INFO switch_info[64];
	int switch_info_nums=0;
//	Status arpdyn_info_insert=ArpDyn_Info_Table_Insert(&mysql,2,1,"192.168.200.2",2,"ae:3c:8b:4a:5a",2,6,23);
/*	Status switch_info_insert=Switch_Info_Table_Insert(&mysql,0,SWITCHIPADDR0 );
	switch_info_insert=Switch_Info_Table_Insert(&mysql,1,SWITCHIPADDR1 );
	switch_info_insert=Switch_Info_Table_Insert(&mysql,2,SWITCHIPADDR2 );
	switch_info_insert=Switch_Info_Table_Insert(&mysql,3,SWITCHIPADDR3 );
	printf("status:%d\n",switch_info_insert);*/

//	Status get_switch_info=Get_Switch_Info(&mysql,switch_info,&switch_info_nums,1);
//	printf("%d\n%s\n",switch_info[0].SwitchId,switch_info[0].IpAddr);
//        Status delete_switch_table_by_switchId=Delete_Switch_Table_By_SwitchId(&mysql,1);
//	printf("status:%d\n",delete_switch_table_by_switchId);
/*
	ArpDynTable_INFO arpdynTable_info[90];
	int arpdynTable_info_nums=0; 
	Status arpdyn_info_insert=ArpDyn_Info_Table_Insert(&mysql,1,1,"192.168.200.2",2,"ae:3c:8b:4a:5a",2,6,23);
	printf("insert status:%d\n",arpdyn_info_insert);
	Status get_arpdyn_info=Get_ArpDynTable_Info(&mysql,arpdynTable_info,&arpdynTable_info_nums,1);	
	printf("get status:%d\n",get_arpdyn_info);
	printf("get arpdynTable_info_nums:%d\n",arpdynTable_info_nums);
	int k=0;
	for(k = 0; k<arpdynTable_info_nums; k++){
		printf("%d,%d,%s,%d,%s,%d,%d,%d\n",arpdynTable_info[k].SwitchId,arpdynTable_info[k].ArpDynIfIndex,arpdynTable_info[k].ArpDynIpAdd,arpdynTable_info[k].ArpDynVrf,arpdynTable_info[k].ArpDynMacAdd,arpdynTable_info[k].ArpDynVlanId,arpdynTable_info[k].ArpDynOutIfIndex,arpdynTable_info[k].ArpDynExpireTime);
	}

//SwitchId,ArpDynIfIndex,ArpDynIpAdd,ArpDynVrf,ArpDynMacAdd,ArpDynVlanId,ArpDynOutIfInd,ArpDynExpireTime
//	Status delete_arpdyn_table_info=Delete_ArpDyn_Table_By_SwitchId(&mysql,1);
//	printf("status:%d\n",delete_arpdyn_table_info);
//	Status update_router_info=Update_Router_info_int(&mysql, 1 , "RouterID",9) ;
//	Status update_switch_char=Update_Switch_info_char(&mysql, 2 , "IpAddr","199.192.100.100");
//	printf("status:%d\n",update_switch_char);*/
//	j=Database_DisConnect(&mysql);
//	return 	0;	







/*******************************************************************************************/	




/*******************************************************************************************/	

/*
	IP_ArpDyn_TABLE_INFO *ip_arpdyn_table_info = NULL;
	unsigned int num = 0;
	unsigned int count = 0;

	ip_arpdyn_table_info = (IP_ArpDyn_TABLE_INFO *)malloc(sizeof(IP_ArpDyn_TABLE_INFO) * MAX_MY_OID_RESULT_LEN);
	memset(ip_arpdyn_table_info, 0, sizeof(IP_ArpDyn_TABLE_INFO) * MAX_MY_OID_RESULT_LEN);
	//char *ip = SWITCHIPADDR0 ;
	//char *ip = SWITCHIPADDR1 ;
	//char *ip = SWITCHIPADDR2 ;
	char *ip = SWITCHIPADDR3 ;
	char *comm = PASSWORD;
	
	unsigned int ret=get_ArpDyn_table_info_from_device_by_snmp(ip,comm,ip_arpdyn_table_info, &num);
	printf("ret:%d\n",ret);
	printf("num:%d\n",num);
	if(ret != 0) {
		printf("[ERROR] call get_ip_addr_table_info_from_device_by_snmp error\n");
		return 1;
	}

	printf("IfIndex        Vrf        VlanId          OutIfIndex     ExpireTime            IpAdd                    MacAdd\n");
	Status arpdyn_info_insert;
	
	for(count = 0; count < num; count++) {
		
		arpdyn_info_insert=ArpDyn_Info_Table_Insert(&mysql,SWITCH_ID3,
									ip_arpdyn_table_info[count].hwArpDynIfIndex,
									ip_arpdyn_table_info[count].hwArpDynIpAdd,
									ip_arpdyn_table_info[count].hwArpDynVrf,
									ip_arpdyn_table_info[count].hwArpDynMacAdd,
									ip_arpdyn_table_info[count].hwArpDynVlanId,
									ip_arpdyn_table_info[count].hwArpDynOutIfIndex,
									ip_arpdyn_table_info[count].hwArpDynExpireTime);
		
		printf("%-16ld%-16ld%-16ld%-16ld%-16ld%-16s%-16s\n", ip_arpdyn_table_info[count].hwArpDynIfIndex,
					ip_arpdyn_table_info[count].hwArpDynVrf,
					ip_arpdyn_table_info[count].hwArpDynVlanId,
					ip_arpdyn_table_info[count].hwArpDynOutIfIndex,
					ip_arpdyn_table_info[count].hwArpDynExpireTime,
					ip_arpdyn_table_info[count].hwArpDynIpAdd,
					ip_arpdyn_table_info[count].hwArpDynMacAdd
					);

						
									
	}
	printf("status:%d\n",arpdyn_info_insert);

	j=Database_DisConnect(&mysql);
	return 	0;

*/

	char *ip = SWITCHIPADDR0 ;
	char *comm = PASSWORD;
	IP_ArpDyn_TABLE_INFO *src_arpdyn_table_info=NULL;
	ArpDynTable_INFO *des_arpdyn_table_info=NULL;

	int result=Judge_IpAddr_is_Legal(ip,comm,src_arpdyn_table_info,des_arpdyn_table_info);
	printf("result:%d\n",result);


/*******test*****/
/*

	char *str;
	char list[20];
	
	str="loh";
	strncpy(list,str,3);
	printf("str:%s\n",str);
	printf("list:%s\n",list);
*/
}
