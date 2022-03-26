#!/bin/sh
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

echo "update attribute set attribute = 'drillthru_yn' where attribute = 'lookup_before_drop_down_yn';" | sql

echo "update folder_attribute set attribute = 'drillthru_yn' where attribute = 'lookup_before_drop_down_yn';" | sql

echo "alter table folder change lookup_before_drop_down_yn drillthru_yn char (1);" | sql.e 2>&1 | grep -v 'Unknown column'
exit 0
