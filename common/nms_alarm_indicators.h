/***********************************************************************/
/*	Fucntion: the module function is alarm about all object collectors */
/*	Author: lining01@iie.ac.cn 15810423651                             */
/*                                                                     */
/***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>


#include "../../database_access_interface/database_public.h"
#include "../../database_access_interface/database_private.h"


#define 	ALARM_BASIC_INFO_PRINT_H
//#undef		ALARM_BASIC_INFO_PRINT_H

#define 	EVENTTYPE_VALUE			"系统轮询"
#define		REPORTMAN_VALUE			"系统"

#define 	MANAGESIGN_VALUE		0

#define		NUMBER_ALARM_TYPE		1
#define		STRING_ALARM_TYPE		2
#define		SNMP_ALARM_TYPE			3


#define		UN_HANDLE_MANAGESIGN	0
#define		HANDLED_MANAGESIGN		4


/* FROM nms_alarm_indicators_node and topo_host_node TABLE to get info */
#ifndef ALARM_BASIC_INFO_H
#define ALARM_BASIC_INFO_H
typedef struct ALARM_BASIC_INFO {
	unsigned int node_id;
	unsigned char name[32];
	unsigned char type[32];
	unsigned char subtype[32];
	unsigned char datatype[64];
	unsigned char threshold_unit[8];
	unsigned int compare;
	unsigned char alarm_info[128];
	unsigned int enabled;
	long limenvalue0;
	long limenvalue1;
	long limenvalue2;
	unsigned int time0;
	unsigned char cur_times[255];
	unsigned char alias[256];
	unsigned char ip[32];
}ALARM_BASIC_INFO;
#endif


/* put all alarms info to the system_eventlist table in the NMS database */
#ifndef ALARM_RESULT_INFO_H
#define ALARM_RESULT_INFO_H
typedef struct ALARM_RESULT_INFO {
	unsigned char eventtype[16];
	unsigned char eventlocation[64];
	unsigned char content[512];
	unsigned int level;
	unsigned int mamagesign;
	unsigned char recordtime[32];
	unsigned char reportman[16];
	unsigned int node_id;
	unsigned char subtype[32];
	unsigned int subentity_index;
	unsigned char subentity[32];
}ALARM_RESULT_INFO;
#endif

#ifndef CUR_TIMES_MAP_H
#define CUR_TIMES_MAP_H
typedef struct CUR_TIMES_MAP {
	unsigned int entity_name_index;
	unsigned int cur_times;
}CUR_TIMES_MAP;
#endif


#ifndef ALARM_BASIC_CONF_INFO_H
#define ALARM_BASIC_CONF_INFO_H
typedef struct ALARM_BASIC_CONF_INFO {
	unsigned long nodeid;
	unsigned long indexid;
	unsigned short indicator_type;
	unsigned short collector_type;
	unsigned short is_alarm;
	long limenvalue0;
	long limenvalue1;
	long limenvalue2;
 }ALARM_BASIC_CONF_INFO;
#endif


/**
 * Get all alarm info about collecors to the structure array.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param node_id: the object node_id value in the topo_host_node table.
 * @param index_name: the name value in the nms_alarm_indicators_node table.
 * @param p_alarm_basic_info: the alarm_basic_info structure pointer.
 * @return 0 on success, or return 1 on failure.
 */
int get_alarm_basic_info_from_table(MYSQL pConn, 
									unsigned int node_id, 
									const unsigned char index_name[], 
									ALARM_BASIC_INFO *p_alarm_basic_info);
/**
 * Get all alarm info about collectors to the structure array.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param node_id: the object node_id value in the topo_host_node table.
 * @param entity_name: the entity_name value in the nms_alarm_indicators_node table, for example ping.
 * @param alarm_value_type: the alarm value type, 1 is number, 2 is string value.
 * @param number_alarm_value: the number type value.
 * @param string_alarm_value: the string type value.
 * @param string_old_value: the string old value.
 * @param future_used_value: the bak argument.
 * @return 0 on success, 1 on failure.
 */									
