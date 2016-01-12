
#include "my_snmp_bulkwalk.h"

/**
 * Get snmp value to the struct my_oid_result.
 * Author : lining 15810423651 lining01@iie.ac.cn 
 * @param netsnmp_variable_list *vars  : the return pdu variable value.
 * @param my_oid_result : the struct my_oid_result.
 * @return void.
 */
void get_bulkwalk_oid_values(netsnmp_variable_list *vars, my_oid_result *oid_result) {
	
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
		return;
	}

	if (vars->type == ASN_UINTEGER) {
		oid_result->val.integer = *vars->val.integer;
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
}

/**
 * print the my_snmp_bulkwalk_perror function return error 1 ~ 5 string.
 * Author : lining 15810423651 lining01@iie.ac.cn 
 * @param ratval  : the my_get_snmps function return value.
 * @return no return.
 */
void my_snmp_bulkwalk_perror(unsigned int ratval) {
	switch (ratval) {
		case 1:
			printf("[ERROR] my_snmp_walk: the argument value oid_names is empty\n");
			break;
		case 2:
			printf("[ERROR] my_snmp_walk: the snmp_open function run failed\n");
			break;
		case 3:
			printf("[ERROR] my_snmp_walk: the snmp_parse_oid function run failed\n");
			break;
		case 4:
			printf("[ERROR] my_snmp_walk: time out\n");
			break;	
		case 5:
			printf("[ERROR] my_snmp_walk: snmpget sesson_handls error\n");
			break;
		case 6:
			printf("[ERROR] my_snmp_walk: end of MIB\n");
			break;
		case 7:
			printf("[ERROR] my_snmp_walk: response->errstat others error\n");
			break;
		default :
			printf("[ERROR] my_snmp_walk: no %d error type return value\n", ratval);
	}
}

/**
 * Bulk walk snmp value to the structure my_oid_result array.
 * Author : lining 15810423651 lining01@iie.ac.cn 
 * @param peername  : the remote host ip address.
 * @param oid_argv : the char *oid pointer.
 * @param my_oid_result oid_results : the return values array.
 * @param my_oid_result oid_results_nums : the values array numbers. 
 * @return 0 on success, or others on failure.
 * @return 1 on failure, the argument error.
 * @return 2 on failure, snmp_open return error.
 * @return 3 on failure, snmp_parse_oid return error.
 * @return 4 on failure, snmp_synch_response return status time out.
 * @return 5 on failure, snmp_synch_response return status others.
 * @return 6 on failure, response->errstat end of mib.
 * @return 7 on failure, response->errstat others.
 */
