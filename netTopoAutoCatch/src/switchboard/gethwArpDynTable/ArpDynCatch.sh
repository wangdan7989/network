
gcc ./database_ArpDynCatch.c ./gethwArpDynTable.c ./ArpDynCatch.c -o ./ArpDynCatch -I /usr/local/net-snmp/include/net-snmp -L/usr/lib/netsnmp -lnetsnmp -lbulkwalk -lmysqlclient
