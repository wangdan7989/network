#!/bin/bash



#gcc ./common/my_snmp_bulkwalk.c getIPRouteTable.c getIPAddrTable.c getIPForWardingInfo.c getRouterName.c netTopoAutoCatch.c  -I ./common/ -I ./common/netsnmp/include/net-snmp -I ./common/netsnmp/lib/snmplib/ -lnetsnmp -g  ./database_netTopoAutoCatch.c `mysql_config --cflags --libs` -o ../bin/netTopoAutoCatch

gcc ./common/my_snmp_bulkwalk.c getIPRouteTable.c getIPAddrTable.c getIPForWardingInfo.c getRouterName.c netTopoAutoCatch.c  -I ./common/ -I ./common/netsnmp/include/net-snmp -I ./common/netsnmp/lib/snmplib/ -L /usr/lib/netsnmp -lnetsnmp -g  ./database_netTopoAutoCatch.c `mysql_config --cflags --libs` -o ../bin/netTopoAutoCatch