int my_snmp_bulkwalk(const char *peername, 
				     const char *community, 
				     const char *oid_argv, 
				     my_oid_result *oid_results, 
				     unsigned int *oid_results_nums) {

  	if(peername == NULL) {
		printf("[ERROR] my_snmp_walk: the peername pointer is null\n");
		return 1;
	}
	
	if(community == NULL) {
		printf("[ERROR] my_snmp_walk: the community pointer is null\n");
		return 1;
	}
	
	if(oid_argv == NULL) {
		printf("[ERROR] my_snmp_walk: the oid_argv pointer is null\n");	
		return 1;
	}

	netsnmp_session	session,*ss;
	netsnmp_pdu	*pdu, *response;
    netsnmp_variable_list	*vars;
    int	arg;
    oid	name[MAX_OID_LEN];
    size_t	name_length;
    oid	root[MAX_OID_LEN];
    size_t	rootlen;
    int	count;
    int	running;
    int	status;
    int	check;
	int	numprinted = 0;
	int	reps = 10, non_reps = 0;	
	
	netsnmp_ds_register_config(ASN_BOOLEAN, "snmpwalk", "includeRequested", NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_INCLUDE_REQUESTED);
	netsnmp_ds_register_config(ASN_BOOLEAN, "snmpwalk", "printStatistics", NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_PRINT_STATISTICS);
	netsnmp_ds_register_config(ASN_BOOLEAN, "snmpwalk", "dontCheckOrdering", NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_DONT_CHECK_LEXICOGRAPHIC);


	init_snmp("snmpapp");
	snmp_sess_init(&session);
	session.version = SNMP_VERSION_2c;
	session.community = (char*)community;
	session.community_len = strlen(session.community);
	session.peername = (char*)peername;	
	

	rootlen = MAX_OID_LEN;
	if(snmp_parse_oid(oid_argv, root, &rootlen) == NULL) {
		printf("[ERROR] my_snmp_bulkwalk: call snmp_parse_oid function failed\n");
		return 2;
	}

    SOCK_STARTUP;
    ss = snmp_open(&session);
    if(ss == NULL) {
        printf("[ERROR] my_snmp_bulkwalk: cakk snnp_open function failed\n");
        SOCK_CLEANUP;
        return 3;
    }


    memmove(name, root, rootlen * sizeof(oid));
    name_length = rootlen;

    running = 1;

    check = !netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_DONT_CHECK_LEXICOGRAPHIC);

    while(running) {

        pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
        pdu->non_repeaters = non_reps;
        pdu->max_repetitions = reps;
        snmp_add_null_var(pdu, name, name_length);

        status = snmp_synch_response(ss, pdu, &response);
        if(status == STAT_SUCCESS) {
            if(response->errstat == SNMP_ERR_NOERROR) {

                for(vars = response->variables; vars; vars = vars->next_variable) {
                    if((vars->name_length < rootlen) || (memcmp(root, vars->name, rootlen * sizeof(oid)) != 0)) {
                        running = 0;
                        continue;
                    }
		
					get_bulkwalk_oid_values(vars, &oid_results[numprinted]);
                    numprinted++;
			
                    if((vars->type != SNMP_ENDOFMIBVIEW) && (vars->type != SNMP_NOSUCHOBJECT) && (vars->type != SNMP_NOSUCHINSTANCE)) {
                        if(check && snmp_oid_compare(name, name_length, vars->name, vars->name_length) >= 0) {
                            running = 0;
                        }
                        if(vars->next_variable == NULL) {
							
							unsigned int count;
							for(count = 0; count < vars->name_length; count++) {
								oid_results[numprinted - 1].oid_name[count] = vars->name[count];
								oid_results[numprinted - 1].oid_name_length = vars->name_length;
							}
                            memmove(name, vars->name, vars->name_length * sizeof(oid));
                            name_length = vars->name_length;
                        }
                    } else {
                        running = 0;
                    }
                }
            } else {
                running = 0;
                if(response->errstat == SNMP_ERR_NOSUCHNAME) {
					printf("[ERROR] my_snmp_bulkwalk: have reached the end of MIB file\n");
					return 6;
                } else {
					printf("[ERROR] my_snmp_bulkwalk: return response->errstat others error\n");
					return 7;
                }
            }
        } else if(status == STAT_TIMEOUT) {
			printf("[ERROR] my_snmp_bulkwalk: return status is session time out\n");
			running = 0;
			return 4;
        } else {
			printf("[ERROR] my_snmp_bulkwalk: return session response error\n");
            running = 0;
			return 5;
        }
		
        if(response) {
            snmp_free_pdu(response);
		}
    }
	
	*oid_results_nums = numprinted;
    snmp_close(ss);
    SOCK_CLEANUP;
	
    return 0;	
}


