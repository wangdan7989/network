#!/bin/bash

#gcc my_snmp_bulkwalk.c -o walk -I ../netsnmp/include/net-snmp -lnetsnmp 
gcc my_snmp_bulkwalk.c -o walk -I ./netsnmp/include/net-snmp -L /usr/lib/netsnmp -lnetsnmp 
#gcc -shared my_snmp_bulkwalk.c -o libbulkwalk.so -I ./netsnmp/include/net-snmp -L /usr/lib/netsnmp -lnetsnmp -fPIC


