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


#ifndef MY_SNMP_GET_H
#define MY_SNMP_GET_H



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


void get_oid_value(netsnmp_variable_list *vars, my_oid_result **oid_result);
int my_snmp_get(const char *peername, const char *commuity, const char *oid_argv, my_oid_result *oid_result);



#endif
