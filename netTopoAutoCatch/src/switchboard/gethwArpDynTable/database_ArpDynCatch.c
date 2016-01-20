#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include "database_ArpDynCatch.h"


/* Connect the netTopoAutoCatch mysql database */

Status Database_Connect(MYSQL *my_connection)
{
	if(my_connection==NULL)
	{
#ifdef DATABASE_DEBUG_MESSAGE
		printf("[ERROR] database_netTopoAutoCatch: The parameter of the database connect function is invalid.\n");
#endif
		return DATABASE_INFEASIBLE;
	}
	mysql_init(my_connection);

	if(mysql_real_connect(my_connection, DB_IPADDR, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0))
	{
#ifdef DATABASE_DEBUG_MESSAGE
		printf("[DEBUG] database_public: Connection netTopoAutoCatch DataBase Successful.\n");
		printf("[DEBUG] database_public: Waiting the netTopoAutoCatch DataBase Closed...\n");
#endif
		if(mysql_set_character_set(my_connection, "utf8")){ 
			fprintf(stderr ,"error , %s/n" ,mysql_error(my_connection) ) ; 
		}
		return DATABASE_OK;
	}
	else
	{
#ifdef DATABASE_DEBUG_MESSAGE
		fprintf(stderr,"[ERROR] database_public: Connection netTopoAutoCatch DataBase Failure.\n");
#endif
		if(mysql_errno(my_connection))
		{
#ifdef DATABASE_DEBUG_MESSAGE
			fprintf(stderr,"[ERROR] database_public: Connection error %d: %s\n",mysql_errno(my_connection),mysql_error(my_connection));
#endif
		}
		return DATABASE_ERROR;
	}


	if(!mysql_real_connect(my_connection, DB_IPADDR, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0)) {  
	
        printf("Error connecting to Mysql!\n");  
    }else {  
        printf("Connected Mysql successful!\n");  
    }  

}
 


/* DisConnect the NetWork_Management_DataInfo mysql database (New Version)*/
Status Database_DisConnect(MYSQL *my_connection)
{
	if(my_connection==NULL)
	{
#ifdef DATABASE_DEBUG_MESSAGE
		printf("[ERROR] database_netTopoAutoCatch: The parameter of the database disconnect function is invalid.\n");
#endif
		return DATABASE_INFEASIBLE;
	}
	
	mysql_close(my_connection);
	
#ifdef DATABASE_DEBUG_MESSAGE
	printf("[DEBUG] database_netTopoAutoCatch: The DataInfo1 DataBase is Closed.\n");
#endif

	return DATABASE_OK;
}


/* ---------------------------------------------------------- */
/*                Switch   functions                    */
/* ---------------------------------------------------------- */
/* Insert information into SwitchInfoTable*/
Status Switch_Info_Table_Insert(MYSQL *my_connection,SwitchId SwitchId,char *IpAddr){
	int res;
	char sqlbuff[SQLBUFFLEN];
	sprintf(sqlbuff,"insert into SwitchInfoTable(SwitchId,IpAddr) values(%d,'%s')",SwitchId,IpAddr);
	res=mysql_query(my_connection,sqlbuff);
	if(!res){
#ifdef DATABASE_DEBUG_MESSAGE
		printf("[DEBUG] database_ArpDynCatch: Insert %lu rows\n",(unsigned long)mysql_affected_rows(my_connection));
#endif
		return DATABASE_OK;			
	}
	else
	{
#ifdef DATABASE_DEBUG_MESSAGE
		fprintf(stderr,"[ERROR] database_ArpDynCatch: Insert error %d: %s\n",mysql_errno(my_connection),mysql_error(my_connection));
#endif
		return DATABASE_ERROR;
	}
}


