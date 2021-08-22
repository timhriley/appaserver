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

echo "delete from attribute where attribute = 'appaserver_yn';" | sql
echo "delete from folder_attribute where attribute = 'appaserver_yn';" | sql
echo "alter table folder drop column appaserver_yn;" | sql
echo "alter table process drop column appaserver_yn;" | sql
exit 0
