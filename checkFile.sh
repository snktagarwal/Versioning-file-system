#!/bin/bash

if [ `file -i -b '+filename+' | cut -d \/ -f 1` == 'text' ]
then
	echo "text"
	exit 1
else
	echo "non-text"
	exit 0
fi 
