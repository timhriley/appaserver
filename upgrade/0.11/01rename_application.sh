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

echo "alter table ${application}_application rename as appaserver_application;" | sql
echo "alter table appaserver_application drop index ${application}_application;" | sql
echo "create unique index appaserver_application on appaserver_application( application );" | sql

exit 0
