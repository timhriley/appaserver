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

echo "delete from folder_attribute where folder = 'javascript_files';" | sql
echo "delete from folder where folder = 'javascript_files';" | sql
echo "delete from role_folder where folder = 'javascript_files';" | sql
echo "delete from role_operation where folder = 'javascript_files';" | sql
echo "delete from relation where folder = 'javascript_folders';" | sql
echo "delete from relation where related_folder = 'javascript_files';" | sql

echo "drop table javascript_files;" | sql

exit 0
