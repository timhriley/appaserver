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

echo "alter table javascript_files rename as javascript_file;" | sql.e

echo "update folder set folder = 'javascript_file' where folder = 'javascript_files';" | sql
echo "update folder_attribute set folder = 'javascript_file' where folder = 'javascript_files';" | sql

echo "insert into folder_attribute ( folder, attribute, display_order ) values ( 'folder', 'javascript_filename', 16 );" | sql
echo "insert into folder_attribute ( folder, attribute, display_order ) values ( 'process', 'javascript_filename', 9 );" | sql
echo "insert into folder_attribute ( folder, attribute, display_order ) values ( 'process_set', 'javascript_filename', 8 );" | sql

echo "insert into relation ( folder, related_folder, related_attribute ) values ('folder', 'javascript_file', 'null' );" | sql
echo "insert into relation ( folder, related_folder, related_attribute ) values ('process', 'javascript_file', 'null' );" | sql
echo "insert into relation ( folder, related_folder, related_attribute ) values ('process_set', 'javascript_file', 'null' );" | sql

echo "alter table folder add javascript_filename char (80);" | sql
echo "alter table process add javascript_filename char (80);" | sql
echo "alter table process_set add javascript_filename char (80);" | sql

echo "delete from folder where folder = 'javascript_folders';" | sql
echo "delete from folder where folder = 'javascript_processes';" | sql
echo "delete from folder where folder = 'javascript_process_sets';" | sql

echo "delete from folder_attribute where folder = 'javascript_folders';" | sql
echo "delete from folder_attribute where folder = 'javascript_processes';" | sql
echo "delete from folder_attribute where folder = 'javascript_process_sets';" | sql

echo "delete from relation where folder = 'javascript_folders';" | sql
echo "delete from relation where folder = 'javascript_processes';" | sql
echo "delete from relation where folder = 'javascript_process_sets';" | sql

echo "delete from role_folder where folder = 'javascript_folders';" | sql
echo "delete from role_folder where folder = 'javascript_processes';" | sql
echo "delete from role_folder where folder = 'javascript_process_sets';" | sql

echo "delete from role_operation where folder = 'javascript_folders';" | sql
echo "delete from role_operation where folder = 'javascript_processes';" | sql
echo "delete from role_operation where folder = 'javascript_process_sets';" | sql

from=javascript_folders
select.sh "folder,'javascript_filename',javascript_filename" $from	|
update_statement.e table=folder key=folder carrot=y			|
sql

from=javascript_processes
select.sh "process,'javascript_filename',javascript_filename" $from	|
update_statement.e table=process key=process carrot=y			|
sql

from=javascript_process_sets
select.sh "process_set,'javascript_filename',javascript_filename" $from	|
update_statement.e table=process_set key=process_set carrot=y		|
sql

echo "drop table javascript_folders;" | sql
echo "drop table javascript_processes;" | sql
echo "drop table javascript_process_sets;" | sql

exit 0
