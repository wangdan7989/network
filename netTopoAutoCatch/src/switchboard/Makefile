object=getIPAddrTable.o
getIPAddrTable:$(object)
	$(cc) -o getIPAddrTable getIPAddrTable.o -I $(include) -L $(lib) -lnetsnmp -lbulkwalk
getIPAddrTable.o:getIPAddrTable.c 
	$(cc) -c getIPAddrTable.c  -I $(include) -L $(lib) -lnetsnmp -lbulkwalk

clean:
	-rm getIPAddrTable $(object)
cc=gcc
lib=/usr/lib/netsnmp
include=/usr/local/net-snmp/include/net-snmp

