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
alter table select_statement drop login_name;
alter table select_statement drop index select_statement;
alter table select_statement drop index select_statement_unique;
create unique index select_statement_unique on select_statement (select_statement_title);

delete from table_column where table_name = 'select_statement' and column_name = 'login_name';

show create table select_statement;
shell_all_done
) | sql.e

exit 0
