#include "my_snmp_get.h"


/**
 * Get snmp value to the structure my_poid_result->
 * Author : lining 15810423651 lining01@iie.ac.cn 
 * @param netsnmp_variable_list *vars  : the return pdu variable value.
 * @param my_oid_result : the structure my_poid_result->
 * @return void.
 */
void get_oid_value(netsnmp_variable_list *vars, my_oid_result **oid_result) {
	
	unsigned int count = 0;
	
	(*oid_result)->oid_name_length = vars->name_length;	
	
	for(count=0; count < vars->name_length; count++) {
		(*oid_result)->oid_name[count]	= *(vars->name+count);
	}
	(*oid_result)->val_len = vars->val_len;
	
	(*oid_result)->type = vars->type;

	if(vars->type == SNMP_NOSUCHOBJECT) {
		return;
	}
	
	if(vars->type == SNMP_NOSUCHINSTANCE) {
		return;
	}
	
	if(vars->type == SNMP_ENDOFMIBVIEW) {
		return;
	}
	
	if (vars->type == ASN_OCTET_STR) { 

		int hex = 0;
		int x;
		u_char * cp;
		int allow_realloc = 1;
		u_char *buf = NULL;
		size_t buf_len = 256, out_len = 0;

		for (cp = vars->val.string, x = 0; x < (int)vars->val_len; x++, cp++) {
			if (!isprint(*cp) && !isspace(*cp)) {
				hex = 1;
			}
		}
		
		if(!hex) {
			for(count=0; count < vars->val_len; count++) {
				(*oid_result)->val.string[count] = vars->val.string[count];
			}
			return;
			
		} else {
			buf = (u_char *) calloc(buf_len, 1);
			sprint_realloc_hexstring(&buf, &buf_len, &out_len, allow_realloc, vars->val.string, vars->val_len);
			
			for(count=0; count < strlen(buf); count++) {
				(*oid_result)->val.string[count] = buf[count];
			}
			(*oid_result)->val.string[strlen(buf)] = 0;
			
			free(buf);
			return;
		}

	}

	if (vars->type == ASN_BIT_STR) {
		sprintf((*oid_result)->val.bitstring, "%s" ,vars->val.bitstring);
		return;
	}
	
	if (vars->type == ASN_OPAQUE) {
		return;
	}
	
	if (vars->type == ASN_INTEGER) {
		(*oid_result)->val.integer = *vars->val.integer;
		(*oid_result)->val.integer = (*oid_result)->val.integer & 0x00000000ffffffff;
		return;
	}
	
	if (vars->type == ASN_UINTEGER) {
		(*oid_result)->val.integer = *vars->val.integer;
		(*oid_result)->val.integer = (*oid_result)->val.integer & 0x00000000ffffffff;
		return;
	}
	
	if (vars->type == ASN_OBJECT_ID) {
		(*oid_result)->objid_length = vars->val_len/sizeof(oid);
		for(count=0; count < (*oid_result)->objid_length; count++) {
			(*oid_result)->val.objid[count] = vars->val.objid[count];
		}
		return;
	}
	
	if (vars->type == ASN_TIMETICKS) {
		(*oid_result)->val.integer = *(u_long *)vars->val.integer;
		return;
	}
	
	if (vars->type == ASN_IPADDRESS) {
		(*oid_result)->val.string[0] = vars->val.string[0];
		(*oid_result)->val.string[1] = vars->val.string[1];
		(*oid_result)->val.string[2] = vars->val.string[2];
		(*oid_result)->val.string[3] = vars->val.string[3];
		return;
	}
	
	if (vars->type == ASN_OPAQUE_I64) {
		printI64((*oid_result)->val.string, vars->val.counter64);
		return;
	}
	
	if (vars->type == ASN_OPAQUE_COUNTER64 || vars->type == ASN_OPAQUE_U64 || vars->type == ASN_COUNTER64) {
		printU64((*oid_result)->val.string, vars->val.counter64);
		return;
	}

	if (vars->type == ASN_COUNTER) {
		(*oid_result)->val.integer = *vars->val.integer;
		return;
	}
	
	if (vars->type == ASN_GAUGE) {
		(*oid_result)->val.integer = *vars->val.integer;
		return;
	}
	
	if (vars->type == ASN_OPAQUE_FLOAT) {
		(*oid_result)->val.floatVal = *vars->val.floatVal;
		return;
	}
	
	if (vars->type == ASN_OPAQUE_DOUBLE) {
		(*oid_result)->val.doubleVal = *vars->val.doubleVal;
		return;
	}
	
	if (vars->type == ASN_NULL) {
		return;
	}
	
	return;
}

