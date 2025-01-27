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
alter table foreign_attribute rename as foreign_column;
alter table foreign_column change folder table_name char(50) not null;
alter table foreign_column change related_folder related_table char(50) not null;
alter table foreign_column change related_attribute related_column char(60) not null;
alter table foreign_column change foreign_attribute foreign_column char(60) not null;
alter table foreign_column drop index foreign_attribute;
create unique index foreign_column on foreign_column (table_name,related_table,related_column,foreign_column);
update relation set folder = 'foreign_column' where folder = 'foreign_attribute' and related_folder = 'relation';
update relation set folder = 'foreign_column', related_folder = 'table_column', related_attribute = 'foreign_column'  where folder = 'foreign_attribute' and related_folder = 'folder_attribute';
all_done

exit 0
