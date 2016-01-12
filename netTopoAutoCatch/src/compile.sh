#!/bin/bash

gcc database_netTopoAutoCatch.c `mysql_config --cflags --libs` -o test