/**
 * print the my_get_snmps function return error 1 ~ 5 string.
 * Author : lining 15810423651 lining01@iie.ac.cn 
 * @param ratval  : the my_get_snmps function return value.
 * @return no return.
 */
void my_snmp_get_perror(unsigned int ratval) {
	switch (ratval) {
		case 1:
			printf("[ERROR] my_snmp_get: the argument value oid_names is empty\n");
			break;
		case 2:
			printf("[ERROR] my_snmp_get: the snmp_open function run failed\n");
			break;
		case 3:
			printf("[ERROR] my_snmp_get: the snmp_parse_oid function run failed\n");
			break;
		case 4:
			printf("[ERROR] my_snmp_get: call snmp time out\n");
			break;	
		case 5:
			printf("[ERROR] my_snmp_get: snmpget sesson_handls error\n");
			break;
		default :
			printf("[ERROR] my_snmp_get: no %d error type return value\n", ratval);
	}
	
}
/**
 * Get snmp value to the structure my_oid_result array.
 * Author : lining 15810423651 lining01@iie.ac.cn 
 * @param peername  : the remote host ip address.
 * @param oid_argvs : the oids pointer array.
 * @param my_oid_result oid_result : the return values. 
 * @return 0 on success, or others on failure.
 * @return 1 on failure, the argument value error.
 * @return 2 on failure, snmp_open return error.
 * @return 3 on failure, snmp_parse_oid return error.
 * @return 4 on failure, snmp_synch_response return status time out.
 * @return 5 on failure, snmp_synch_response return status others.
 */
int my_snmp_get(const char *peername, 
				const char *community, 
				const char *oid_argv, 
				my_oid_result *oid_result) {
	
	netsnmp_session	session;
	netsnmp_session *sess_handle;
	netsnmp_pdu	*pdu;
	netsnmp_pdu	*response;
	netsnmp_variable_list	*vars;
	
	oid oids[MAX_OID_LEN];
    size_t	oids_length;

    int	status;
    int	failures = 0;
	oids_length = 0;
	
	if (peername == NULL) {
		printf("[ERROR] my_snmp_get: the peername pointer is null\n");
		return 1;
	}
	if (community == NULL) {
		printf("[ERROR] my_snmp_get: the community pointer is null\n");
		return 1;
	}
	if (oid_argv == NULL) {
		printf("[ERROR] my_snmp_get: the oid_argv pointer is null\n");
		return 1;
	}
	
	init_snmp("snmpget");
	snmp_sess_init(&session);
	session.version = SNMP_VERSION_2c;
	session.community = (char*)community;
	session.community_len = strlen(session.community);
	session.peername = (char*)peername;
	session.timeout = 3000000;
	session.retries = 1;
	
    SOCK_STARTUP;

    sess_handle = snmp_open(&session);
    if (sess_handle == NULL) {
        SOCK_CLEANUP;
		printf("[ERROR] my_snmp_get: call snmp_open function failed, return sess_handle is null\n");
        return 2;
    }

    pdu = snmp_pdu_create(SNMP_MSG_GET);

	oids_length = MAX_OID_LEN;

	if (!snmp_parse_oid(oid_argv, oids, &oids_length)) {
		snmp_perror(oid_argv);
		failures++;
	} else {
		snmp_add_null_var(pdu, oids, oids_length);
	}
    if (failures) {
        SOCK_CLEANUP;
		printf("[ERROR] my_snmp_get: call snmp_parse_oid function failed\n");
        return 3;
    }
	
    status = snmp_synch_response(sess_handle, pdu, &response);
	
    if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
	
		for (vars = response->variables; vars; vars = vars->next_variable) {
			get_oid_value(vars, &oid_result);
		}
		
    } else if (status == STAT_TIMEOUT) {
		printf("[ERROR] my_snmp_get: call snmp_synch_response function failed, return status time out\n");
        return 4;
    } else {
		printf("[ERROR] my_snmp_get: call snmp_synch_response function failed, return status others\n");
        return 5;
    }

    if (response) {
        snmp_free_pdu(response);
	}
    snmp_close(sess_handle);
    SOCK_CLEANUP;
	
    return 0;
}

