#include "print_log.h"

void get_current_log_time(char *current_time) {
	
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

void print_log(const char *log_str, const char *collector_name) {

	if(log_str == NULL || collector_name == NULL) {
		fprintf(stderr, "[ERROR] the argument value pointer is null in print_log function\n");
		return;
	}
	
	time_t now_time;
	time(&now_time);
	struct tm *p;
	p = localtime(&now_time);
	
	char tmp[8];
	char log_file_name[32];
	char log_file_path[256];
	memset(log_file_path, 0, 256);
	memset(log_file_name, 0, 32);
	
	sprintf(log_file_name, "%d", p->tm_year + 1900);
	
	memset(tmp, 0, 8);
	if(p->tm_mon + 1 < 10) {
		snprintf(tmp, 8, "0%d", p->tm_mon + 1);
	} else {
		snprintf(tmp, 8, "%d", p->tm_mon + 1);
	}
	strncat(log_file_name, tmp, 32);
	
	memset(tmp, 0, 8);
	if(p->tm_mday < 10) {
		snprintf(tmp, 8, "0%d", p->tm_mday);
	} else {
		snprintf(tmp, 8, "%d", p->tm_mday);
	}
	strncat(log_file_name, tmp, 32);
	snprintf(log_file_path, 256, "%s/%s/%s.log", COLLECTOR_LOG_PATH, collector_name, log_file_name);

	FILE *fp = NULL;
	if((fp = fopen(log_file_path, "a+")) == NULL) {
		fprintf(stderr, "error: the \"%s\" file open failed in print_log function\n", log_file_path);
		return;
	}
	char cur_time[64];
	memset(cur_time, 0, 64);
	get_current_log_time(cur_time);
	fprintf(fp, "[%s] %s", cur_time, log_str);
	fclose(fp);
}

