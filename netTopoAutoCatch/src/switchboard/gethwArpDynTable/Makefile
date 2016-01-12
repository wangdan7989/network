object=gethwArpDynTable.o
gethwArpDynTable:$(object)
	$(cc) -o gethwArpDynTable gethwArpDynTable.o -I $(include) -L $(lib) -lnetsnmp -lbulkwalk
gethwArpDynTable.o:gethwArpDynTable.c 
	$(cc) -c gethwArpDynTable.c  -I $(include) -L $(lib) -lnetsnmp -lbulkwalk

clean:
	-rm gethwArpDynTable $(object)
cc=gcc
lib=/usr/lib/netsnmp
include=/usr/local/net-snmp/include/net-snmp

