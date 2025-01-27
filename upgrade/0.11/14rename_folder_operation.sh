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
alter table folder_operation rename as table_operation;
alter table table_operation change folder table_name char(50) not null;
alter table table_operation drop index folder_operation;
create unique index table_operation on table_operation (table_name,role,operation);
update relation set table_name = 'table_operation', related_table = 'table' where table_name = 'folder_operation' and related_table = 'folder';
update relation set table_name = 'table_operation' where table_name = 'folder_operation' and related_table = 'operation';
update relation set table_name = 'table_operation' where table_name = 'folder_operation' and related_table = 'role';
update table_operation set table_name = 'table' where table_name = 'folder';
update table_operation set table_name = 'table_column' where table_name = 'folder_attribute';
update table_operation set table_name = 'foreign_column' where table_name = 'foreign_attribute';
update table_operation set table_name = 'column' where table_name = 'attribute';
update table_operation set table_name = 'column_datatype' where table_name = 'attribute_datatype';
update table_operation set table_name = 'role_table' where table_name = 'role_folder';
update table_operation set table_name = 'table_row_level_restriction' where table_name = 'folder_row_level_restriction';
update table_operation set table_name = 'table_operation' where table_name = 'folder_operation';
update table_operation set table_name = 'column_exclude' where table_name = 'attribute_exclude';
all_done

exit 0
