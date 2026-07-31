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

(
echo "insert into appaserver_column (column_name,column_datatype,width) values ('appaserver_full_name','character',60);" | sql.e 2>&1

) 2>&1 | grep -vi duplicate

exit 0

