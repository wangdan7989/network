
#include "net-snmp-config.h"
#include "net-snmp-includes.h"
#include "utilities.h"


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>
#include <time.h>
#include <sys/select.h>
#include <netdb.h>
#include <arpa/inet.h>


#define DEBUG_PRINT_MY_SNMP_GETS
#undef  DEBUG_PRINT_MY_SNMP_GETS




#ifndef MY_SNMP_GETS_H
#define MY_SNMP_GETS_H


#ifndef MY_SNMP_VAL_H
#define MY_SNMP_VAL_H
typedef union {
	long	integer;
	u_char	string[256];
	u_char	bitstring[256];
	long	objid[256];
	float	floatVal;
	double	doubleVal;
	struct	counter64 *counter64;
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

#ifndef GETS_OID_VALUES_H
#define GETS_OID_VALUES_H
void gets_oid_values(netsnmp_variable_list *vars, my_oid_result *oid_result);
#endif

int my_snmp_gets(const char *peername, const char *community, const char *oid_argvs[], const unsigned int oid_argvs_nums, my_oid_result oids_result[]);



#endif
