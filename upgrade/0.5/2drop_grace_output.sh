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


folder="grace_output"
echo "drop table $folder;" | sql.e
echo "delete from folder where folder = '$folder';" | sql
echo "delete from folder_attribute where folder = '$folder';" | sql
echo "delete from relation where folder = '$folder';" | sql
echo "delete from relation where related_folder = '$folder';" | sql
echo "delete from role_folder where folder = '$folder';" | sql
echo "delete from process_parameter where folder = '$folder';" | sql
echo "delete from role_operation where folder = '$folder';" | sql

exit 0
