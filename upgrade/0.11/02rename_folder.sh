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
alter table folder rename as appaserver_table;
alter table appaserver_table change folder table_name char(50) not null;
alter table appaserver_table drop index folder;
create unique index appaserver_table on appaserver_table (table_name);
update appaserver_table set table_name = 'table' where table_name = 'folder';
update attribute set attribute = 'table_name', width=50 where attribute = 'folder';
update folder_attribute set attribute = 'table_name' where attribute = 'folder';
update relation set folder = 'table' where folder = 'folder' and related_folder = 'process' and related_attribute = 'populate_drop_down_process';
update relation set folder = 'table' where folder = 'folder' and related_folder = 'process' and related_attribute = 'post_change_process';
update relation set folder = 'table' where folder = 'folder' and related_folder = 'subschema';
update relation set folder = 'table' where folder = 'folder' and related_folder = 'form';
update appaserver_table set subschema = 'schema' where subschema = 'folder';
update subschema set subschema = 'schema' where subschema = 'folder';
update appaserver_table set table_name = 'table' where table_name = 'folder';
update appaserver_table set table_name = 'table_column' where table_name = 'folder_attribute';
update appaserver_table set table_name = 'foreign_column' where table_name = 'foreign_attribute';
update appaserver_table set table_name = 'column' where table_name = 'attribute';
update appaserver_table set table_name = 'column_datatype' where table_name = 'attribute_datatype';
update appaserver_table set table_name = 'role_table' where table_name = 'role_folder';
update appaserver_table set table_name = 'column_exclude' where table_name = 'attribute_exclude';
update appaserver_table set table_name = 'table_row_level_restriction' where table_name = 'folder_row_level_restriction';
update appaserver_table set table_name = 'table_operation' where table_name = 'folder_operation';
all_done

exit 0
