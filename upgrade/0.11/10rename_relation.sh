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
alter table relation change folder table_name char(50) not null;
alter table relation change related_folder related_table char(50) not null;
alter table relation change related_attribute related_column char(60) not null;
alter table relation drop index relation;
create unique index relation on relation (table_name,related_table,related_column);
update appaserver_column set column_name = 'related_table' where column_name = 'related_folder';
update appaserver_column set width = 50 where column_name = 'related_table';
update appaserver_column set column_name = 'related_column' where column_name = 'related_attribute';
update table_column set column_name = 'related_table' where column_name = 'related_folder';
update table_column set column_name = 'related_column' where column_name = 'related_attribute';
update relation set related_table = 'table' where table_name = 'relation' and related_table = 'folder' and related_column = 'null';
update relation set related_table = 'table', related_column = 'related_table' where table_name = 'relation' and related_table = 'folder' and related_column = 'related_folder';
update relation set related_table = 'column', related_column = 'related_column' where table_name = 'relation' and related_table = 'attribute' and related_column = 'related_attribute';
all_done

exit 0
