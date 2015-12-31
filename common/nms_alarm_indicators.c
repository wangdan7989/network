/***********************************************************************/
/*	Function: the module function is alarm about all object collectors */
/*	Author: lining01@iie.ac.cn 15810423651                             */
/*                                                                     */
/***********************************************************************/

#include "nms_alarm_indicators.h"


/**
 * Get all alarm info about collectors to the structure array.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param node_id: the object node_id value in the topo_host_node table.
 * @param index_name: the name value in the nms_alarm_indicators_node table.
 * @param p_alarm_basic_info: the alarm_basic_info structure pointer.
 * @return 0 on success, or return 1 on failure.
 */
int get_alarm_basic_info_from_table(MYSQL pConn,
									unsigned int node_id, 
									const unsigned char *index_name, 
									ALARM_BASIC_INFO *p_alarm_basic_info) {
	
	int ret = 0;
	char objip[32];
	char alias[256];

	unsigned int alarm_basic_info_nums = 0;
	ret = Get_Info_From_nms_alarm_indicators_node_Table(&pConn, 
														 node_id, 
														 index_name, 
														 p_alarm_basic_info, 
														 &alarm_basic_info_nums);
	if(ret != 0) {
		printf("[ERROR] call Get_Info_From_nms_alarm_indicators_node_Table function failed in get_alarm_basic_info_from_table function\n");
		return 1;
	}

	if(alarm_basic_info_nums == 0) {
		printf("[ERROR] get empty from nms_alarm_indicators_node table in get_alarm_basic_info_from_table function\n");
		return 1;
	}

	ret = Get_IP_And_Alias_From_topo_host_node_Table(&pConn,  node_id, objip, alias);
	
	if(ret != 0) {
		printf("[ERROR] call Get_IP_And_Alias_From_topo_host_node_Table failed in get_alarm_basic_info_from_table function\n");
		return 1;
	}
	
	strncpy(p_alarm_basic_info->alias, alias, 256);
	strncpy(p_alarm_basic_info->ip, objip, 32);
	
	if(alarm_basic_info_nums > 1 ) {
		printf("[ERROR] get info more 1 items from nms_alarm_indicators_node table in get_alarm_basic_info_from_table function\n");
		return 1;
	}
	return 0;
}

/**
 * Get system current time.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param current_time: the current system time.
 * @return: no return value.
 */
