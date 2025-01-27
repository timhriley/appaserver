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

echo "update relation set related_folder = 'subschema' where related_folder = 'subschemas';" | sql
echo "update role_folder set folder = 'subschema' where folder = 'subschemas';" | sql
echo "update role_operation set folder = 'subschema' where folder = 'subschemas';" | sql
echo "update folder_attribute set folder = 'subschema' where folder = 'subschemas';" | sql
echo "update folder set folder = 'subschema' where folder = 'subschemas';" | sql
echo "alter table subschemas rename as subschema;" | sql
echo "alter table subschema drop index subschemas;" | sql.e
echo "create unique index subschema on subschema ( subschema );" | sql.e

exit 0
