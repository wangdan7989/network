
#include "/usr/include/mysql/mysql.h" 

#define DATABASE_DEBUG_MESSAGE
#undef DATABASE_DEBUG_MESSAGE

/* Define the micro related connection database */
#ifndef DB_CONF_FILE_H
#define DB_CONF_FILE_H
#define DB_IPADDR "localhost"
#define DB_USER   "root"
#define DB_PASSWORD "123"
#define DB_NAME   "netdata"
#endif

#ifndef DATABASE_RETURN_H
#define DATABASE_RETURN_H

#define DATABASE_OK 0
#define DATABASE_ERROR 1
#define DATABASE_INFEASIBLE -1
#define DATABASE_TRUE 2
#define DATABASE_FALSE 3

#endif

#define SQLBUFFLEN 2048
#define MAX_OBJECT_COUNT 1024


#ifndef STATUS
#define STATUS
typedef int Status;
#endif

#ifndef TABLES
#define TABLES
typedef char *Tables;
#endif

#ifndef ID_H
#define ID_H
typedef unsigned int SwitchId;
#endif


#ifndef Switch_INFO_H
#define Switch_INFO_H
typedef struct Switch_INFO_H {
	unsigned int SwitchId;
	char *IpAddr;

} SWITCH_INFO;
#endif

#ifndef ArpDynTable_INFO_H
#define ArpDynTable_INFO_H
typedef struct ArpDynTable_INFO_H {
	unsigned int SwitchId;
	unsigned long ArpDynIfIndex;
	unsigned char ArpDynIpAdd[16];
	unsigned char ArpDynMacAdd[20];
	unsigned long ArpDynVrf;
	unsigned long  ArpDynVlanId;
	unsigned long  ArpDynOutIfIndex;
	unsigned long  ArpDynExpireTime;

} ArpDynTable_INFO;
#endif



/* Database New Connected Function */
Status Database_Connect(MYSQL *my_connection);
/* Database New DisConnected Function */
Status Database_DisConnect(MYSQL *my_connection);
/* ---------------------------------------------------------- */
/*                Switch_Table   functions                    */
/* ---------------------------------------------------------- */

/* Insert information into SwitchInfoTable*/
Status Switch_Info_Table_Insert(MYSQL *my_connection,SwitchId SwitchId,char *IpAddr);

/* Functions:  Get SwitchInfo */
Status Get_Switch_Info(MYSQL *my_connection,SWITCH_INFO *switch_info, unsigned int *switch_info_nums, const unsigned char *SwitchIpAddr);

/* delete in the Switch table where SwitchId = ? */
Status Delete_Switch_Table_By_SwitchId(MYSQL *my_connection,const int SwitchId);

/*update Switch Table (int)*/
Status Update_Switch_info_int(MYSQL *my_connection, unsigned int SwitchId , char *Rowname,unsigned int value);

/*update Router Table (char)*/
Status Update_Switch_info_char(MYSQL *my_connection, unsigned int SwitchId , char *Rowname,char *value);



/* ---------------------------------------------------------- */
/*                Basic_Check_Pool_Info_Table   functions                    */
/* ---------------------------------------------------------- */


/*Insert information into Basic_Check_Pool_Info_Table*/
Status Basic_Check_Pool_Info_Table_Insert(MYSQL *my_connection,SwitchId SwitchId,unsigned int ArpDynIfIndex,
					char *ArpDynIpAdd,unsigned int ArpDynVrf,char *ArpDynMacAdd,
						unsigned int ArpDynVlanId,unsigned int ArpDynOutIfInd,unsigned int ArpDynExpireTime);

/* Functions:  Get Basic_Check_Pool_Info */
Status Get_Basic_Check_Pool_Info(MYSQL *my_connection,ArpDynTable_INFO *arpdyntable_info, unsigned int *arpdyntable_info_nums, 
					const unsigned int SwitchId);

/* delete in the Basic_Check_Pool table where SwitchId = ? */
Status Delete_Basic_Check_Pool_Table_By_SwitchId(MYSQL *my_connection,const int SwitchId);

/*******************Get_SwitchId_bySwitchIP****************/
Status Get_SwitchId_bySwitchIP(MYSQL *my_connect,const unsigned int SwitchId,const char *IpAddr);

