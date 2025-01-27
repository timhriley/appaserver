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
alter table row_security_role_update change folder table_name char(50) not null;
alter table row_security_role_update change attribute_not_null column_not_null char(60);
alter table row_security_role_update drop index row_security_role_update;
create unique index row_security_role_update on row_security_role_update (table_name);
update appaserver_column set column_name = 'column_not_null' where column_name = 'attribute_not_null';
update table_column set column_name = 'column_not_null' where column_name = 'attribute_not_null';
update relation set related_folder = 'table_column', related_attribute = 'column_not_null' where folder = 'row_security_role_update' and related_folder = 'folder_attribute';
all_done

exit 0
