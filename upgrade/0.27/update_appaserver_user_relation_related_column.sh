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

if [ "$application" = "appahost" ]
then
	exit 0
fi

echo "update relation set related_column = 'appaserver_full_name' where table_name = 'appaserver_user' and related_table = 'entity';" | sql
exit 0

