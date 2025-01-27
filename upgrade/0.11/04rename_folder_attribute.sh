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
alter table folder_attribute rename as table_column;
alter table table_column change folder table_name char(50) not null;
alter table table_column change attribute column_name char(60) not null;
alter table table_column drop index folder_attribute;
create unique index table_column on table_column (table_name,column_name);
update relation set folder = 'table_column', related_folder = 'table' where folder = 'folder_attribute' and related_folder = 'folder';
update relation set folder = 'table_column', related_folder = 'column' where folder = 'folder_attribute' and related_folder = 'attribute';
update table_column set table_name = 'table' where table_name = 'folder';
update table_column set table_name = 'table_column' where table_name = 'folder_attribute';
update table_column set table_name = 'foreign_column' where table_name = 'foreign_attribute';
update table_column set table_name = 'column' where table_name = 'attribute';
update table_column set table_name = 'column_datatype' where table_name = 'attribute_datatype';
update table_column set table_name = 'role_table' where table_name = 'role_folder';
update table_column set table_name = 'column_exclude' where table_name = 'attribute_exclude';
update table_column set table_name = 'table_row_level_restriction' where table_name = 'folder_row_level_restriction';
update table_column set table_name = 'table_operation' where table_name = 'folder_operation';
update table_column set column_name = 'column_name' where column_name = 'attribute';
update table_column set column_name = 'column_datatype' where column_name = 'attribute_datatype';
update table_column set column_name = 'table_name' where column_name = 'folder';
update table_column set column_name = 'related_table' where column_name = 'related_folder';
update table_column set column_name = 'related_column' where column_name = 'related_attribute';
update table_column set column_name = 'foreign_column' where column_name = 'foreign_attribute';
update table_column set column_name = 'column_not_null' where column_name = 'attribute_not_null';
update table_column set column_name = 'value_table' where column_name = 'value_folder';
update table_column set column_name = 'value_column' where column_name = 'value_attribute';
update table_column set column_name = 'datatype_table' where column_name = 'datatype_folder';
update table_column set column_name = 'foreign_table' where column_name = 'foreign_folder';
update table_column set column_name = 'datatype_column' where column_name = 'datatype_attribute';
update table_column set column_name = 'date_column' where column_name = 'data_attribute';
update table_column set column_name = 'time_column' where column_name = 'time_attribute';
all_done

exit 0
