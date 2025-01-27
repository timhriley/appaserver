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
alter table process_generic change value_folder value_table char(50);
alter table process_generic change value_attribute value_column char(60);
alter table process_generic drop index process_generic;
create unique index process_generic on process_generic (process,process_set);
update appaserver_column set column_name = 'value_table' where column_name = 'value_folder';
update appaserver_column set column_name = 'value_column' where column_name = 'value_attribute';
update table_column set column_name = 'value_table' where column_name = 'value_folder';
update table_column set column_name = 'value_column' where column_name = 'value_attribute';
insert into relation (table_name,related_table,related_column) values ('process_generic','process','null');
insert into relation (table_name,related_table,related_column) values ('process_generic','process_set','null');
insert into relation (table_name,related_table,related_column) values ('process_generic','process_generic_value','null');
all_done

exit 0
