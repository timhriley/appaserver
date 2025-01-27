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

echo "update folder set folder = 'process_group' where folder = 'process_groups';" | sql.e
echo "update folder_attribute set folder = 'process_group' where folder = 'process_groups';" | sql.e
echo "update role_folder set folder = 'process_group' where folder = 'process_groups';" | sql.e
echo "update role_operation set folder = 'process_group' where folder = 'process_groups';" | sql.e
echo "update relation set related_folder = 'process_group' where related_folder = 'process_groups';" | sql.e

echo "alter table process_groups rename as process_group;" | sql.e
echo "alter table process_group drop index process_groups;" | sql.e
echo "create unique index process_group on process_group (process_group);" | sql.e

exit 0

