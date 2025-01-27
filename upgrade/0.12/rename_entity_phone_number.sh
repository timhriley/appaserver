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
alter table entity change phone_number land_phone_number char(15);
alter table entity change cell_number cell_phone_number char(15);
update appaserver_column set column_name = 'land_phone_number' where column_name = 'phone_number';
update appaserver_column set column_name = 'cell_phone_number' where column_name = 'cell_number';
update table_column set column_name = 'land_phone_number' where column_name = 'phone_number';
update table_column set column_name = 'cell_phone_number' where column_name = 'cell_number';
all_done

exit 0