/* Functions:  Get SwitchInfo */
Status Get_Switch_Info(MYSQL *my_connection,SWITCH_INFO *switch_info, unsigned int *switch_info_nums, const unsigned char *SwitchIpAddr)
{
	MYSQL_RES *res_ptr;
	MYSQL_ROW sqlrow;
	int count=0;
	int res,j;
	char sqlbuff[SQLBUFFLEN];
	sprintf(sqlbuff,"select SwitchId,IpAddr from SwitchInfoTable where IpAddr='%s'",SwitchIpAddr);
	res=mysql_query(my_connection,sqlbuff);
	if(res)
	{
#ifdef DATABASE_DEBUG_MESSAGE
		fprintf(stderr,"[ERROR] database_public: Select error %d: %s\n",mysql_errno(my_connection),mysql_error(my_connection));
#endif
		return	DATABASE_ERROR;
	}
	else
	{
		res_ptr=mysql_store_result(my_connection);
		if(res_ptr)
		{
#ifdef DATABASE_DEBUG_MESSAGE
			printf("[DEBUG] database_public: Retrieved %lu rows\n",(unsigned long)mysql_num_rows(res_ptr));
#endif
			*switch_info_nums=(unsigned int)mysql_num_rows(res_ptr);


			while((sqlrow=mysql_fetch_row(res_ptr)))
			{
#ifdef DATABASE_DEBUG_MESSAGE
				printf("[DEBUG] database_public: Fetched data...\n");
#endif

				switch_info[count].SwitchId=atoi(sqlrow[0]);
				
				switch_info[count].IpAddr = (char*)malloc(sizeof(char) *16);
				strcpy(switch_info[count].IpAddr,sqlrow[1]);
				
				count++;
				
			}
			if(mysql_errno(my_connection))
			{
#ifdef DATABASE_DEBUG_MESSAGE
				fprintf(stderr,"[ERROR] database_public: Retrieve error %d: %s\n",mysql_errno(my_connection),mysql_error(my_connection));
#endif
				return DATABASE_ERROR;
			}
			mysql_free_result(res_ptr);
		}
	
		return DATABASE_OK;
	}
}

/* delete in the Switch table where SwitchId = ? */
Status Delete_Switch_Table_By_SwitchId(MYSQL *my_connection,const int SwitchId) {
	int res;
	char sqlbuff[SQLBUFFLEN];
	sprintf(sqlbuff,"delete from SwitchInfoTable where SwitchId = %d", SwitchId);
	res = mysql_query(my_connection, sqlbuff);                                                                                                                  
	if(!res) {
#ifdef DATABASE_DEBUG_MESSAGE
		printf("[DEBUG] database_public: Delete %lu rows\n",(unsigned long)mysql_affected_rows(my_connection));
#endif
		return DATABASE_OK;
	} else {
#ifdef DATABASE_DEBUG_MESSAGE
		fprintf(stderr,"[ERROR] database_public: Delete error %d: %s\n", mysql_errno(my_connection), mysql_error(my_connection));
#endif
		return DATABASE_ERROR;
	}
}
/*update Switch Table (int)*/
Status Update_Switch_info_int(MYSQL *my_connection, unsigned int SwitchId , char *Rowname,unsigned int value) {
	int res;
	char sqlbuff[SQLBUFFLEN];
	sprintf(sqlbuff,"update Router set %s = '%d' where RouterID = '%d'", Rowname, value, SwitchId);
	res = mysql_query(my_connection,sqlbuff);                                                                                                                  
	if(!res) {
#ifdef DATABASE_DEBUG_MESSAGE
		printf("[DEBUG] database_public: Update %lu rows\n", (unsigned long)mysql_affected_rows(my_connection));
#endif
		return DATABASE_OK;
	} else {
#ifdef DATABASE_DEBUG_MESSAGE
		fprintf(stderr,"[ERROR] database_public: Update error %d: %s\n", mysql_errno(my_connection),mysql_error(my_connection));
#endif
		return DATABASE_ERROR;
	}

}
/*update Router Table (char)*/
Status Update_Switch_info_char(MYSQL *my_connection, unsigned int SwitchId , char *Rowname,char *value) {
	int res;
	char sqlbuff[SQLBUFFLEN];
	sprintf(sqlbuff,"update SwitchInfoTable set %s = '%s' where SwitchId = '%d'", Rowname, value, SwitchId);
	res = mysql_query(my_connection,sqlbuff);                                                                                                                  
	if(!res) {
#ifdef DATABASE_DEBUG_MESSAGE
		printf("[DEBUG] database_public: Update %lu rows\n", (unsigned long)mysql_affected_rows(my_connection));
#endif
		return DATABASE_OK;
	} else {
#ifdef DATABASE_DEBUG_MESSAGE
		fprintf(stderr,"[ERROR] database_public: Update error %d: %s\n", mysql_errno(my_connection),mysql_error(my_connection));
#endif
		return DATABASE_ERROR;
	}

}

