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

(
cat << shell_all_done
alter table entity drop notepad;
delete from table_column where table_name = 'entity' and column_name = 'notepad';
show create table entity;
shell_all_done
) | sql.e

exit 0
