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
alter table process_parameter change folder table_name char(50) not null;
alter table process_parameter change attribute column_name char(60) not null;
alter table process_parameter drop index process_parameter;
create unique index process_parameter on process_parameter (process,table_name,column_name,drop_down_prompt,prompt);
update relation set related_table = 'column' where table_name = 'process_parameter' and related_table = 'attribute';
update relation set related_table = 'table' where table_name = 'process_parameter' and related_table = 'folder';
update process_parameter set table_name = 'table' where table_name = 'folder';
update process_parameter set table_name = 'column' where table_name = 'attribute';
update process_parameter set table_name = 'table_column' where table_name = 'folder_attribute';
update appaserver_table set notepad = null where table_name = 'process_parameter';
update table_column set primary_key_index = 4 where table_name = 'process_parameter' and column_name = 'drop_down_prompt';
update table_column set primary_key_index = 5 where table_name = 'process_parameter' and column_name = 'prompt';
all_done

exit 0
