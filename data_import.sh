#!/bin/sh

DATAPATH=/home/abcdabcd987/test/
g++ -Wall data_import.cc /usr/local/lib/libleveldb.a -lpthread
find $DATAPATH -type f -exec ./a.out {} \;
