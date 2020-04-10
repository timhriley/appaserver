#!/bin/sh
#create station_datatype_alias

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

if [ "$application" != "hydrology" -a "$application" != "audubon" ]
then
	exit 0
fi

table_name=station_datatype_alias
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (datatype_alias char (25) not null,station char (20) not null,datatype char (25)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (datatype_alias,station);" | sql.e


