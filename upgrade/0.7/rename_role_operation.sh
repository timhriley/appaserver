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

echo "update folder set folder = 'folder_operation' where folder = 'role_operation';" | sql.e
echo "update folder_attribute set folder = 'folder_operation' where folder = 'role_operation';" | sql.e
echo "update role_folder set folder = 'folder_operation' where folder = 'role_operation';" | sql.e
echo "update role_operation set folder = 'folder_operation' where folder = 'role_operation';" | sql.e
echo "update relation set folder = 'folder_operation' where folder = 'role_operation';" | sql.e
echo "update relation set related_folder = 'folder_operation' where related_folder = 'role_operation';" | sql.e

echo "alter table role_operation rename as folder_operation;" | sql.e

echo "alter table folder_operation drop index role_operation;" | sql.e

echo "create unique index folder_operation on folder_operation( folder, role, operation );" | sql.e

exit 0

