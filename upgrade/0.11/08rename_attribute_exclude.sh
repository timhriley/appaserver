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
alter table attribute_exclude rename as column_exclude;
alter table column_exclude change attribute column_name char(60) not null;
alter table column_exclude drop index attribute_exclude;
create unique index column_exclude on column_exclude (role,column_name,permission);
update appaserver_table set table_name = 'column_exclude' where table_name = 'attribute_exclude';
update relation set folder = 'column_exclude', related_folder = 'column' where folder = 'attribute_exclude' and related_folder = 'attribute';
update relation set folder = 'column_exclude' where folder = 'attribute_exclude' and related_folder = 'permission';
update relation set folder = 'column_exclude' where folder = 'attribute_exclude' and related_folder = 'role';
update column_exclude set column_name = 'column_name' where column_name = 'attribute';
update column_exclude set column_name = 'column_datatype' where column_name = 'attribute_datatype';
update column_exclude set column_name = 'table_name' where column_name = 'folder';
update column_exclude set column_name = 'related_table' where column_name = 'related_folder';
update column_exclude set column_name = 'related_column' where column_name = 'related_attribute';
update column_exclude set column_name = 'foreign_column' where column_name = 'foreign_attribute';
update column_exclude set column_name = 'column_not_null' where column_name = 'attribute_not_null';
update column_exclude set column_name = 'value_table' where column_name = 'value_folder';
update column_exclude set column_name = 'value_column' where column_name = 'value_attribute';
update column_exclude set column_name = 'datatype_table' where column_name = 'datatype_folder';
update column_exclude set column_name = 'foreign_table' where column_name = 'foreign_folder';
update column_exclude set column_name = 'datatype_column' where column_name = 'datatype_attribute';
update column_exclude set column_name = 'date_column' where column_name = 'data_attribute';
update column_exclude set column_name = 'time_column' where column_name = 'time_attribute';
all_done

exit 0
