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

echo "alter table folder add javascript_filename char (80);" | sql

echo "select folder, javascript_filename from javascript_folders;"	|
sql '|'									|
sed 's/|/|javascript_filename|/'					|
tr '|' '^'								|
update_statement.e table=folder key=folder carrot=y			|
sql

echo "insert into folder_attribute ( folder, attribute, display_order ) values ( 'folder', 'javascript_filename', 16 );"				|
sql

echo "delete from folder_attribute where folder = 'javascript_folders';" | sql
echo "delete from folder where folder = 'javascript_folders';" | sql
echo "delete from role_folder where folder = 'javascript_folders';" | sql
echo "delete from role_operation where folder = 'javascript_folders';" | sql
echo "delete from relation where folder = 'javascript_folders';" | sql

echo "drop table javascript_folders;" | sql

exit 0
