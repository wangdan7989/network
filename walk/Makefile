object=test1.o
test1:$(object)
	$(cc) -o test1 test1.o
test1.o:test1.c
	$(cc) -c test1.c -L $(lib) -lnetapi  -lnetsnmp   -lnetsnmpagent -lnetsnmphelper  -lnetsnmpmibs -lnetsnmptrapd -lnetsnmp
clean:
	-rm test1 $(object)
cc=gcc
lib=/usr/lib/snmplib/

