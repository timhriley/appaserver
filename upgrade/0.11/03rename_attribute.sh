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
alter table attribute rename as appaserver_column;
alter table appaserver_column change attribute column_name char(60) not null;
alter table appaserver_column change attribute_datatype column_datatype char(20);
alter table appaserver_column drop index attribute;
create unique index appaserver_column on appaserver_column (column_name);
update appaserver_table set table_name = 'column' where table_name = 'attribute';
update appaserver_column set column_name = 'column_name' where column_name = 'attribute';
update appaserver_column set column_name = 'column_datatype' where column_name = 'attribute_datatype';
update appaserver_column set column_name = 'table_name' where column_name = 'folder';
update appaserver_column set column_name = 'related_table' where column_name = 'related_folder';
update appaserver_column set column_name = 'related_column' where column_name = 'related_attribute';
update appaserver_column set column_name = 'foreign_column' where column_name = 'foreign_attribute';
update appaserver_column set column_name = 'column_not_null' where column_name = 'attribute_not_null';
update appaserver_column set column_name = 'value_table' where column_name = 'value_folder';
update appaserver_column set column_name = 'value_column' where column_name = 'value_attribute';
update appaserver_column set column_name = 'datatype_table' where column_name = 'datatype_folder';
update appaserver_column set column_name = 'foreign_table' where column_name = 'foreign_folder';
update appaserver_column set column_name = 'datatype_column' where column_name = 'datatype_attribute';
update appaserver_column set column_name = 'date_column' where column_name = 'data_attribute';
update appaserver_column set column_name = 'time_column' where column_name = 'time_attribute';
update folder_attribute set folder = 'column' where folder = 'attribute';
update folder_attribute set attribute = 'column_name' where attribute = 'attribute';
update folder_attribute set attribute = 'column_datatype' where attribute = 'dattribute_datatype';
update relation set folder = 'column', related_folder = 'column_datatype'  where folder = 'attribute' and related_folder = 'attribute_datatype';
all_done

exit 0
