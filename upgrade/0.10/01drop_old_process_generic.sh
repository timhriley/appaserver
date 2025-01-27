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

folder=process_generic
echo "drop table if exists $folder;" | sql.e
echo "delete from folder where folder = '$folder';" | sql.e
echo "delete from folder_attribute where folder = '$folder';" | sql.e
echo "delete from role_folder where folder = '$folder';" | sql.e
echo "delete from folder_operation where folder = '$folder';" | sql.e
echo "delete from relation where folder = '$folder';" | sql.e
echo "delete from relation where related_folder = '$folder';" | sql.e

folder=process_generic_datatype_folder
echo "drop table if exists $folder;" | sql.e
echo "delete from folder where folder = '$folder';" | sql.e
echo "delete from folder_attribute where folder = '$folder';" | sql.e
echo "delete from role_folder where folder = '$folder';" | sql.e
echo "delete from folder_operation where folder = '$folder';" | sql.e
echo "delete from relation where folder = '$folder';" | sql.e
echo "delete from relation where related_folder = '$folder';" | sql.e

folder=process_generic_output
echo "drop table if exists $folder;" | sql.e
echo "delete from folder where folder = '$folder';" | sql.e
echo "delete from folder_attribute where folder = '$folder';" | sql.e
echo "delete from role_folder where folder = '$folder';" | sql.e
echo "delete from folder_operation where folder = '$folder';" | sql.e
echo "delete from relation where folder = '$folder';" | sql.e
echo "delete from relation where related_folder = '$folder';" | sql.e

folder=process_generic_units
echo "drop table if exists $folder;" | sql.e
echo "delete from folder where folder = '$folder';" | sql.e
echo "delete from folder_attribute where folder = '$folder';" | sql.e
echo "delete from role_folder where folder = '$folder';" | sql.e
echo "delete from folder_operation where folder = '$folder';" | sql.e
echo "delete from relation where folder = '$folder';" | sql.e
echo "delete from relation where related_folder = '$folder';" | sql.e

folder=process_generic_value
echo "drop table if exists $folder;" | sql.e
echo "delete from folder where folder = '$folder';" | sql.e
echo "delete from folder_attribute where folder = '$folder';" | sql.e
echo "delete from role_folder where folder = '$folder';" | sql.e
echo "delete from folder_operation where folder = '$folder';" | sql.e
echo "delete from relation where folder = '$folder';" | sql.e
echo "delete from relation where related_folder = '$folder';" | sql.e

folder=process_generic_value_folder
echo "drop table if exists $folder;" | sql.e
echo "delete from folder where folder = '$folder';" | sql.e
echo "delete from folder_attribute where folder = '$folder';" | sql.e
echo "delete from role_folder where folder = '$folder';" | sql.e
echo "delete from folder_operation where folder = '$folder';" | sql.e
echo "delete from relation where folder = '$folder';" | sql.e
echo "delete from relation where related_folder = '$folder';" | sql.e

exit 0