int nms_alarm_indicators_centor(MYSQL pConn, 
								const unsigned int node_id, 
								const unsigned char *entity_name,
								const unsigned long entity_name_index,
								const unsigned int alarm_value_type,
								const double number_alarm_value,
								const unsigned char *string_alarm_value,
								const unsigned char *string_old_value,
								const unsigned char *entity_alias);


/**
 * Get system current time.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param current_time: the current system time.
 * @return: no return value.
 */
void get_current_time(char *current_time);


									  
/**
 * Put number alarm info into database system_eventlist table.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param node_id: the object node_id value in the topo_host_node table.
 * @param level: the alarm level, then just 1, 2, 3.
 * @param content: the alarm messages.
 * @param subtype: the subtype value, for example linux.
 * @param subentity: the alarm entity value, for example cpu.
 * @param ip: the alarm ip address.
 * @param alias: from table topo_host_node column alias.
 * @param unit: the alarm value unit, for example %,kb/s.
 * @return 0 on success, others on failure.
 */
int put_number_alarm_infos_into_table(MYSQL pConn,
									  const unsigned int node_id,
									  const unsigned int level,
									  const unsigned int current_value,
									  const unsigned int alarm_value,
									  const unsigned char *content,
									  const unsigned char *subtype,
									  const unsigned int subentity_index,
									  const unsigned char *subentity,
									  const unsigned char *ip,
									  const unsigned char *alias,
									  const unsigned char *unit,
									  const unsigned char *entity_alias);	
/**
 * Put string alarm info into database system_eventlist table.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param node_id: the object node_id value in the topo_host_node table.
 * @param level: the alarm level, then just 1, 2, 3.
 * @param alarm_info: the alarm messages.
 * @param subtype: the subtype value, for example linux.
 * @param subentity: the alarm entity value, for example cpu.
 * @param ip: the ip address.
 * @param current_value: the current_value value.
 * @param alias: the subtype value, for example linux.
 * @param old_value: the old value.
 * @return 0 on success, others on failure.
 */
int put_string_alarm_infos_into_table(MYSQL pConn, 
									  const unsigned node_id, 
									  const unsigned char *alarm_info,
									  const unsigned char *subtype,
									  const unsigned int entity_name_index,
									  const unsigned char *subentity,
									  const unsigned char *ip,
									  const unsigned char *alias,
									  const unsigned char *current_value,
									  const unsigned char *old_value,
									  const unsigned char *entity_alias);		
/**
 * Put string alarm info into database system_eventlist table.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param managesign: the managesign value.
 * @param node_id: the topo_host_node id value.
 * @param subentity: the system_eventlist value.
 * @param id_value: the id value from system_eventlist.
 * @param id_nums: the id value numbers.
 * @return 0 on success, 1 on failure.
 */
int get_id_value_from_system_eventlist_table(MYSQL pConn, 
											 const unsigned long managesign,
											 const unsigned long node_id,
											 const unsigned int subentity_index,
											 const unsigned char subentity[],
											 unsigned int *id_value,
											 unsigned int *id_nums);
/**
 * Put string alarm info into database system_eventlist table.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param id: the id value from system_eventlist.
 * @param alarm_times: the alarm_times about system_eventlist table.
 * @return 0 on success, 1 on failure.
 */
int get_alarm_times_from_system_eventlist_table(MYSQL pConn, 
												const unsigned int id,
												unsigned int *alarm_times);

/**
 * Put string alarm info into database system_eventlist table.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param id: the id value from system_eventlist.
 * @param alarm_times: the alarm_times about system_eventlist table.
 * @param level: the level value from system_eventlist.
 * @param lasttime: the lasttime about system_eventlist table.
 * @return 0 on success, 1 on failure.
 */
int update_system_eventlist_table_info(MYSQL pConn, 
									   const unsigned int id,
									   const unsigned int level,
									   const unsigned int alarm_times,
									   const unsigned char *content,
									   const unsigned char *lasttime);

/**
 * Put string alarm info into database system_eventlist table.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param id: the id value from system_eventlist.
 * @param managesign: the managesign about system_eventlist table.
 * @return 0 on success, 1 on failure.
 */
int update_managesign_system_eventlist_table(MYSQL pConn, 
											 const unsigned int id,
											 const unsigned int managesign);
	