/* ---------------------------------------------------------- */
/*                Basic_Check_Pool   functions                    */
/* ---------------------------------------------------------- */
/*Insert information into Basic_Check_Pool_Info_Table*/
Status Basic_Check_Pool_Info_Table_Insert(MYSQL *my_connection,SwitchId SwitchId,unsigned int ArpDynIfIndex,
					char *ArpDynIpAdd,unsigned int ArpDynVrf,char *ArpDynMacAdd,
						unsigned int ArpDynVlanId,unsigned int ArpDynOutIfIndex,unsigned int ArpDynExpireTime){
	
	int res;
	char sqlbuff[SQLBUFFLEN];

	
	sprintf(sqlbuff,"insert into Basic_Check_Pool(SwitchId,ArpDynIfIndex,ArpDynIpAdd,ArpDynVrf,ArpDynMacAdd,ArpDynVlanId,ArpDynOutIfIndex,ArpDynExpireTime) values(%d,%d,'%s',%d,'%s',%d,%d,%d)",SwitchId,ArpDynIfIndex,ArpDynIpAdd,ArpDynVrf,ArpDynMacAdd,ArpDynVlanId,ArpDynOutIfIndex,ArpDynExpireTime);

	res=mysql_query(my_connection,sqlbuff);
	if(!res)
	{
#ifdef DATABASE_DEBUG_MESSAGE
		printf("[DEBUG] database_netTopoAutoCatch: Insert %lu rows\n",(unsigned long)mysql_affected_rows(my_connection));
#endif
		return DATABASE_OK;
	}
	else
	{
#ifdef DATABASE_DEBUG_MESSAGE
		fprintf(stderr,"[ERROR] database_netTopoAutoCatch: Insert error %d: %s\n",mysql_errno(my_connection),mysql_error(my_connection));
#endif
		return DATABASE_ERROR;
	}

}



/* Functions:  Get Basic_Check_Pool_Info by SwitchId*/
Status Get_Basic_Check_Pool_Info(MYSQL *my_connection,ArpDynTable_INFO *arpdyntable_info, unsigned int *arpdyntable_info_nums, const unsigned int SwitchId)
{
	MYSQL_RES *res_ptr;
	MYSQL_ROW sqlrow;
	int count=0;
	int res,j;
	char sqlbuff[SQLBUFFLEN];
	sprintf(sqlbuff,"select SwitchId,ArpDynIfIndex,ArpDynIpAdd,ArpDynMacAdd,ArpDynVrf,ArpDynVlanId,ArpDynOutIfIndex,ArpDynExpireTime from Basic_Check_Pool where SwitchId=%d",SwitchId);
	
	res=mysql_query(my_connection,sqlbuff);
	if(res)
	{
#ifdef DATABASE_DEBUG_MESSAGE
		fprintf(stderr,"[ERROR] database_public: Select error %d: %s\n",mysql_errno(my_connection),mysql_error(my_connection));
#endif
		return	DATABASE_ERROR;
	}
	else
	{
		res_ptr=mysql_store_result(my_connection);
		//printf("res_ptr:%d\n",res_ptr);
		if(res_ptr)
		{
#ifdef DATABASE_DEBUG_MESSAGE
			printf("[DEBUG] database_public: Retrieved %lu rows\n",(unsigned long)mysql_num_rows(res_ptr));
#endif
			*arpdyntable_info_nums=(unsigned int)mysql_num_rows(res_ptr);			
			while((sqlrow=mysql_fetch_row(res_ptr)))
			{
#ifdef DATABASE_DEBUG_MESSAGE
				printf("[DEBUG] database_public: Fetched data...\n");
#endif
				
				arpdyntable_info[count].SwitchId=atoi(sqlrow[0]);
				arpdyntable_info[count].ArpDynIfIndex = atoi(sqlrow[1]);
				
				//arpdyntable_info[count].ArpDynIpAdd = (char*)malloc(sizeof(char) *16);
				strcpy(arpdyntable_info[count].ArpDynIpAdd,sqlrow[2]);
	
				//arpdyntable_info[count].ArpDynMacAdd = (char*)malloc(sizeof(char) *20);
				strcpy(arpdyntable_info[count].ArpDynMacAdd,sqlrow[3]);

				arpdyntable_info[count].ArpDynVrf = atoi(sqlrow[4]);
				arpdyntable_info[count].ArpDynVlanId= atoi(sqlrow[5]);
				
				arpdyntable_info[count].ArpDynOutIfIndex = atoi(sqlrow[6]);
				arpdyntable_info[count].ArpDynExpireTime= atoi(sqlrow[7]);

				count++;
				
			}
			if(mysql_errno(my_connection))
			{
#ifdef DATABASE_DEBUG_MESSAGE
				fprintf(stderr,"[ERROR] database_public: Retrieve error %d: %s\n",mysql_errno(my_connection),mysql_error(my_connection));
#endif
				return DATABASE_ERROR;
			}
			mysql_free_result(res_ptr);
		}
	
		return DATABASE_OK;
	}
}


/* delete in the Basic_Check_Pool table where SwitchId = ? */
Status Delete_Basic_Check_Pool_By_SwitchId(MYSQL *my_connection,const int SwitchId) {
	int res;
	char sqlbuff[SQLBUFFLEN];
	sprintf(sqlbuff,"delete from Basic_Check_Pool where SwitchId = %d", SwitchId);
	res = mysql_query(my_connection, sqlbuff);                                                                                                                  
	if(!res) {
#ifdef DATABASE_DEBUG_MESSAGE
		printf("[DEBUG] database_public: Delete %lu rows\n",(unsigned long)mysql_affected_rows(my_connection));
#endif
		return DATABASE_OK;
	} else {
#ifdef DATABASE_DEBUG_MESSAGE
		fprintf(stderr,"[ERROR] database_public: Delete error %d: %s\n", mysql_errno(my_connection), mysql_error(my_connection));
#endif
		return DATABASE_ERROR;
	}
}



