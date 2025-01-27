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

sql.e << all_done
alter table role_folder rename as role_table;
alter table role_table change folder table_name char(50) not null;
alter table role_table drop index role_folder;
create unique index role_table on role_table (role,table_name,permission);
update relation set folder = 'role_table', related_folder = 'table' where folder = 'role_folder' and related_folder = 'folder';
update relation set folder = 'role_table' where folder = 'role_folder' and related_folder = 'permission';
update relation set folder = 'role_table' where folder = 'role_folder' and related_folder = 'role';
update role_table set table_name = 'table' where table_name = 'folder';
update role_table set table_name = 'table_column' where table_name = 'folder_attribute';
update role_table set table_name = 'foreign_column' where table_name = 'foreign_attribute';
update role_table set table_name = 'column' where table_name = 'attribute';
update role_table set table_name = 'column_datatype' where table_name = 'attribute_datatype';
update role_table set table_name = 'role_table' where table_name = 'role_folder';
update role_table set table_name = 'column_exclude' where table_name = 'attribute_exclude';
update role_table set table_name = 'table_row_level_restriction' where table_name = 'folder_row_level_restriction';
update role_table set table_name = 'table_operation' where table_name = 'folder_operation';
update table_column set primary_key_index = 1 where table_name = 'role_table' and column_name = 'role';
update table_column set primary_key_index = 2 where table_name = 'role_table' and column_name = 'table_name';
update table_column set primary_key_index = 3 where table_name = 'role_table' and column_name = 'permission';
all_done

exit 0
