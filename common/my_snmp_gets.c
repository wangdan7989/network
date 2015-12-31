#include "my_snmp_gets.h"


/**
 * Get snmp value to the structure my_oid_result.
 * Author : lining 15810423651 lining01@iie.ac.cn 
 * @param netsnmp_variable_list *vars  : the return pdu variable value.
 * @param my_oid_result : the structure my_oid_result.
 * @return void.
 */
void gets_oid_values(netsnmp_variable_list *vars, my_oid_result *oid_result) {
	
	unsigned int count = 0;

	oid_result->oid_name_length = vars->name_length;

	for(count=0; count < vars->name_length; count++) {
		oid_result->oid_name[count]	= *(vars->name+count);
	}
	
	oid_result->val_len = vars->val_len;
	
	oid_result->type = vars->type;
	
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
		for(count=0; count < vars->val_len; count++) {
			oid_result->val.string[count] = vars->val.string[count];
		}
		return;
	}

	if (vars->type == ASN_BIT_STR) {
		sprintf(oid_result->val.bitstring, "%s" ,vars->val.bitstring);
		return;
	}
	
	if (vars->type == ASN_OPAQUE) {
		return;
	}
	
	if (vars->type == ASN_INTEGER) {
		oid_result->val.integer = *vars->val.integer;
		oid_result->val.integer = oid_result->val.integer & 0x00000000ffffffff;
		return;
	}
	
	if (vars->type == ASN_UINTEGER) {
		oid_result->val.integer = *vars->val.integer;
		oid_result->val.integer = oid_result->val.integer & 0x00000000ffffffff;
		return;
	}
	
	if (vars->type == ASN_OBJECT_ID) {
		oid_result->objid_length = vars->val_len/sizeof(oid);
		for(count=0; count < oid_result->objid_length; count++) {
			oid_result->val.objid[count] = vars->val.objid[count];
		}
		return;
	}
	
	if (vars->type == ASN_TIMETICKS) {
		oid_result->val.integer = *(u_long *)vars->val.integer;
		return;
	}
	
	if (vars->type == ASN_IPADDRESS) {
		oid_result->val.string[0] = vars->val.string[0];
		oid_result->val.string[1] = vars->val.string[1];
		oid_result->val.string[2] = vars->val.string[2];
		oid_result->val.string[3] = vars->val.string[3];
		return;
	}
	
	if (vars->type == ASN_OPAQUE_I64) {
		printI64(oid_result->val.string, vars->val.counter64);
		return;
	}

	if (vars->type == ASN_OPAQUE_COUNTER64 || vars->type == ASN_OPAQUE_U64 || vars->type == ASN_COUNTER64) {
		printU64(oid_result->val.string, vars->val.counter64);
		return;
	}

	if (vars->type == ASN_COUNTER) {
		oid_result->val.integer = *vars->val.integer;
		return;
	}
	
	if (vars->type == ASN_GAUGE) {
		oid_result->val.integer = *vars->val.integer;
		return;
	}
	
	if (vars->type == ASN_OPAQUE_FLOAT) {
		oid_result->val.floatVal = *vars->val.floatVal;
		return;
	}
	
	if (vars->type == ASN_OPAQUE_DOUBLE) {
		oid_result->val.doubleVal = *vars->val.doubleVal;
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
void my_snmp_gets_perror(unsigned int ratval) {
	switch (ratval) {
		case 1:
			printf("[ERROR] my_snmp_gets: the argument oid_names is empty\n");
			break;
		case 2:
			printf("[ERROR] my_snmp_gets: the snmp_open function run failed\n");
			break;
		case 3:
			printf("[ERROR] my_snmp_gets: the snmp_parse_oid function run failed\n");
			break;
		case 4:
			printf("[ERROR] my_snmp_gets: time out\n");
			break;	
		case 5:
			printf("[ERROR] my_snmp_gets: snmpget sesson_handls error\n");
			break;
		default :
			printf("[ERROR] my_snmp_gets: no %d error type return value\n", ratval);
	}
}

/**
 * Get snmp value to the structure my_oid_result array.
 * Author : lining 15810423651 lining01@iie.ac.cn 
 * @param peername  : the remote host ip address.
 * @param oid_argvs : the oids pointer array.
 * @param oid_argvs_nums :  the oids pointer numbers.
 * @param my_oid_result oids_result[] : the return values. 
 * @return 0 on success, or others on failure.
 * @return 1 on failure, the argument value error.
 * @return 2 on failure, snmp_open return error.
 * @return 3 on failure, snmp_parse_oid return error.
 * @return 4 on failure, snmp_synch_response return status time out.
 * @return 5 on failure, snmp_synch_response return status others.
 * @return 6 on failure, snmp_synch_response wrong object for get process.
 */
int my_snmp_gets(const char *peername, 
				 const char *community, 
				 const char *oid_argvs[], 
				 const unsigned int oid_argvs_nums, 
				 my_oid_result oids_result[] ) {
	
	netsnmp_session	session;
	netsnmp_session *sess_handle;
	netsnmp_pdu	*pdu;
	netsnmp_pdu	*response;
	netsnmp_variable_list	*vars;
	
	oid oids[MAX_OID_LEN];

    size_t	oids_length;
	
    int	count;
    int	status;
    int	failures = 0;
	oids_length = 0;
	
	if (peername == NULL) {
		printf("[ERROR] my_snmp_gets: the argument peername pointer is null\n");
		return 1;
	}
	if (community == NULL) {
		printf("[ERROR] my_snmp_gets: the argument community pointer is null\n");
		return 1;
	}
	if (oid_argvs == NULL) {
		printf("[ERROR] my_snmp_gets: the argument oid_argvs pointer is null\n");
		return 1;
	}

	init_snmp("APC Check");
	snmp_sess_init(&session);
	session.version = SNMP_VERSION_2c;
	session.community = (char*)community;
	session.community_len = strlen(session.community);
	session.peername = (char*)peername;
	session.timeout = 2000000;
	session.retries = 2;
	
    SOCK_STARTUP;

    sess_handle = snmp_open(&session);
    if (sess_handle == NULL) {
		snmp_close(sess_handle);
        SOCK_CLEANUP;
		printf("[ERROR] my_snmp_gets: call snmp_open function failed\n");
        return 2;
    }

    pdu = snmp_pdu_create(SNMP_MSG_GET);
	
    for (count = 0; count<oid_argvs_nums; count++) {
        oids_length = MAX_OID_LEN;
        if(!snmp_parse_oid(oid_argvs[count], oids, &oids_length)) {
            snmp_perror(oid_argvs[count]);
            failures++;
        } else {
            snmp_add_null_var(pdu, oids, oids_length);
		}
    }
	
    if (failures) {
		if (response) {
			snmp_free_pdu(response);
		}
		snmp_close(sess_handle);
        SOCK_CLEANUP;
		printf("[ERROR] my_snmp_gets: call snmp_parse_oid function failed\n");
        return 3;
    }
	
	
	size_t oids_count = 0;
    status = snmp_synch_response(sess_handle, pdu, &response);
    if (status == STAT_SUCCESS) {
	
		if(response->errstat == SNMP_ERR_NOERROR) {
			for (vars = response->variables; vars; vars = vars->next_variable) {
				gets_oid_values(vars, &oids_result[oids_count]);
				oids_count++;		
			}
		} else {
			/* wrong object */
			printf("[ERROR] my_snmp_gets: response status is error, status: %d\n", status);
			if (response) {
				snmp_free_pdu(response);
			}
			snmp_close(sess_handle);
			SOCK_CLEANUP;
			return 6;
		}
		
    } else if (status == STAT_TIMEOUT) {
		printf("[ERROR] my_snmp_gets: return status is time out\n");
		if (response) {
			snmp_free_pdu(response);
		}
		snmp_close(sess_handle);
		SOCK_CLEANUP;
        return 4;
    } else {
		printf("[ERROR] my_snmp_gets: return status is others\n");
		if (response) {
			snmp_free_pdu(response);
		}
		snmp_close(sess_handle);
		SOCK_CLEANUP;
        return 5;
    }

    if (response) {
        snmp_free_pdu(response);
	}
    snmp_close(sess_handle);
    SOCK_CLEANUP;
    return 0;
}

#ifdef DEBUG_PRINT_MY_SNMP_GETS
int main(int argc, char *argv[]) {
	
	char *peername = NULL;
	char *community = NULL;
	const char *oids_names[SNMP_MAX_CMDLINE_OIDS];
	unsigned int oids_name_length =0;
	unsigned int count = 0;
	
	if(argc <= 3) {
		printf("please input the peername , oid_values and community\n");
		return 1;
	}
	peername = argv[1];
	community = argv[2];
	for(count=3; count<argc; count++) {
		oids_names[oids_name_length++] = argv[count];
	}
	
	my_oid_result oids_result[1024];
	
	
	int ral = my_snmp_gets(peername, community, oids_names, oids_name_length, oids_result);
	
	if(ral == 4) {
		printf("time out\n");
		return 1;
	}
	
	unsigned int oids_count = 0;
	if(ral == 0) {
		for(oids_count=0; oids_count<oids_name_length; oids_count++) {
			if(oids_result[oids_count].type == SNMP_NOSUCHOBJECT) {
				printf("[main] No Such Object available on this agent at this OID\n");
			}
	
			if(oids_result[oids_count].type == SNMP_NOSUCHINSTANCE) {
				printf("[main] No Such Instance currently exists at this OID\n");
			}
	
			if(oids_result[oids_count].type == SNMP_ENDOFMIBVIEW) {
				printf("[main] No more variables left in this MIB View (It is past the end of the MIB tree)\n");
			}

			if(oids_result[oids_count].type == ASN_OCTET_STR || oids_result[oids_count].type == ASN_BIT_STR) {
				printf("%s\n", oids_result[oids_count].val.string);
			}
			
			if(oids_result[oids_count].type == ASN_INTEGER) {
				printf("%ld\n", oids_result[oids_count].val.integer);
			}
			
			if(oids_result[oids_count].type == ASN_UINTEGER) {
				printf("%lu\n", oids_result[oids_count].val.integer);
			}
			
			if(oids_result[oids_count].type == ASN_OBJECT_ID) {
				for(count=0; count<oids_result[oids_count].objid_length; count++) {
					printf("%d", oids_result[oids_count].val.objid[count]);
					count < (oids_result[oids_count].objid_length-1) ? printf(".") : printf("\n");
				}
			}
			
			if( oids_result[oids_count].type == ASN_TIMETICKS || 
				oids_result[oids_count].type == ASN_GAUGE || 
				oids_result[oids_count].type == ASN_COUNTER ) {
				printf("%lu\n", oids_result[oids_count].val.integer);
			}
			
			if(oids_result[oids_count].type == ASN_OPAQUE_FLOAT) {
				printf("%f\n", oids_result[oids_count].val.floatVal);
			}
			
			if(oids_result[oids_count].type == ASN_OPAQUE_DOUBLE) {
				printf("%f\n", oids_result[oids_count].val.doubleVal);
			}
			
			if (oids_result[oids_count].type == ASN_OPAQUE_COUNTER64 || 
				oids_result[oids_count].type == ASN_OPAQUE_U64 || 
				oids_result[oids_count].type == ASN_COUNTER64 || 
				oids_result[oids_count].type == ASN_OPAQUE_I64 ) 
			{
			printf("%s\n", oids_result[oids_count].val.string);
			}
			
			if(oids_result[oids_count].type == ASN_IPADDRESS) {
				printf("%d.%d.%d.%d\n", 
						oids_result[oids_count].val.string[0], 
						oids_result[oids_count].val.string[1],
						oids_result[oids_count].val.string[2],
						oids_result[oids_count].val.string[3] 
				);
			}
		}
	}
	return 0;
}
#endif