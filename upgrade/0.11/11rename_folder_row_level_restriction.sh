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
alter table folder_row_level_restriction rename as table_row_level_restriction;
alter table table_row_level_restriction change folder table_name char(50) not null;
alter table table_row_level_restriction drop index folder_row_level_restriction;
create unique index table_row_level_restriction on table_row_level_restriction (table_name);
update appaserver_table set table_name = 'table_row_level_restriction' where table_name = 'folder_row_level_restriction';
update table_column set table_name = 'table_row_level_restriction' where table_name = 'folder_row_level_restriction';
update table_column set column_name = 'table_name' where table_name = 'table_row_level_restriction' and column_name = 'folder';
update relation set table_name = 'table_row_level_restriction', related_table = 'table' where table_name = 'folder_row_level_restriction' and related_table = 'folder';
update relation set table_name = 'table_row_level_restriction' where table_name = 'folder_row_level_restriction' and related_table = 'row_level_restriction';
all_done

exit 0
