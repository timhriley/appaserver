#!/bin/sh
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
alter table form rename as appaserver_form;
alter table appaserver_form change form appaserver_form char(6) not null;
alter table appaserver_table change form appaserver_form char(6);
update appaserver_table set table_name = 'appaserver_form' where table_name = 'form';
update appaserver_column set column_name = 'appaserver_form' where column_name = 'form';
update table_column set table_name = 'appaserver_form' where table_name = 'form';
update table_column set column_name = 'appaserver_form' where column_name = 'form';
update relation set related_table = 'appaserver_form' where related_table = 'form';
update role_table set table_name = 'appaserver_form' where table_name = 'form';
update table_operation set table_name = 'appaserver_form' where table_name = 'form';
alter table appaserver_form drop index form;
create unique index appaserver_form on appaserver_form ( appaserver_form );
all_done

exit 0
