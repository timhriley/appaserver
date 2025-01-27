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


echo "drop table if exists date_format;" | sql.e
echo "alter table date_formats rename as date_format;" | sql.e

echo "update folder set folder = 'date_format' where folder = 'date_formats';" | sql
echo "update folder_attribute set folder = 'date_format' where folder = 'date_formats';" | sql
echo "update relation set folder = 'date_format' where folder = 'date_formats';" | sql
echo "update relation set related_folder = 'date_format' where related_folder = 'date_formats';" | sql
echo "update role_folder set folder = 'date_format' where folder = 'date_formats';" | sql
echo "update process_parameter set folder = 'date_format' where folder = 'date_formats';" | sql
echo "update role_operation set folder = 'date_format' where folder = 'date_formats';" | sql

echo "alter table date_format drop index date_formats;" | sql.e

echo "create unique index date_format on date_format( date_format );" | sql.e

exit 0
