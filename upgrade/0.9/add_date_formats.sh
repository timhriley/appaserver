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

echo "insert into date_format values ( 'american' );" | sql.e 2>&1 | grep -vi duplicate
echo "insert into date_format values ( 'military' );" | sql.e 2>&1 | grep -vi duplicate

exit 0
