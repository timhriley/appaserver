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

columnList="full_name,street_address,website_address,website_login,website_password"

where="full_name not in (select full_name from self)"

echo "select $columnList from entity where $where;"		|
sql '|'								|
insert_statement.e table=vendor field=$columnList		|
sql

exit 0