int main(int argc, char *argv[]) {
	
	char *peername = NULL;
	char *oid_name = NULL;
	char *community = NULL;
	unsigned int count = 0;
	my_oid_result *oid_results;
	
	oid_results =  (struct my_oid_result *)malloc(sizeof(struct my_oid_result) * 1024 * 16);
	
	unsigned int oid_results_nums =0;	
	
	peername  = argv[1];
	community = argv[2];
	oid_name  = argv[3];

	int ral = my_snmp_bulkwalk(peername, community, oid_name, oid_results, &oid_results_nums);

	unsigned int oids_count = 0;
	
	if(ral == 0) {

		for(oids_count=0; oids_count < oid_results_nums; oids_count++) {
			if(oid_results[oids_count].type == SNMP_NOSUCHOBJECT) {
				printf("[main] No Such Object available on this agent at this OID\n");
			}
	
			if(oid_results[oids_count].type == SNMP_NOSUCHINSTANCE) {
				printf("[main] No Such Instance currently exists at this OID\n");
			}
	
			/* print the oid value */
			unsigned int oid_count_len;
			for(oid_count_len = 0; oid_count_len < oid_results[oids_count].oid_name_length; oid_count_len++) {
				printf(".%ld", oid_results[oids_count].oid_name[oid_count_len]);
			}
			printf(": ");
			/* end print the oid value */
			
			
			if(oid_results[oids_count].type == SNMP_ENDOFMIBVIEW) {
				printf("[main] No more variables left in this MIB View (It is past the end of the MIB tree)\n");
			}

			if(oid_results[oids_count].type == ASN_OCTET_STR || oid_results[oids_count].type == ASN_BIT_STR) {
				
				int hex = 0, x;
				char *cp;
				for (cp = oid_results[oids_count].val.string, x = 0; x < (int)oid_results[oids_count].val_len; x++, cp++) {
					if (!isprint(*cp) && !isspace(*cp)) {
						hex = 1;
					}
				}
				
				if(!hex) {
					
					printf("%s\n", oid_results[oids_count].val.string);
				} else {
					for(count=0; count<(int)oid_results[oids_count].val_len; count++) {
						printf("%x:", oid_results[oids_count].val.string[count]);
					}
					printf("\n");
				}
			}
			
			if(oid_results[oids_count].type == ASN_INTEGER) {
				printf("%ld\n", oid_results[oids_count].val.integer);
			}
			
			if(oid_results[oids_count].type == ASN_UINTEGER) {
				printf("%lu\n", oid_results[oids_count].val.integer);
			}
			
			if(oid_results[oids_count].type == ASN_OBJECT_ID) {
				for(count=0; count<oid_results[oids_count].objid_length; count++) {
					printf("%d", oid_results[oids_count].val.objid[count]);
					count < (oid_results[oids_count].objid_length-1) ? printf(".") : printf("\n");
				}
			}
			
			if( oid_results[oids_count].type == ASN_TIMETICKS || 
				oid_results[oids_count].type == ASN_GAUGE || 
				oid_results[oids_count].type == ASN_COUNTER ) {
				printf("%lu\n", oid_results[oids_count].val.integer);
			}
			
			if(oid_results[oids_count].type == ASN_OPAQUE_FLOAT) {
				printf("%f\n", oid_results[oids_count].val.floatVal);
			}
			
			if(oid_results[oids_count].type == ASN_OPAQUE_DOUBLE) {
				printf("%f\n", oid_results[oids_count].val.doubleVal);
			}
			
			if (oid_results[oids_count].type == ASN_OPAQUE_COUNTER64 || 
				oid_results[oids_count].type == ASN_OPAQUE_U64 || 
				oid_results[oids_count].type == ASN_COUNTER64 || 
				oid_results[oids_count].type == ASN_OPAQUE_I64 ) {
				printf("%s\n", oid_results[oids_count].val.string);
			}
			
			if(oid_results[oids_count].type == ASN_IPADDRESS) {
				printf("%d.%d.%d.%d\n", 
						oid_results[oids_count].val.string[0], 
						oid_results[oids_count].val.string[1],
						oid_results[oids_count].val.string[2],
						oid_results[oids_count].val.string[3] 
				);
			}
		}
	}
	printf("[main] walk %d oids values\n", oid_results_nums);
	return 0;
}

