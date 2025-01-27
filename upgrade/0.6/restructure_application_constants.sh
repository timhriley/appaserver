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

echo "update folder set folder = 'application_constant' where folder = 'application_constants';" | sql.e
echo "update folder_attribute set folder = 'application_constant' where folder = 'application_constants';" | sql.e
echo "update role_folder set folder = 'application_constant' where folder = 'application_constants';" | sql.e
echo "update role_operation set folder = 'application_constant' where folder = 'application_constants';" | sql.e

echo "alter table application_constants rename as application_constant;" | sql.e

echo "update attribute set width = 255 where attribute = 'application_constant_value';" | sql.e

echo "alter table application_constant modify application_constant_value char(255);" | sql.e

echo "alter table application_constant drop index application_constants;" | sql.e

echo "create unique index application_constant on application_constant( application_constant);" | sql.e

exit 0