/*
int main(int argc, char *argv[]) {
	
	
	char *peername = NULL;
	char *commucity = NULL;
	const char *oid_name;
	unsigned int count = 0;
	my_oid_result *poid_result = NULL;
	
	poid_result = (struct my_oid_result *)malloc(sizeof(struct my_oid_result));
	

	peername = argv[1];
	oid_name = argv[2];
	commucity = argv[3];


	int ral = my_snmp_get(peername, commucity, oid_name, poid_result);
	
	if(ral == 4 || ral == 5) {
		printf("[main] timeout or other errors\n");
	}
	
	if(ral == 0) {
		if(poid_result->type == SNMP_NOSUCHOBJECT) {
			printf("[main] No Such Object available on this agent at this OID\n");
		}
	
		if(poid_result->type == SNMP_NOSUCHINSTANCE) {
			printf("[main] No Such Instance currently exists at this OID\n");
		}

		if(poid_result->type == SNMP_ENDOFMIBVIEW) {
			printf("[main] No more variables left in this MIB View (It is past the end of the MIB tree)\n");
		}

		if(poid_result->type == ASN_OCTET_STR || poid_result->type == ASN_BIT_STR) {
			printf("%s\n", poid_result->val.string);
		}
			
		if(poid_result->type == ASN_INTEGER) {
			printf("%ld\n", poid_result->val.integer);
		}
			
		if(poid_result->type == ASN_UINTEGER) {
			printf("%lu\n", poid_result->val.integer);
		}
			
		if(poid_result->type == ASN_OBJECT_ID) {
			for(count=0; count<poid_result->objid_length; count++) {
				printf("%d", poid_result->val.objid[count]);
				count < (poid_result->objid_length-1) ? printf(".") : printf("\n");
			}
		}
			
		if( poid_result->type == ASN_TIMETICKS || 
			poid_result->type == ASN_GAUGE || 
			poid_result->type == ASN_COUNTER ) {
			printf("%lu\n", poid_result->val.integer);
		}
			
		if(poid_result->type == ASN_OPAQUE_FLOAT) {
			printf("%f\n", poid_result->val.floatVal);
		}
			
		if(poid_result->type == ASN_OPAQUE_DOUBLE) {
			printf("%f\n", poid_result->val.doubleVal);
		}
			
		if (poid_result->type == ASN_OPAQUE_COUNTER64 || 
			poid_result->type == ASN_OPAQUE_U64 || 
			poid_result->type == ASN_COUNTER64 || 
			poid_result->type == ASN_OPAQUE_I64 ) 
		{
			printf("%s\n", poid_result->val.string);
		}
			
		if(poid_result->type == ASN_IPADDRESS) {
			printf("%d.%d.%d.%d\n", 
				poid_result->val.string[0], 
				poid_result->val.string[1],
				poid_result->val.string[2],
				poid_result->val.string[3] 
			);
		}
	}
	
	free(poid_result);
	return 0;
}
*/