/* ---------------------------------------------------------- */
/*                topo_host_node   functions                    */
/* ---------------------------------------------------------- */

/* Functions:  Get topo_host_node by SwitchId*/
Status Get_topo_host_node_Info(MYSQL *my_connection,ArpDynTable_INFO *arpdyntable_info, unsigned int *arpdyntable_info_nums, const unsigned int SwitchId)
{
	MYSQL_RES *res_ptr;
	MYSQL_ROW sqlrow;
	int count=0;
	int res,j;
	char sqlbuff[SQLBUFFLEN];
	sprintf(sqlbuff,"select SwitchId,ArpDynIfIndex,ArpDynIpAdd,ArpDynMacAdd,ArpDynVrf,ArpDynVlanId,ArpDynOutIfIndex,ArpDynExpireTime from topo_host_node where SwitchId=%d",SwitchId);
	
	res=mysql_query(my_connection,sqlbuff);
	printf("res:%d\n",res);
	if(res)
	{
#ifdef DATABASE_DEBUG_MESSAGE
		fprintf(stderr,"[ERROR] database_public: Select error %d: %s\n",mysql_errno(my_connection),mysql_error(my_connection));
#endif
		return	DATABASE_ERROR;
	}
	else
	{
		res_ptr=mysql_store_result(my_connection);
		//printf("res_ptr:%d\n",res_ptr);
		if(res_ptr)
		{
#ifdef DATABASE_DEBUG_MESSAGE
			printf("[DEBUG] database_public: Retrieved %lu rows\n",(unsigned long)mysql_num_rows(res_ptr));
#endif
			*arpdyntable_info_nums=(unsigned int)mysql_num_rows(res_ptr);			
			while((sqlrow=mysql_fetch_row(res_ptr)))
			{
#ifdef DATABASE_DEBUG_MESSAGE
				printf("[DEBUG] database_public: Fetched data...\n");
#endif
				
				arpdyntable_info[count].SwitchId=atoi(sqlrow[0]);
				arpdyntable_info[count].ArpDynIfIndex = atoi(sqlrow[1]);
				
				//arpdyntable_info[count].ArpDynIpAdd = (char*)malloc(sizeof(char) *16);
				strcpy(arpdyntable_info[count].ArpDynIpAdd,sqlrow[2]);
	
				//arpdyntable_info[count].ArpDynMacAdd = (char*)malloc(sizeof(char) *20);
				strcpy(arpdyntable_info[count].ArpDynMacAdd,sqlrow[3]);

				arpdyntable_info[count].ArpDynVrf = atoi(sqlrow[4]);
				arpdyntable_info[count].ArpDynVlanId= atoi(sqlrow[5]);
				
				arpdyntable_info[count].ArpDynOutIfIndex = atoi(sqlrow[6]);
				arpdyntable_info[count].ArpDynExpireTime= atoi(sqlrow[7]);

				count++;
				
			}
			if(mysql_errno(my_connection))
			{
#ifdef DATABASE_DEBUG_MESSAGE
				fprintf(stderr,"[ERROR] database_public: Retrieve error %d: %s\n",mysql_errno(my_connection),mysql_error(my_connection));
#endif
				return DATABASE_ERROR;
			}
			mysql_free_result(res_ptr);
		}
	
		return DATABASE_OK;
	}
}



/*

int main(){

	MYSQL mysql;
	Status i,j;

	i=Database_Connect(&mysql);
	SWITCH_INFO switch_info[64];
	int switch_info_nums=0;
//	Status arpdyn_info_insert=ArpDyn_Info_Table_Insert(&mysql,2,1,"192.168.200.2",2,"ae:3c:8b:4a:5a",2,6,23);
//	Status switch_info_insert=Switch_Info_Table_Insert(&mysql,2,"192.268.200.253");
//	printf("status:%d\n",switch_info_insert);
//	Status get_switch_info=Get_Switch_Info(&mysql,switch_info,&switch_info_nums,1);
//	printf("%d\n%s\n",switch_info[0].SwitchId,switch_info[0].IpAddr);
//        Status delete_switch_table_by_switchId=Delete_Switch_Table_By_SwitchId(&mysql,1);
//	printf("status:%d\n",delete_switch_table_by_switchId);

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
//	printf("status:%d\n",update_switch_char);
	j=Database_DisConnect(&mysql);
	return 	0;
}
*/
