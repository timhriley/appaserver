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
alter table role change folder_count_yn table_count_yn char(1);
update table_column set column_name = 'table_count_yn' where column_name = 'folder_count_yn';
update appaserver_column set column_name = 'table_count_yn' where column_name = 'folder_count_yn';
all_done

exit 0
