
#include <stdio.h>  
#include "/usr/include/mysql/mysql.h"  
  
int main()  
{  
    MYSQL mysql;  
    int t, r;  
    mysql_init(&mysql);  
    if(!mysql_real_connect(&mysql, "localhost", "root", "123", "netdata", 0, NULL, 0)) {  
        printf("Error connecting to Mysql!\n");  
    }else {  
        printf("Connected Mysql successful!\n");  
    }  
    mysql_close(&mysql);  
    return 0;  
}  
