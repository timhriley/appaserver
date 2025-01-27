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
alter table attribute_datatype rename as column_datatype;
alter table column_datatype change attribute_datatype column_datatype char(20) not null;
alter table column_datatype drop index attribute_datatype;
create unique index column_datatype on column_datatype (column_datatype);
update appaserver_table set table_name = 'column_datatype' where table_name = 'attribute_datatype';
update table_column set table_name = 'column_datatype' where table_name = 'attribute_datatype';
all_done

exit 0
