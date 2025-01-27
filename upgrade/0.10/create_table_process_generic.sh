#!/bin/bash
if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "$ . set_database" 1>&2
	exit 1
fi

echo "drop table if exists process_generic;" | sql.e;echo "create table process_generic (process char (40) not null,process_set char (40) not null,value_folder char (50),value_attribute char (60)) engine MyISAM;" |sql.e 2>&1 |tee_appaserver.sh;echo "create unique index process_generic on process_generic (process,process_set);" |sql.e 2>&1
exit 0