void get_current_time(char *current_time) {
	
	time_t now_time;
	time(&now_time);
	struct tm *p;
	char tmp[8];
	p = localtime(&now_time);
	
	sprintf(current_time, "%d", p->tm_year + 1900);
	memset(tmp, 0, 8);
	if(p->tm_mon + 1 < 10) {
		snprintf(tmp, 8, "-0%d", p->tm_mon + 1);
	} else {
		snprintf(tmp, 8, "-%d", p->tm_mon + 1);
	}
	strncat(current_time, tmp, 8);
	
	memset(tmp, 0, 8);
	if(p->tm_mday < 10) {
		snprintf(tmp, 8, "-0%d", p->tm_mday);
	} else {
		snprintf(tmp, 8, "-%d", p->tm_mday);
	}
	strncat(current_time, tmp, 8);
	
	memset(tmp, 0, 8);
	if(p->tm_hour < 10) {
		snprintf(tmp, 8, " 0%d", p->tm_hour);
	} else {
		snprintf(tmp, 8, " %d", p->tm_hour);
	}
	strncat(current_time, tmp, 8);

	memset(tmp, 0, 8);
	if(p->tm_min < 10) {
		snprintf(tmp, 8, ":0%d", p->tm_min);
	} else {
		snprintf(tmp, 8, ":%d", p->tm_min);
	}
	strncat(current_time, tmp, 8);
	
	memset(tmp, 0, 8);
	if(p->tm_sec < 10) {
		snprintf(tmp, 8, ":0%d", p->tm_sec);
	} else {
		snprintf(tmp, 8, ":%d", p->tm_sec);
	}
	strncat(current_time, tmp, 8);
}


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
									  const unsigned char *entity_alias) {
								   
	unsigned char content_result[256];
	unsigned char eventlocation[128];
	unsigned char record_time[64];
	unsigned int  count = 0;
	
	if(!(level == 1 || level == 2 || level == 3)) {
		printf("[ERROR] the argument level is wrong, just only is 1, 2, 3 level is right in put_number_alarm_infos_into_table function\n");
		return 1;
	}
	
	if(content == NULL || subtype == NULL || subentity ==NULL || ip == NULL || alias == NULL) {
		printf("[ERROR] the arguments content, subtype, subentity is null in put_number_alarm_infos_into_table function\n");
		return 1;
	}
	
	memset(content_result, 0, 256);
	memset(eventlocation,  0, 128);
	snprintf(eventlocation, 128, "%s(%s)", alias, ip);
	char *cur_string = "实时状态";
	char *alarm_string = "告警阀值";
	snprintf(content_result, 256, "%s(%s) %s%s %s：%d%s，%s：%d%s", 
			 alias, ip, entity_alias, content, cur_string, current_value, unit, alarm_string, alarm_value, unit);
	
	ALARM_RESULT_INFO alarm_result_info;
	
	get_current_time(record_time);
	
	strncpy(alarm_result_info.eventtype, EVENTTYPE_VALUE, 16);
	strncpy(alarm_result_info.eventlocation, eventlocation , 64);
	strncpy(alarm_result_info.content, content_result, 256);
	alarm_result_info.level = level;
	alarm_result_info.mamagesign = MANAGESIGN_VALUE;
	strncpy(alarm_result_info.recordtime, record_time, 32);
	strncpy(alarm_result_info.reportman, REPORTMAN_VALUE, 16);
	alarm_result_info.node_id = node_id;
	strncpy(alarm_result_info.subtype, subtype, 32);
	alarm_result_info.subentity_index = subentity_index;
	strncpy(alarm_result_info.subentity, subentity, 32);
	
	int ret = 0;
	ret = Alarm_Info_Insert_Into_system_eventlist_Table(&pConn, 
														alarm_result_info.eventtype,
														alarm_result_info.eventlocation,
														alarm_result_info.content, 
														alarm_result_info.level,
														alarm_result_info.mamagesign,
														alarm_result_info.recordtime,
														alarm_result_info.reportman,
														alarm_result_info.node_id,
														alarm_result_info.subtype,
														alarm_result_info.subentity_index,
														alarm_result_info.subentity,
														alarm_result_info.recordtime);

	if(ret != 0) {
		printf("[ERROR] call Alarm_Info_Insert_Into_system_eventlist_Table function failed\n");
		return 1;
	}
	
	return 0;
}


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
								const unsigned char *entity_alias) {
	
	if(entity_name == NULL || string_alarm_value == NULL || string_old_value == NULL) {
		printf("[ERROR] the argument value is null in nms_alarm_indicators_centor function\n");
		return 1;
	}
	
	
	if(alarm_value_type == SNMP_ALARM_TYPE) {
		int ret_snmp = 0;
		ret_snmp = nms_snmp_exception_alarm(pConn, node_id, string_alarm_value);
		return ret_snmp;
	}	
	
	
	ALARM_BASIC_INFO *alarm_basic_info = (ALARM_BASIC_INFO *)malloc(sizeof(ALARM_BASIC_INFO));
	unsigned int count = 0;
	unsigned int ret = 0;
	unsigned int cur_times = 0;
	unsigned int max_times = 0;
	float limentvalue0 = 0;
	float limentvalue1 = 0;
	float limentvalue2 = 0;
	unsigned int level = 1;
	unsigned int id = 0;
	unsigned int flag_insert_or_update = 0;
	
	unsigned char string_cur_times[255];
	CUR_TIMES_MAP cur_times_map[255];
	unsigned int cur_times_map_nums = 0;
	memset(cur_times_map, 0, sizeof(cur_times_map));
	memset(string_cur_times, 0, 255);
		
	char cur_time[64];
	memset(cur_time, 0, 64);
	get_current_time(cur_time);
	
	
	ret = get_alarm_basic_info_from_table(pConn, node_id, entity_name, alarm_basic_info);
	if(ret != 0) {
		printf("[ERROR] call get_alarm_basic_info_from_table return %lu failed in the alarm function\n", ret);
		free(alarm_basic_info);
		return 1;
	}

/*********************add alarm indicators conf start *****************************/	
	ALARM_BASIC_CONF_INFO alarm_basic_conf_info;
	unsigned int alarm_basic_conf_info_nums = 0;
	unsigned short indicator_type = 0;
	unsigned short collector_type = 0;
	if(!strcmp(entity_name, "ifadminstatus")){
		indicator_type = 1;
		collector_type = 1;
	}
	
	if(!strcmp(entity_name, "ifoperstatus")){
		indicator_type = 2;
		collector_type = 1;
	}
	
	if(!strcmp(entity_name, "utilhdx")){
		indicator_type = 3;
		collector_type = 1;
	}

	if(!strcmp(entity_name, "AllInBandwidthUtilHdx")){
		indicator_type = 4;
		collector_type = 1;
	}

	if(!strcmp(entity_name, "AllOutBandwidthUtilHdx")){
		indicator_type = 5;
		collector_type = 1;
	}

	if(!strcmp(entity_name, "inerrorrate")){
		indicator_type = 6;
		collector_type = 1;
	}

	if(!strcmp(entity_name, "outerrorrate")){
		indicator_type = 7;
		collector_type = 1;
	}

	if(!strcmp(entity_name, "indiscardrate")){
		indicator_type = 8;
		collector_type = 1;
	}

	if(!strcmp(entity_name, "outdiscardrate")){
		indicator_type = 9;
		collector_type = 1;
	}
	
	if(get_alarm_basic_conf_info_from_table(pConn, 
											node_id, 
											entity_name_index, 
											indicator_type, 
											collector_type, 
											&alarm_basic_conf_info, 
											&alarm_basic_conf_info_nums)) {
		printf("[ERROR] call get_alarm_basic_conf_info_from_table return wrong in the alarm function\n");
	} else {
		if(alarm_basic_conf_info_nums == 0) {
			printf("[DEBUG] the alarm_basic_conf_info_nums is 0, should alarm by default value\n");
		} else {
			if(alarm_basic_conf_info.is_alarm == 1) {
				printf("[DEBUG] the %d indicators don not need alarm, return 0 in the alarm function\n", entity_name_index);
				alarm_basic_info->enabled == 0;
				free(alarm_basic_info);
				return 0;
			} else if(alarm_basic_conf_info.is_alarm == 2) {
				printf("[DEBUG] the %d indicators need alarm in the alarm function\n", entity_name_index);
				alarm_basic_info->enabled == 1;
				alarm_basic_info->limenvalue0 = alarm_basic_conf_info.limenvalue0;
				alarm_basic_info->limenvalue1 = alarm_basic_conf_info.limenvalue1;
				alarm_basic_info->limenvalue2 = alarm_basic_conf_info.limenvalue2;
			}
		}		
	}
/*********************add alarm indicators conf end *******************************/	

	
	if(!(alarm_basic_info->enabled == 0 || alarm_basic_info->enabled == 1)) {
		printf("[ERROR] the enabled value is invalid just only 0 or 1 in the alarm function\n");
		free(alarm_basic_info);
		return 1;
	}
	
	if(alarm_basic_info->enabled == 0) {
		printf("[DEBUG] the alarm entity enabled is 0, do not need alarm in the alarm function\n");
		free(alarm_basic_info);
		return 0;
	}
		
	if(!(alarm_value_type == NUMBER_ALARM_TYPE || alarm_value_type == STRING_ALARM_TYPE || alarm_value_type == SNMP_ALARM_TYPE)) {
		printf("[ERROR] the alarm_value_type value is invalid just only 1 or 2 or 3 in the alarm function\n");
		free(alarm_basic_info);
		return 1;
	}

	if(get_id_value_from_system_eventlist_table(pConn,
												UN_HANDLE_MANAGESIGN, 
												alarm_basic_info->node_id,
												entity_name_index,
												alarm_basic_info->name,
												&id, &flag_insert_or_update) != 0) {
		printf("[ERROR] call get_id_value_from_system_eventlist_table failed in the alarm function\n");
		free(alarm_basic_info);
		return 1;
	}

	if(get_int_cur_times_value_from_string_by_subentity_index(alarm_basic_info->cur_times,
														      cur_times_map, 
														      &cur_times_map_nums) != 0) {
		printf("[ERROR] call get_int_cur_times_value_from_string_by_subentity_index failed in the alarm function\n");
		free(alarm_basic_info);
		return 1;
	}
	unsigned flag_in_map = 0;
	for(count = 0; count < cur_times_map_nums; count++) {
		if(entity_name_index == cur_times_map[count].entity_name_index) {
			cur_times =  cur_times_map[count].cur_times;
			flag_in_map = 1;
			break;
		}
	}
	
	if(flag_in_map == 0) {
		cur_times_map[cur_times_map_nums].entity_name_index = entity_name_index;
		cur_times_map[cur_times_map_nums].cur_times = 0;
		cur_times_map_nums++;
		cur_times = 0;
	}
	
	max_times = alarm_basic_info->time0;
	limentvalue0 = (float)(alarm_basic_info->limenvalue0);
	limentvalue1 = (float)(alarm_basic_info->limenvalue1);
	limentvalue2 = (float)(alarm_basic_info->limenvalue2);

	if(alarm_value_type == NUMBER_ALARM_TYPE) {
		
		if(strncmp(alarm_basic_info->threshold_unit, "%", 2) == 0) {
			if(number_alarm_value < 0 || number_alarm_value > 100) {
				printf("[ERROR] the number_alarm_value value out of 0 and 100 range in the alarm function\n");
				free(alarm_basic_info);
				return 1;
			}
		}
		if(!(alarm_basic_info->compare == 0 || alarm_basic_info->compare == 1)) {
			printf("[ERROR] the compare value is invalid just only 0 or 1 in the alarm function\n");
			free(alarm_basic_info);
			return 1;
		}
		
		if(alarm_basic_info->compare == 1) {
	
			if(!(alarm_basic_info->limenvalue0 <= alarm_basic_info->limenvalue1 && alarm_basic_info->limenvalue1 <= alarm_basic_info->limenvalue2)) {
				printf("[ERROR] the sequence of limit value is wrong should from small to big in the alarm function\n");
				free(alarm_basic_info);
				return 1;
			}
	
			if(number_alarm_value < limentvalue0) {
			
				for(count = 0; count < cur_times_map_nums; count++) {
					if(entity_name_index == cur_times_map[count].entity_name_index) {
						cur_times_map[count].cur_times = 0;
						break;
					}
				}		
				if(cur_times != 0) {
					if(get_string_cur_times_value_from_int_by_subentity_index(cur_times_map, 
																			  cur_times_map_nums,
																			  string_cur_times) != 0) {
						printf("[ERROR] call get_string_cur_times_value_from_int_by_subentity_index failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;
					}
					if(Update_nms_alarm_indicators_node_Table_cur_times_Field(&pConn, node_id, entity_name, string_cur_times) != 0) {
						printf("[ERROR] call Update_nms_alarm_indicators_node_Table_cur_times_Field failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;
					}
				}
				
				if(flag_insert_or_update == 0) { /* normal */
					printf("[DEBUG] current value:%.2f is normal, alarm value:%.2f don't need alarm in the alarm function\n", 
						   number_alarm_value, limentvalue0);
				} else if(flag_insert_or_update == 1) {		/* need auto recover */			

					if(update_managesign_system_eventlist_table(pConn, id, HANDLED_MANAGESIGN) != 0) {
						printf("[ERROR] call update_managesign_system_eventlist_table failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;
					}

					char record_time[64];
					memset(record_time, 0, 64);
					if(get_recordtime_from_system_eventlist_table(pConn, id, record_time) !=0) {
						printf("[ERROR] call get_recordtime_from_system_eventlist_table failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;
					}
					
					/* add get current alarm times from system_eventlist table */
					unsigned int alarm_times_from_system_eventlist = 0;
					if(get_alarm_times_from_system_eventlist_table(pConn, id, &alarm_times_from_system_eventlist) != 0) {
						printf("[ERROR] call get_alarm_times_from_system_eventlist_table failed in the alarm function\n");
						/* if get failed from system_eventlist table, then computer the value by cur_times*/
						if((cur_times - alarm_basic_info->time0 + 1) < 0) {
							alarm_times_from_system_eventlist = 1;
						} else {
							alarm_times_from_system_eventlist = cur_times - alarm_basic_info->time0 + 1;
						}
					}
					/* end add get current alarm times from system_eventlist table*/

					
					if(put_auto_recover_alarm_message_into_table(pConn, 
																 node_id, 
																 alarm_basic_info->alarm_info,
																 alarm_basic_info->subtype,
																 entity_name_index,
																 alarm_basic_info->name,
																 alarm_basic_info->ip,
																 alarm_basic_info->alias,
																 record_time,
																 cur_time,
																 alarm_times_from_system_eventlist,
																 entity_alias,
																 number_alarm_value,
																 limentvalue0,
																 alarm_basic_info->threshold_unit) != 0) {
						printf("[ERROR] call put auto_recover_alarm_message_into_table failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;
					}
				}
				
			} else if(number_alarm_value < limentvalue1) {
			//	if(alarm_basic_info->time0 == 1) {
			//		level = 3;
			//	} else {
			//		level = 1;
			//	}
				
				level = 1;
				
				for(count = 0; count < cur_times_map_nums; count++) {
					if(entity_name_index == cur_times_map[count].entity_name_index) {
						cur_times_map[count].cur_times++;
						break;
					}
				}
				if(get_string_cur_times_value_from_int_by_subentity_index(cur_times_map, 
																		  cur_times_map_nums,
																		  string_cur_times) != 0) {
					printf("[ERROR] call get_string_cur_times_value_from_int_by_subentity_index failed in the alarm function\n");
					free(alarm_basic_info);
					return 1;
				}
		
				if(Update_nms_alarm_indicators_node_Table_cur_times_Field(&pConn, node_id, entity_name, string_cur_times) != 0) {
					printf("[ERROR] call Update_nms_alarm_indicators_node_Table_cur_times_Field failed in the alarm function\n");
					free(alarm_basic_info);
					return 1;
				}
				
				if(flag_insert_or_update == 0) {
					cur_times++;
					if(cur_times >= max_times) {
						int ret = put_number_alarm_infos_into_table(pConn,
																	node_id, 
																	level, 
																	number_alarm_value,
																	alarm_basic_info->limenvalue0,
																	alarm_basic_info->alarm_info,
																	alarm_basic_info->subtype,
																	entity_name_index,
																	alarm_basic_info->name,
																	alarm_basic_info->ip,
																	alarm_basic_info->alias,
																	alarm_basic_info->threshold_unit,
																	entity_alias);
						if(ret != 0) {
							printf("[ERROR] call put_number_alarm_infos_into_table failed in the alarm function\n");
							free(alarm_basic_info);
							return 1;
						}
					}
				} else if(flag_insert_or_update == 1) {
				
					unsigned int alarm_times = 0;
					if(get_alarm_times_from_system_eventlist_table(pConn, id, &alarm_times) != 0) {
						printf("[ERROR] call get_alarm_times_from_system_eventlist_table function failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;	
					}
					char content_result[256];
					memset(content_result, 0, 256);
					snprintf(content_result, 256, "%s(%s) %s%s 实时状态：%d%s，告警阀值：%d%s", 
							 alarm_basic_info->alias, 
							 alarm_basic_info->ip, 
							 entity_alias,
							 alarm_basic_info->alarm_info, 
							 (long)number_alarm_value,
							 alarm_basic_info->threshold_unit, 
							 alarm_basic_info->limenvalue0, 
							 alarm_basic_info->threshold_unit);
							 
					if(update_system_eventlist_table_info(pConn, id, level, alarm_times + 1, content_result, cur_time) != 0) {
						printf("[ERROR] call update_system_eventlist_table_info failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;	
					}
				}
			}else if(number_alarm_value < limentvalue2) {
			//	if(alarm_basic_info->time0 == 1) {
			//		level = 3;
			//	} else {
			//		level = 2;
			//	}
				level = 2;
				for(count = 0; count < cur_times_map_nums; count++) {
					if(entity_name_index == cur_times_map[count].entity_name_index) {
						cur_times_map[count].cur_times++;
					}
				}
				if(get_string_cur_times_value_from_int_by_subentity_index(cur_times_map, 
																		  cur_times_map_nums,
																		  string_cur_times) != 0) {
					printf("[ERROR] call get_string_cur_times_value_from_int_by_subentity_index failed in the alarm function\n");
					free(alarm_basic_info);
					return 1;
				}
				if(Update_nms_alarm_indicators_node_Table_cur_times_Field(&pConn, node_id, entity_name, string_cur_times) != 0) {
					printf("[ERROR] call Update_nms_alarm_indicators_node_Table_cur_times_Field failed in the alarm function\n");
					free(alarm_basic_info);
					return 1;
				}
				
				if(flag_insert_or_update == 0) {
					cur_times++;
					if(cur_times >= max_times) {
						int ret = put_number_alarm_infos_into_table(pConn,
																	node_id, 
																	level, 
																	number_alarm_value,
																	alarm_basic_info->limenvalue1,
																	alarm_basic_info->alarm_info,
																	alarm_basic_info->subtype,
																	entity_name_index,
																	alarm_basic_info->name,
																	alarm_basic_info->ip,
																	alarm_basic_info->alias,
																	alarm_basic_info->threshold_unit,
																	entity_alias);
						if(ret != 0) {
							printf("[ERROR] call put_number_alarm_infos_into_table failed in the alarm function\n");
							free(alarm_basic_info);
							return 1;
						}
					}

				} else if(flag_insert_or_update == 1) {
	
					unsigned int alarm_times = 0;
					if(get_alarm_times_from_system_eventlist_table(pConn, id, &alarm_times) != 0) {
						printf("[ERROR] call get_alarm_times_from_system_eventlist_table failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;	
					}

					char content_result[256];
					memset(content_result, 0, 256);
					snprintf(content_result, 256, "%s(%s) %s%s 实时状态：%d%s，告警阀值：%d%s",
							 alarm_basic_info->alias, 
							 alarm_basic_info->ip, 
							 entity_alias,
							 alarm_basic_info->alarm_info,
							 (long)number_alarm_value,
							 alarm_basic_info->threshold_unit,
							 alarm_basic_info->limenvalue1,
							 alarm_basic_info->threshold_unit);					
					
					if(update_system_eventlist_table_info(pConn, id, level, alarm_times + 1, content_result, cur_time) != 0) {
						printf("[ERROR] call update_system_eventlist_table_info failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;	
					}
				}
				
			} else {
			
			//	if(alarm_basic_info->time0 == 1) {
			//		level = 3;
			//	} else {
			//		level = 3;
			//	}
				level = 3;
				for(count = 0; count < cur_times_map_nums; count++) {
					if(entity_name_index == cur_times_map[count].entity_name_index) {
						cur_times_map[count].cur_times++;
					}
				}
		
				if(get_string_cur_times_value_from_int_by_subentity_index(cur_times_map, 
																		  cur_times_map_nums,
																		  string_cur_times) != 0) {
					printf("[ERROR] call get_string_cur_times_value_from_int_by_subentity_index failed\n");
					free(alarm_basic_info);
					return 1;
				}
			
				if(Update_nms_alarm_indicators_node_Table_cur_times_Field(&pConn, node_id, entity_name, string_cur_times) != 0) {
					printf("[ERROR] call Update_nms_alarm_indicators_node_Table_cur_times_Field failed in the alarm function\n");
					free(alarm_basic_info);
					return 1;
				}
		
				if(flag_insert_or_update == 0) {
					cur_times++;
					if(cur_times >= max_times) {
						int ret = put_number_alarm_infos_into_table(pConn,
																	node_id, level, 
																	number_alarm_value,
																	alarm_basic_info->limenvalue2,
																	alarm_basic_info->alarm_info,
																	alarm_basic_info->subtype,
																	entity_name_index,
																	alarm_basic_info->name,
																	alarm_basic_info->ip,
																	alarm_basic_info->alias,
																	alarm_basic_info->threshold_unit,
																	entity_alias);
						if(ret != 0) {
							printf("[ERROR] call put_number_alarm_infos_into_table failed in the alarm function\n");
							free(alarm_basic_info);
							return 1;
						}
					}
				} else if(flag_insert_or_update == 1) {

					unsigned int alarm_times = 0;
					if(get_alarm_times_from_system_eventlist_table(pConn, id, &alarm_times) != 0) {
						printf("[ERROR] call get_alarm_times_from_system_eventlist_table function failed\n");
						free(alarm_basic_info);
						return 1;	
					}
					
					char content_result[256];
					memset(content_result, 0, 256);
					snprintf(content_result, 256, "%s(%s) %s%s 实时状态：%d%s，告警阀值：%d%s",
							 alarm_basic_info->alias, 
							 alarm_basic_info->ip, 
							 entity_alias,
							 alarm_basic_info->alarm_info,
							 (long)number_alarm_value,
							 alarm_basic_info->threshold_unit,
							 alarm_basic_info->limenvalue2,
							 alarm_basic_info->threshold_unit);	
												
					if(update_system_eventlist_table_info(pConn, id, level, alarm_times + 1, content_result, cur_time) != 0) {
						printf("[ERROR] call update_system_eventlist_table_info function failed\n");
						free(alarm_basic_info);
						return 1;	
					}
				}
			}
		} 
		
		if(alarm_basic_info->compare == 0) {
		
			if(!(alarm_basic_info->limenvalue0 >= alarm_basic_info->limenvalue1 && alarm_basic_info->limenvalue1 >= alarm_basic_info->limenvalue2)) {
				printf("[ERROR] the sequence of limit value is wrong should from big to small in the alarm function\n");
				free(alarm_basic_info);
				return 1;
			}
			if(number_alarm_value > limentvalue0) {

				for(count = 0; count < cur_times_map_nums; count++) {
					if(entity_name_index == cur_times_map[count].entity_name_index) {
						cur_times_map[count].cur_times = 0;
					}
				}

				if(cur_times != 0 ) {
					if(get_string_cur_times_value_from_int_by_subentity_index(cur_times_map, 
																			cur_times_map_nums,
																			string_cur_times) != 0) {
						printf("[ERROR] call get_string_cur_times_value_from_int_by_subentity_index failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;
					}
					if(Update_nms_alarm_indicators_node_Table_cur_times_Field(&pConn, node_id, entity_name, string_cur_times) != 0) {
						printf("[ERROR] call Update_nms_alarm_indicators_node_Table_cur_times_Field failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;
					}
				}
				
				if(flag_insert_or_update == 0) {
					printf("[DEBUG] current value:%.2f is normal, alarm value:%.2f don't need alarm\n", number_alarm_value, limentvalue0);
				} else if(flag_insert_or_update == 1) {					

					if(update_managesign_system_eventlist_table(pConn, id, HANDLED_MANAGESIGN) != 0) {
						printf("[ERROR] call update_managesign_system_eventlist_table failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;
					}
					
					char record_time[64];
					memset(record_time, 0, 64);
					if(get_recordtime_from_system_eventlist_table(pConn, id, record_time) !=0) {
						printf("[ERROR] call get_recordtime_from_system_eventlist_table failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;
					}
					
					/* add get current alarm times from system_eventlist table */
					unsigned int alarm_times_from_system_eventlist = 0;
					if(get_alarm_times_from_system_eventlist_table(pConn, id, &alarm_times_from_system_eventlist) != 0) {
						printf("[ERROR] call get_alarm_times_from_system_eventlist_table failed in the alarm function\n");
						/* if get failed from system_eventlist table, then computer the value by cur_times*/
						if((cur_times - alarm_basic_info->time0 + 1) < 0) {
							alarm_times_from_system_eventlist = 1;
						} else {
							alarm_times_from_system_eventlist = cur_times - alarm_basic_info->time0 + 1;
						}
					}
					/* end add get current alarm times from system_eventlist table*/
					int ret = put_auto_recover_alarm_message_into_table(pConn, 
																		node_id, 
																	    alarm_basic_info->alarm_info,
																	    alarm_basic_info->subtype,
																	    entity_name_index,
																	    alarm_basic_info->name,
																	    alarm_basic_info->ip,
																	    alarm_basic_info->alias,
																	    record_time,
																	    cur_time,
																	    alarm_times_from_system_eventlist,
																		entity_alias,
																		number_alarm_value,
																		limentvalue0,
																		alarm_basic_info->threshold_unit);
					if(ret != 0) {
						printf("[ERROR] call put auto_recover_alarm_message_into_table failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;
					}
				}
				
			} else if(number_alarm_value > limentvalue1) {
			
				if(alarm_basic_info->time0 == 1) {
					level = 3;
				} else {
					level = 1;
				}
				
				for(count = 0; count < cur_times_map_nums; count++) {
					if(entity_name_index == cur_times_map[count].entity_name_index) {
						cur_times_map[count].cur_times++;
					}
				}
				if(get_string_cur_times_value_from_int_by_subentity_index(cur_times_map, 
																		  cur_times_map_nums,
																		  string_cur_times) != 0) {
					printf("[ERROR] call get_string_cur_times_value_from_int_by_subentity_index failed in the alarm function\n");
					free(alarm_basic_info);
					return 1;
				}
				if(Update_nms_alarm_indicators_node_Table_cur_times_Field(&pConn, node_id, entity_name, string_cur_times) != 0) {
					printf("[ERROR] call Update_nms_alarm_indicators_node_Table_cur_times_Field failed in the alarm function\n");
					free(alarm_basic_info);
					return 1;
				}
				
				if(flag_insert_or_update == 0) {
					cur_times++;
					if(cur_times >= max_times) {
						int ret = put_number_alarm_infos_into_table(pConn,
																	node_id, 
																	level, 
																	number_alarm_value,
																	alarm_basic_info->limenvalue0,
																	alarm_basic_info->alarm_info,
																	alarm_basic_info->subtype,
																	entity_name_index,
																	alarm_basic_info->name,
																	alarm_basic_info->ip,
																	alarm_basic_info->alias,
																	alarm_basic_info->threshold_unit,
																	entity_alias);
						if(ret != 0) {
							printf("[ERROR] call put_number_alarm_infos_into_table failed in the alarm function\n");
							free(alarm_basic_info);
							return 1;
						}
					}
				} else if(flag_insert_or_update == 1) {

					unsigned int alarm_times = 0;
					if(get_alarm_times_from_system_eventlist_table(pConn, id, &alarm_times) != 0) {
						printf("[ERROR] call get_alarm_times_from_system_eventlist_table failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;	
					}

					char content_result[256];
					memset(content_result, 0, 256);
					snprintf(content_result, 256, "%s(%s) %s%s 实时状态：%d%s，告警阀值：%d%s",
							 alarm_basic_info->alias, 
							 alarm_basic_info->ip, 
							 entity_alias,
							 alarm_basic_info->alarm_info,
							 (long)number_alarm_value,
							 alarm_basic_info->threshold_unit,
							 alarm_basic_info->limenvalue0,
							 alarm_basic_info->threshold_unit);	
							 
					if(update_system_eventlist_table_info(pConn, id, level, alarm_times + 1, content_result, cur_time) != 0) {
						printf("[ERROR] call update_system_eventlist_table_info failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;	
					}
				}
			}else if(number_alarm_value > limentvalue2) {
			
				if(alarm_basic_info->time0 == 1) {
					level = 3;
				} else {
					level = 2;
				}
				
				for(count = 0; count < cur_times_map_nums; count++) {
					if(entity_name_index == cur_times_map[count].entity_name_index) {
						cur_times_map[count].cur_times++;
					}
				}
				
				if(get_string_cur_times_value_from_int_by_subentity_index(cur_times_map, 
																		  cur_times_map_nums,
																		  string_cur_times) != 0) {
					printf("[ERROR] call get_string_cur_times_value_from_int_by_subentity_index failed in the alarm function\n");
					free(alarm_basic_info);
					return 1;
				}
				
				if(Update_nms_alarm_indicators_node_Table_cur_times_Field(&pConn, node_id, entity_name, string_cur_times) != 0) {
					printf("[ERROR] call Update_nms_alarm_indicators_node_Table_cur_times_Field failed in the alarm function\n");
					free(alarm_basic_info);
					return 1;
				}
				
				if(flag_insert_or_update == 0) {
					cur_times++;
					if(cur_times >= max_times) {
						int ret = put_number_alarm_infos_into_table(pConn,
																	node_id, 
																	level, 
																	number_alarm_value,
																	alarm_basic_info->limenvalue1,
																	alarm_basic_info->alarm_info,
																	alarm_basic_info->subtype,
																	entity_name_index,
																	alarm_basic_info->name,
																	alarm_basic_info->ip,
																	alarm_basic_info->alias,
																	alarm_basic_info->threshold_unit,
																	entity_alias);
						if(ret != 0) {
							printf("[ERROR] call put_number_alarm_infos_into_table failed in the alarm function\n");
							free(alarm_basic_info);
							return 1;
						}
					}
				} else if(flag_insert_or_update == 1) {
				
					unsigned int alarm_times = 0;
					if(get_alarm_times_from_system_eventlist_table(pConn, id, &alarm_times) != 0) {
						printf("[ERROR] call get_alarm_times_from_system_eventlist_table failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;	
					}

					char content_result[256];
					memset(content_result, 0, 256);
					snprintf(content_result, 256, "%s(%s) %s%s 实时状态：%d%s，告警阀值：%d%s",
							 alarm_basic_info->alias, 
							 alarm_basic_info->ip, 
							 entity_alias,
							 alarm_basic_info->alarm_info,
							 (long)number_alarm_value,
							 alarm_basic_info->threshold_unit,
							 alarm_basic_info->limenvalue1,
							 alarm_basic_info->threshold_unit);
							 
					if(update_system_eventlist_table_info(pConn, id, level, alarm_times + 1, content_result, cur_time) != 0) {
						printf("[ERROR] call update_system_eventlist_table_info failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;	
					}
				}
				
			} else {
			
				if(alarm_basic_info->time0 == 1) {
					level = 3;
				} else {
					level = 3;
				}			
				
				for(count = 0; count < cur_times_map_nums; count++) {
					if(entity_name_index == cur_times_map[count].entity_name_index) {
						cur_times_map[count].cur_times++;
					}
				}
				if(get_string_cur_times_value_from_int_by_subentity_index(cur_times_map, 
																		  cur_times_map_nums,
																		  string_cur_times) != 0) {
					printf("[ERROR] call get_string_cur_times_value_from_int_by_subentity_index failed in the alarm function\n");
					free(alarm_basic_info);
					return 1;
				}
				
				if(Update_nms_alarm_indicators_node_Table_cur_times_Field(&pConn, node_id, entity_name, string_cur_times) != 0) {
					printf("[ERROR] call Update_nms_alarm_indicators_node_Table_cur_times_Field failed in the alarm function\n");
					free(alarm_basic_info);
					return 1;
				}
				
				if(flag_insert_or_update == 0) {
					cur_times++;
					if(cur_times >= max_times) {
						int ret = put_number_alarm_infos_into_table(pConn,
																	node_id, 
																	level, 
																	number_alarm_value,
																	alarm_basic_info->limenvalue2,
																	alarm_basic_info->alarm_info,
																	alarm_basic_info->subtype,
																	entity_name_index,
																	alarm_basic_info->name,
																	alarm_basic_info->ip,
																	alarm_basic_info->alias,
																	alarm_basic_info->threshold_unit,
																	entity_alias);
						if(ret != 0) {
							printf("[ERROR] call put_number_alarm_infos_into_table failed in the alarm function\n");
							free(alarm_basic_info);
							return 1;
						}
					}
				} else if(flag_insert_or_update == 1) {
					unsigned int alarm_times = 0;
					if(get_alarm_times_from_system_eventlist_table(pConn, id, &alarm_times) != 0) {
						printf("[ERROR] call get_alarm_times_from_system_eventlist_table failed in the alarm function\n");
						free(alarm_basic_info);
						return 1;	
					}
					
					char content_result[256];
					memset(content_result, 0, 256);
					snprintf(content_result, 256, "%s(%s) %s%s 实时状态：%d%s，告警阀值：%d%s",
							 alarm_basic_info->alias, 
							 alarm_basic_info->ip, 
							 entity_alias,
							 alarm_basic_info->alarm_info,
							 (long)number_alarm_value,
							 alarm_basic_info->threshold_unit,
							 alarm_basic_info->limenvalue2,
							 alarm_basic_info->threshold_unit);						
					
					if(update_system_eventlist_table_info(pConn, id, level, alarm_times + 1, content_result, cur_time) != 0) {
						printf("[ERROR] call update_system_eventlist_table_info function failed\n");
						free(alarm_basic_info);
						return 1;	
					}
				}
			}
		}
	}	
	
	if(alarm_value_type == STRING_ALARM_TYPE) {
	
		if(string_alarm_value == NULL) {
			printf("[ERROR] the string_alarm_value pointer is null in the alarm function\n");
			free(alarm_basic_info);
			return 1;
		}
		if(string_old_value == NULL) {
			printf("[ERROR] the string_old_value pointer is null in the alarm function\n");
			free(alarm_basic_info);
			return 1;
		}		
			
		if(strncmp(string_alarm_value, string_old_value, 256) == 0) {
			printf("[DEBUG] current value:%s, old value:%s\n", string_alarm_value, string_old_value);
			free(alarm_basic_info);
			return 0;
		} else {
			printf("[DEBUG] current value:%s, old value:%s\n", string_alarm_value, string_old_value);
		}

		unsigned int alarm_times = 0;
		if(flag_insert_or_update == 0) {
			int ret = put_string_alarm_infos_into_table(pConn, 
														node_id, 
														alarm_basic_info->alarm_info,
														alarm_basic_info->subtype,
														entity_name_index,
														alarm_basic_info->name,
														alarm_basic_info->ip,
														alarm_basic_info->alias,
														string_alarm_value,
														string_old_value,
														entity_alias);
			if(ret != 0) {
					printf("[ERROR] call put_string_alarm_infos_into_table failed in the alarm function\n");
					free(alarm_basic_info);
					return 1;
			}
		} else if(flag_insert_or_update == 1) {
			
			if(get_alarm_times_from_system_eventlist_table(pConn, id, &alarm_times) != 0) {
				printf("[ERROR] call get_alarm_times_from_system_eventlist_table failed in the alarm function\n");
				free(alarm_basic_info);
				return 1;	
			}
			
			char content_result[256];
			memset(content_result, 0, 256);
			snprintf(content_result, 256, "%s(%s) %s%s 实时状态：%s，前置状态：%s",
									alarm_basic_info->alias, 
									alarm_basic_info->ip,
									entity_alias,									
									alarm_basic_info->alarm_info,
									string_alarm_value,
									string_old_value);		
			
			if(update_system_eventlist_table_info(pConn, id, 3, alarm_times + 1, content_result, cur_time) != 0) {
				printf("[ERROR] call update_system_eventlist_table_info function failed\n");
				free(alarm_basic_info);
				return 1;	
			}
		}
	}
	
	
	free(alarm_basic_info);
	return 0;
}

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
									  const unsigned char *entity_alias) {
									  
	if(alarm_info == NULL || subtype == NULL || subentity ==NULL
		|| ip == NULL || alias ==NULL || current_value ==NULL || old_value == NULL) {
		printf("[ERROR] the arguments is not valid in put_string_alarm_infos_into_table function\n");
		return 1;
	}

	unsigned char content_result[512];
	unsigned char eventlocation[256];
	unsigned char record_time[64];

	memset(content_result, 0, 512);
	memset(eventlocation, 0, 256);
	snprintf(eventlocation, 256, "%s(%s)", alias, ip);
	snprintf(content_result, 512, "%s(%s) %s%s 实时状态：%s，前置状态：%s", alias, ip, entity_alias, alarm_info, current_value, old_value);
		
	ALARM_RESULT_INFO alarm_result_info;
	
	get_current_time(record_time);
	
	strncpy(alarm_result_info.eventtype, EVENTTYPE_VALUE, 16);
	strncpy(alarm_result_info.eventlocation,eventlocation , 64);
	strncpy(alarm_result_info.content, content_result, 512);
	alarm_result_info.level = 3;
	alarm_result_info.mamagesign = MANAGESIGN_VALUE;
	strncpy(alarm_result_info.recordtime, record_time, 32);
	strncpy(alarm_result_info.reportman, REPORTMAN_VALUE, 16);
	alarm_result_info.node_id = node_id;
	strncpy(alarm_result_info.subtype, subtype, 32);
	alarm_result_info.subentity_index = entity_name_index;
	strncpy(alarm_result_info.subentity, subentity, 32);
	

	int ret = 0;
	ret = Alarm_Info_Insert_Into_system_eventlist_Table(&pConn, 
														alarm_result_info.eventtype,
														alarm_result_info.eventlocation,
														alarm_result_info.content, 
														alarm_result_info.level,
														alarm_result_info.mamagesign,
														alarm_result_info.recordtime,
														alarm_result_info.reportman,
														alarm_result_info.node_id,
														alarm_result_info.subtype,
														alarm_result_info.subentity_index,
														alarm_result_info.subentity,
														alarm_result_info.recordtime);
												  
	if(ret != 0) {
		printf("[ERROR] call Alarm_Info_Insert_Into_system_eventlist_Table function failed\n");
		return 1;
	}	
}



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
											 unsigned int *id_nums) {
	
	if(node_id == 0) {
		printf("[ERROR] the parameter node_id is not valid in get_id_value_from_system_eventlist_table function\n");
		return 1;
	}
	
	if(subentity == NULL) {
		printf("[ERROR] the parameter subentity value pointer is null in get_id_value_from_system_eventlist_table function\n");
		return 1;
	}
	
	int ret = 0;
	*id_value = 0;
	*id_nums = 0;
	
	ret = Get_ID_From_system_eventlist_Table(&pConn, node_id, managesign, subentity_index, subentity, id_value, id_nums);
	
	if(*id_nums > 1) {
		printf("[ERROR] get multi id values return wrong in get_id_value_from_system_eventlist_table function\n");
		return 1;
	}
	return 0;
}

/**
 * Put string alarm info into database system_eventlist table.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param id: the id value from system_eventlist.
 * @param alarm_times: the alarm_times about system_eventlist table.
 * @return 0 on success, 1 on failure.
 */
int get_alarm_times_from_system_eventlist_table(MYSQL pConn,
												const unsigned int id,
												unsigned int *alarm_times) {
	*alarm_times = 0;
	int ret = 0;
	ret = Get_alarm_times_From_system_eventlist_Table(&pConn, id, alarm_times);
	if(ret != 0) {
		printf("[ERROR] call Get_alarm_times_From_system_eventlist_Table failed in get_alarm_times_from_system_eventlist_table function\n");
		return 1;
	}
	return 0;
}

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
									   const unsigned char *cotent,
									   const unsigned char *lasttime) {
									   
	if(lasttime == NULL || lasttime == NULL) {
		printf("[ERROR] the argument pointer is null in update_system_eventlist_table_info function\n");
		return 1;
	}
	
	int ret = 0;
	ret = Alarm_Update_system_eventlist_Table(&pConn, id, alarm_times, level, cotent, lasttime);
	if(ret !=0 ) {
		printf("[ERROR] call Alarm_Update_system_eventlist_Table function failed in update_system_eventlist_table_info function\n");
		return 1;
	}
	return 0;
}


/**
 * Put string alarm info into database system_eventlist table.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param id: the id value from system_eventlist.
 * @param managesign: the managesign about system_eventlist table.
 * @return 0 on success, 1 on failure.
 */
int update_managesign_system_eventlist_table(MYSQL pConn, 
											 const unsigned int id,
											 const unsigned int managesign) {

	int ret = 0;
	
	ret = Update_system_eventlist_Table_managesign_Column(&pConn, id, managesign);
	if(ret !=0 ) {
		printf("[ERROR] call Update_system_eventlist_Table_managesign_Column failed in update_managesign_system_eventlist_table function\n");
		return 1;
	}
	return 0;
}

/**
 * Get get_recordtime_from_system_eventlist_table.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param id: the id value from system_eventlist.
 * @param recordtime: the managesign about system_eventlist table.
 * @return 0 on success, 1 on failure.
 */
int get_recordtime_from_system_eventlist_table(MYSQL pConn, const unsigned int id, unsigned char *recordtime) {
	
	if(recordtime == NULL) {
		printf("[ERROR] the argument is null in get_recordtime_from_system_eventlist_table function\n");
	}
	int ret = 0;
	ret = Get_recordtime_From_system_eventlist_Table(&pConn, id, recordtime);
	if(ret !=0 ) {
		printf("[ERROR] call Get_recordtime_From_system_eventlist_Table failed in get_recordtime_from_system_eventlist_table function\n");
		return 1;
	}
	return 0;
}


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
											  const unsigned char *threshold_unit) {
									  
	if(alarm_info == NULL || subtype == NULL || subentity == NULL || ip == NULL || 
		alias == NULL || starttime == NULL || lasttime == NULL || entity_alias == NULL) {
		printf("[ERROR] the arguments is null in put auto recover_alarm_message_into_table function\n");
		return 1;
	}

	unsigned char content_result[512];
	unsigned char eventlocation[256];
	unsigned char record_time[64];

	memset(content_result, 0, 512);
	memset(eventlocation, 0, 256);
	snprintf(eventlocation, 256, "%s(%s)", alias, ip);
	snprintf(content_result, 512, "%s(%s) %s%s自动恢复正常 当前值：%.0f%s，告警值：%.0f%s 开始时间：%s，结束时间：%s", 
															  alias, 
															  ip,
															  entity_alias,
															  alarm_info,
															  cur_value,
															  threshold_unit,
															  limit0,
															  threshold_unit,
															  starttime,
															  lasttime);
	
	ALARM_RESULT_INFO alarm_result_info;
	
	strncpy(alarm_result_info.eventtype, EVENTTYPE_VALUE, 16);
	strncpy(alarm_result_info.eventlocation, eventlocation , 64);
	strncpy(alarm_result_info.content, content_result, 512);
	alarm_result_info.level = 0;
	alarm_result_info.mamagesign = HANDLED_MANAGESIGN;
	strncpy(alarm_result_info.reportman, REPORTMAN_VALUE, 16);
	alarm_result_info.node_id = node_id;
	strncpy(alarm_result_info.subtype, subtype, 32);
	alarm_result_info.subentity_index = subentity_index;
	strncpy(alarm_result_info.subentity, subentity, 32);
	
	int ret = 0;
	ret = Auto_Recover_Alarm_Info_Insert_Into_system_eventlist_Table(&pConn, 
																	  alarm_result_info.eventtype,
																	  alarm_result_info.eventlocation,
																	  alarm_result_info.content, 
																	  alarm_result_info.level,
																	  alarm_result_info.mamagesign,
																	  starttime,
																	  lasttime,
																	  alarm_result_info.reportman,
																	  alarm_result_info.node_id,
																	  alarm_result_info.subtype,
																	  alarm_result_info.subentity_index,
																	  alarm_result_info.subentity,
																	  alarm_times);
	if(ret != 0) {
		printf("[ERROR] call Auto_Recover_Alarm_Info_Insert_Into_system_eventlist_Table failed in put auto recover_alarm_message_into_table function\n");
		return 1;
	}	
}


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
														   unsigned int *nums) {
	if(string_value == NULL) {
		printf("[ERROR] the argument string_value pointer is null in get_int_cur_times_value_from_string_by_subentity_index function\n");
		return 1;
	}
	*nums = 0;
	unsigned cur_loc = 0;
	unsigned end_loc = 0;
	unsigned int cur_string_loc = 0;
	unsigned string_length = 0;
	unsigned char cur_string[255];
	unsigned int count = 0;
	string_length = strlen(string_value);
	
	while(cur_loc < string_length) {
	
		while(cur_loc < string_length && string_value[cur_loc] != '{') {
			cur_loc++;
		}
		cur_loc++;
		end_loc = cur_loc;
	
		while(end_loc < string_length && string_value[end_loc] != ',') {
			end_loc++;
		}
		
		memset(cur_string, 0, 255);
		cur_string_loc = 0;
		while(cur_loc < end_loc) {
			if(!(string_value[cur_loc] >= '0' && string_value[cur_loc] <= '9')) {
				printf("[ERROR] the string_value have not in 0 to 9 char in get_int_cur_times_value_from_string_by_subentity_index function\n");
				return 1;
			}
			cur_string[cur_string_loc++] = string_value[cur_loc++];
		}
		cur_times_map[count].entity_name_index = atoi(cur_string);
		
		end_loc++;
		cur_loc = end_loc;
		while(end_loc < string_length && string_value[end_loc] != '}') {
			end_loc++;
		}
		memset(cur_string, 0, 255);
		cur_string_loc = 0;
		while(cur_loc < end_loc) {
			if(!(string_value[cur_loc] >= '0' && string_value[cur_loc] <= '9')) {
				printf("[ERROR] the string_value have not in 0 to 9 char in get_int_cur_times_value_from_string_by_subentity_index function\n");
				return 1;
			}
			cur_string[cur_string_loc++] = string_value[cur_loc++];
		}
;
		cur_times_map[count].cur_times = atoi(cur_string);
		count++;
	}
	if(count == 0) {
		*nums = 0;
	} else {
		*nums = count - 1;
	}
	return 0;
}


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
														   unsigned char *string_cur_times) {
	if(string_cur_times == NULL) {
		printf("[ERROR] the argument string_cur_times pointer is null in get_string_cur_times_value_from_int_by_subentity_index function\n");
		return 1;
	}
	memset(string_cur_times, 0, 255);
	unsigned char tmp[255];
	unsigned int count = 0;
	
	for(count = 0; count < cur_times_map_len; count++) {
		memset(tmp, 0, 255);
		snprintf(tmp, 255, "{%lu,%lu}", cur_times_map[count].entity_name_index, cur_times_map[count].cur_times);
		strncat(string_cur_times, tmp, 255);
	}
	
	return 0;
}



/**
 * Start alarm the snmp exception.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param pConn: the database connect handle.
 * @param node_id: the device nodeid from topo_host_node.
 * @param snmp_alarm_string: the alarm string value.
 * @return 0 on success, 1 on failure.
 */
int nms_snmp_exception_alarm(MYSQL pConn, 
							 unsigned int node_id, 
							 const unsigned char *snmp_alarm_string) {
	
	char ip[32];
	char alias[128];
	memset(ip, 0, 32);
	memset(alias, 0, 128);
	
	if(Get_IP_And_Alias_From_topo_host_node_Table(&pConn, node_id, ip, alias) != 0){
		printf("[ERROR] call the Get_IP_And_Alias_From_topo_host_node_Table failed in the nms_snmp_exception_alarm\n");
		return 1;
	}
	
	unsigned int id_from_system_eventlist = 0;
	unsigned int id_nums = 0;
	if(Get_ID_From_system_eventlist_Table(&pConn, node_id, 0, 0, "snmpexception", &id_from_system_eventlist, &id_nums) != 0) {
		printf("[ERROR] call Get_ID_From_system_eventlist_Table failed in the nms_snmp_exception_alarm\n");
		return 1;
	}
	

	if(id_nums == 0) {

		if(put_snmp_alarm_infos_into_table(pConn, node_id, ip, alias, snmp_alarm_string) != 0 ) {
			printf("[ERROR] call put_snmp_alarm_infos_into_table failed in the nms_snmp_exception_alarm\n");
			return 1;
		}

	}

	if(id_nums == 1) {

		unsigned int current_alarm_times = 0;
		unsigned char lasttime[64];
		memset(lasttime, 0, 64);
		
		
		if(get_alarm_times_from_system_eventlist_table(pConn, id_from_system_eventlist, &current_alarm_times) != 0) {
			printf("[ERROR] call get_alarm_times_from_system_eventlist_table failed in the nms_snmp_exception_alarm\n");
			return 1;	
		}

		get_current_time(lasttime);

		if(update_snmp_alarm_infos_into_table(pConn, id_from_system_eventlist, current_alarm_times + 1, lasttime) != 0) {
			printf("[ERROR] call update_snmp_alarm_infos_into_table failed in nms_snmp_exception_alarm\n");
			return 1;
		}
	}
	
	return 0;
}



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
									const unsigned char *snmp_alarm_string) {
	
	unsigned char content_result[256];
	unsigned char eventlocation[128];
	unsigned char record_time[64];

	
	memset(content_result, 0, 256);
	memset(eventlocation, 0, 128);
	snprintf(eventlocation, 128, "%s(%s)", alias, ip);
	snprintf(content_result, 256, "%s(%s) %s", alias, ip, snmp_alarm_string);
		
	ALARM_RESULT_INFO alarm_result_info;
	
	get_current_time(record_time);
	
	
	strncpy(alarm_result_info.eventtype, EVENTTYPE_VALUE, 16);
	strncpy(alarm_result_info.eventlocation,eventlocation , 64);
	strncpy(alarm_result_info.content, content_result, 256);
	alarm_result_info.level = 3;
	alarm_result_info.mamagesign = MANAGESIGN_VALUE;
	strncpy(alarm_result_info.recordtime, record_time, 32);
	strncpy(alarm_result_info.reportman, REPORTMAN_VALUE, 16);
	alarm_result_info.node_id = node_id;
	strncpy(alarm_result_info.subtype, "", 32);
	alarm_result_info.subentity_index = 0;
	strncpy(alarm_result_info.subentity, "snmpexception", 32);
	
	int ret = 0;
	
	
	ret = Alarm_Info_Insert_Into_system_eventlist_Table(&pConn, 
														alarm_result_info.eventtype,
														alarm_result_info.eventlocation,
														alarm_result_info.content, 
														alarm_result_info.level,
														alarm_result_info.mamagesign,
														alarm_result_info.recordtime,
														alarm_result_info.reportman,
														alarm_result_info.node_id,
														alarm_result_info.subtype,
														alarm_result_info.subentity_index,
														alarm_result_info.subentity,
														alarm_result_info.recordtime);

	
	if(ret != 0) {
		printf("[ERROR] call Alarm_Info_Insert_Into_system_eventlist_Table function failed\n");
		return 1;
	}
	
	return 0;
}


/**
 * Update snmp alarm exception into database.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param pConn: the database connect handle.
 * @param node_id: the device nodeid from topo_host_node.
 * @param snmp_alarm_string: the alarm string value.
 * @return 0 on success, 1 on failure.
 */
int update_snmp_alarm_infos_into_table(MYSQL pConn, unsigned int id, unsigned int current_alarm_times, char *lasttime) {
	
	if(Update_Snmp_Alarm_Infos_Into_System_eventlist(&pConn, id, current_alarm_times, lasttime) != 0) {
		printf("[ERROR] call Update_Snmp_Alarm_Infos_Into_System_eventlist failed in update_snmp_alarm_infos_into_table\n");
		return 1;
	}
	
	return 0;
}



/**
 * Start alarm the snmp recovery.
 * Author: lining 15810423651 lining01@iie.ac.cn 
 * @param pConn: the database connect handle.
 * @param node_id: the device nodeid from topo_host_node.
 * @param snmp_alarm_string: the alarm string value.
 * @return 0 on success, 1 on failure.
 */
int nms_snmp_recovery_alarm(MYSQL pConn, unsigned int node_id) {
	
	unsigned int id_from_system_eventlist = 0;
	unsigned int id_nums = 0;
	if(Get_ID_From_system_eventlist_Table(&pConn, node_id, 0, 0, "snmpexception", &id_from_system_eventlist, &id_nums) != 0) {
		printf("[ERROR] call Get_ID_From_system_eventlist_Table failed in the nms_snmp_exception_alarm\n");
		return 1;
	}
	

	if(id_nums == 0) {
		printf("[DEBUG] there is no alarm snmpexception in nms_snmp_recovery_alarm function\n");
		return 0;
	}

	if(id_nums == 1) {
		if(update_managesign_system_eventlist_table(pConn, id_from_system_eventlist, HANDLED_MANAGESIGN) != 0) {
			printf("[ERROR] call update_managesign_system_eventlist_table failed in the alarm function\n");
			return 1;
		}
	}
	
	return 0;
}





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
										unsigned int *alarm_basic_conf_info_nums) {
	
	unsigned int ret = 0;
	*alarm_basic_conf_info_nums = 0;
	ret = Get_Info_From_nms_alarm_indicators_node_conf_Table(&pConn, 
															 nodeid, 
															 indexid,
															 indicator_type,
															 collector_type,
															 p_alarm_basic_conf_info, 
															 alarm_basic_conf_info_nums);
	if(ret != 0) {
		printf("[ERROR] call Get_Info_From_nms_alarm_indicators_node_conf_Table failed\n");
		return 1;
	}
	
	return 0;
}