/**
 * Put string alarm info into database system_eventlist table.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param node_id: the object node_id value in the topo_host_node table.
 * @param level: the alarm level, then just 1, 2, 3.
 * @param alarm_info: the alarm messages.
 * @param subtype: the subtype value, for example linux.
 * @param subentity: the alarm entity value, for example cpu.
 * @param ip: the ip address.
 * @param current_value: the current_value value.
 * @param alias: the subtype value, for example linux.
 * @param old_value: the old value.
 * @return 0 on success, others on failure.
 */
int put_auto_recover_alarm_message_into_table(MYSQL pConn,
											  const unsigned node_id, 
											  const unsigned char *alarm_info,
											  const unsigned char *subtype,
											  const unsigned int subentity_index,
											  const unsigned char *subentity,
											  const unsigned char *ip,
											  const unsigned char *alias,
											  const unsigned char *starttime,
											  const unsigned char *lasttime,
											  const unsigned int alarm_times,
											  const unsigned char *entity_alias,
											  const float cur_value,
											  const float limit0,
											  const unsigned char *threshold_unit);
											  
											  
/**
 * Get the current times map value.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param string_value: the string value current times value.
 * @param cur_times_map: the map current map value.
 * @param nums: the map number value.
 * @return 0 on success, 1 on failure.
 */
int get_int_cur_times_value_from_string_by_subentity_index(const char *string_value,
														   CUR_TIMES_MAP cur_times_map[], 
														   unsigned int *nums);
														   
/**
 * Get the current string current value.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param string_cur_times: the string value current times value.
 * @param cur_times_map: the map current map value.
 * @param cur_times_map_len: the map number value.
 * @return 0 on success, 1 on failure.
 */
int get_string_cur_times_value_from_int_by_subentity_index(const CUR_TIMES_MAP *cur_times_map, 
														   const unsigned int cur_times_map_len,
														   unsigned char *string_cur_times);


/**
 * Start alarm the snmp exception.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param pConn: the database connect handle.
 * @param node_id: the device nodeid from topo_host_node.
 * @param snmp_alarm_string: the alarm string value.
 * @return 0 on success, 1 on failure.
 */
int nms_snmp_exception_alarm(MYSQL pConn, unsigned int node_id, const unsigned char *snmp_alarm_string);

/**
 * Put snmp alarm exception into database.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param pConn: the database connect handle.
 * @param node_id: the device nodeid from topo_host_node.
 * @param snmp_alarm_string: the alarm string value.
 * @return 0 on success, 1 on failure.
 */
int put_snmp_alarm_infos_into_table(MYSQL pConn, 
									unsigned int node_id, 
									unsigned char *ip, 
									unsigned char *alias, 
									const unsigned char *snmp_alarm_string);

/**
 * Update snmp alarm exception into database.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param pConn: the database connect handle.
 * @param node_id: the device nodeid from topo_host_node.
 * @param snmp_alarm_string: the alarm string value.
 * @return 0 on success, 1 on failure.
 */
int update_snmp_alarm_infos_into_table(MYSQL pConn, unsigned int id, unsigned int current_alarm_times, char *lasttime);

/**
 * Start alarm the snmp recovery.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param pConn: the database connect handle.
 * @param node_id: the device nodeid from topo_host_node.
 * @param snmp_alarm_string: the alarm string value.
 * @return 0 on success, 1 on failure.
 */
int nms_snmp_recovery_alarm(MYSQL pConn, unsigned int node_id);

/**
 * Get all alarm info about collectors to the structure array.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param node_id: the object node_id value in the topo_host_node table.
 * @param index_name: the name value in the nms_alarm_indicators_node table.
 * @param p_alarm_basic_info: the alarm_basic_info structure pointer.
 * @return 0 on success, or return 1 on failure.
 */
int get_alarm_basic_conf_info_from_table(MYSQL pConn,
										unsigned long nodeid, 
										unsigned long indexid,
										unsigned short indicator_type,
										unsigned short collector_type,
										ALARM_BASIC_CONF_INFO *p_alarm_basic_conf_info,
										unsigned int *alarm_basic_conf_info_nums);										   