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
alter table process_generic_value change value_folder value_table char(50);
alter table process_generic_value change value_attribute value_column char(60);
alter table process_generic_value change datatype_folder datatype_table char(50);
alter table process_generic_value change foreign_folder foreign_table char(50);
alter table process_generic_value change datatype_attribute datatype_column char(60);
alter table process_generic_value change date_attribute date_column char(60);
alter table process_generic_value change time_attribute time_column char(60);
alter table process_generic_value drop index process_generic_value;
create unique index process_generic_value on process_generic_value (value_table,value_column);
update appaserver_column set column_name = 'datatype_table' where column_name = 'datatype_folder';
update appaserver_column set column_name = 'foreign_table' where column_name = 'foreign_folder';
update appaserver_column set column_name = 'datatype_column' where column_name = 'datatype_attribute';
update appaserver_column set column_name = 'date_column' where column_name = 'date_attribute';
update appaserver_column set column_name = 'time_column' where column_name = 'time_attribute';
update table_column set column_name = 'datatype_table' where column_name = 'datatype_folder';
update table_column set column_name = 'foreign_table' where column_name = 'foreign_folder';
update table_column set column_name = 'datatype_column' where column_name = 'datatype_attribute';
update table_column set column_name = 'date_column' where column_name = 'date_attribute';
update table_column set column_name = 'time_column' where column_name = 'time_attribute';
insert into relation (table_name,related_table,related_column) values ('process_generic_value','column','value_column');
insert into relation (table_name,related_table,related_column) values ('process_generic_value','column','datatype_column');
insert into relation (table_name,related_table,related_column) values ('process_generic_value','column','date_column');
insert into relation (table_name,related_table,related_column) values ('process_generic_value','column','time_column');
insert into relation (table_name,related_table,related_column) values ('process_generic_value','table','value_table');
insert into relation (table_name,related_table,related_column) values ('process_generic_value','table','datatype_table');
insert into relation (table_name,related_table,related_column) values ('process_generic_value','table','foreign_table');
all_done

exit 0
