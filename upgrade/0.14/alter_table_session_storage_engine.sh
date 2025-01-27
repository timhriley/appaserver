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
echo "alter table session engine = memory;" | tee_appaserver.sh | sql.e 2>&1
echo "update appaserver_table set storage_engine = 'memory' where table_name = 'session';" | sql.e
) 2>&1 | grep -vi duplicate
exit 0

