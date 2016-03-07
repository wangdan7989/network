
#include "ArpDynCatch.h"

/*  judge IpAddr in database_ArpDyn_TABLE DES=DATABASE */
/*   Resultdata is the return,include the ip,mac,flag, relength is Resultdata length                                                 */
/*--------------------------------------------------------*/
/**
		falg=0:illegal
		flag=1:legal:
				flag=11:ip and mac is legal(ip and mac all in basic_check_pool)
				flag=10:ip is in basic_check_pool,but mac is not
						flag=111:the ip and mac is not in basic and in topo,so it's new one
						flag=110:the ip is is only in topo but mac is not,it's new one
**/
/*--------------------------------------------------------*/
HOST_INFO* Judge_IpAddr_is_Legal(const unsigned char *switchip,unsigned char *comm,
						IP_ArpDyn_TABLE_INFO *src_arpdyn_table_info,
						ArpDynTable_INFO *des_arpdyn_table_info,
						HOST_INFO *Resultdata,unsigned int *relength){
	MYSQL mysql;
	Status i,j;
	i=Database_Connect(&mysql);
	Flag illegal_flag=1;

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
	unsigned int NIBcount;
	unsigned int IBcount;
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
	Resultdata=(HOST_INFO *)malloc(sizeof(HOST_INFO) * MAX_MY_OID_RESULT_LEN);
	memset(Resultdata,0,sizeof(HOST_INFO) * MAX_MY_OID_RESULT_LEN);
	printf("\007[TIME]:%s",asctime(timeinfo));
	Flag flag = Judge_IpAddr_from_Src_And_Des(src_arpdyn_table_info,src_num,
						des_arpdyn_table_info,des_num,legal,NIBase,&IBcount,&NIBcount);

	Copy_Hostinfo_Arry(0,IBcount,legal,Resultdata);
	*relength=(*relength)+(IBcount);
	
	
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
			Copy_Hostinfo_Arry(IBcount,&legcount,legal,Resultdata);
			//new legal join,should change the flag;
			int j=0;
			for(j=(*relength);j<(*relength+*legcount);j++){
				Resultdata[j].flag=Resultdata[j].flag+100;
			}
			*relength=(*relength)+(*legcount);
			unsigned int a;
			unsigned int b;
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

			}
			
			
		}
	
		//if host not in topo_host_node : flag =0(HOST_INFO illegal is not null ),judge the host is illegal
		if(illegal_flag=0){
			Copy_Hostinfo_Arry(IBcount+*legcount,illcount,illegal,Resultdata);
			int j=0;
			for(j=(*relength);j<(*relength+*illcount);j++){
				Resultdata[j].flag=Resultdata[j].flag+100;
			}
			*relength=(*relength)+(*illcount);
		}
	
	}
	
	return Resultdata;
}



/**--------------------------------------------------------------------**/
/**             Judge_IpAddr is illegal from two table src and des     **/
/**
		falg=0:illegal
		flag=1x:legal    flag=11:ip and mac is legal
				 flag=10:ip is legal and mac is illegal    
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
							legal[legcon].flag=11;
							legcon++;				
							flag=1;	
					
					
				}
				else {	
					legal[legcon].index = desindex;
					legal[legcon].ArpDynIpAdd = src_arpdyn_table_info[srcindex].ArpDynIpAdd;
					legal[legcon].ArpDynMacAdd = src_arpdyn_table_info[srcindex].ArpDynMacAdd;
					legal[legcon].flag=10;
					legcon++;
					flag=1;						
				}						
			}
		}
		if(!flag){
			illegal[illcon].index = desindex;
			illegal[illcon].ArpDynIpAdd = src_arpdyn_table_info[srcindex].ArpDynIpAdd;
			illegal[illcon].ArpDynMacAdd = src_arpdyn_table_info[srcindex].ArpDynMacAdd;
			illegal[illcon].flag = 0;
			illcon++;
			
		}		
	}
	*illcount=illcon;
	*legcount=legcon;
	return flag;
			
}

/**--------------------------------------------------------------------**/
/**             copy HOST_INFO src to des,and length is src length,index is the des first     **/
/**--------------------------------------------------------------------**/
void Copy_Hostinfo_Arry(int index,int length,HOST_INFO *src,HOST_INFO *des){
	int m=index,n=0;
	
	for(n=0;n<length;n++){
		des[m].ArpDynIpAdd=src[n].ArpDynIpAdd;
		des[m].ArpDynMacAdd=src[n].ArpDynMacAdd;
		des[m].flag=src[n].flag;
		m++;
	}
	
}

int main(int argc,char *argv[]){

	char *ip = SWITCHIPADDR0 ;
	char *comm = PASSWORD;
	unsigned int relength=0;
	IP_ArpDyn_TABLE_INFO *src_arpdyn_table_info=NULL;
	ArpDynTable_INFO *des_arpdyn_table_info=NULL;
	HOST_INFO *Resultdata=NULL;
	int i=0;

	Resultdata=Judge_IpAddr_is_Legal(ip,comm,src_arpdyn_table_info,des_arpdyn_table_info,Resultdata,&relength);
	
	for(i=0;i<relength;i++){
		printf("ArpDynIpAdd:%s   ArpDynMacAdd:%s  flag:%d   \n",Resultdata[i].ArpDynIpAdd,Resultdata[i].ArpDynMacAdd,Resultdata[i].flag);
	}

	
	
}
