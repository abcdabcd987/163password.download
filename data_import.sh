#!/bin/sh

DATAPATH=/home/abcdabcd987/test/
find $DATAPATH -type f -exec node data_import.js {} \;