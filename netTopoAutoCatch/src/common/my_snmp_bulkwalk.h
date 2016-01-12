
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>
#include <sys/time.h>
#include <time.h>
#include <sys/select.h>
#include <stdio.h>
#include <ctype.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "net-snmp-config.h"
#include "net-snmp-includes.h"
#include "utilities.h"


#define NETSNMP_DS_WALK_INCLUDE_REQUESTED	   		1
#define NETSNMP_DS_WALK_PRINT_STATISTICS			2
#define NETSNMP_DS_WALK_DONT_CHECK_LEXICOGRAPHIC	3


#ifndef MY_SNMP_BULKWALK_H
#define MY_SNMP_BULKWALK_H

#ifndef MY_SNMP_VAL_H
#define MY_SNMP_VAL_H


typedef union {
	long	integer;
	
	u_char	string[256];
	u_char	bitstring[256];
	long	objid[256];
	
	float	floatVal;
	double	doubleVal;
	struct	counter64 *counter64;//表示一个非负的整数，最大值为64位-1
}my_snmp_val;

#endif

#ifndef MY_OID_RESULT_H
#define MY_OID_RESULT_H

typedef struct my_oid_result {
	unsigned long	oid_name[128];   
	size_t	oid_name_length;
	size_t	objid_length;
	u_char	type;   
	size_t	val_len;
	my_snmp_val val;
}my_oid_result;
#endif


#ifndef GET_BULKWALK_OID_VALUES_H
#define GET_BULKWALK_OID_VALUES_H
void get_bulkwalk_oid_values(netsnmp_variable_list *vars, my_oid_result *oid_result);
#endif

void my_snmp_bulkwalk_perror(unsigned int ratval);
int  my_snmp_bulkwalk(const char *peername, const char *comity, const char *oid_argv, my_oid_result oid_results[], unsigned int *oid_results_nums);


#endif
