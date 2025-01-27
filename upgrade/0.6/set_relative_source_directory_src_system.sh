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

echo "update attribute set width = 255 where attribute = 'relative_source_directory';" | sql

table_name="${application}_application"

echo "alter table $table_name modify relative_source_directory char( 255 );" | sql

current_entry=`select.sh relative_source_directory $table_name`
new_entry="${current_entry}:src_system"

echo "update $table_name set relative_source_directory = '$new_entry';" | sql

exit 0
