#!/bin/sh
#create station_alias

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

if [ "$application" != "waterquality" ]
then
	exit 0
fi

table_name=station_alias
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (station_alias char (30) not null,station char (12)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (station_alias);" | sql.e


