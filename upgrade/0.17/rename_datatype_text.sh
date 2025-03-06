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

where="column_datatype = 'text'"
set="column_datatype = 'character'"

echo "update column_datatype set $set where $where;" | sql.e
echo "update appaserver_column set $set where $where;" | sql.e

exit 0
