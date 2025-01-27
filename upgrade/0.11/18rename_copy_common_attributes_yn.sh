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
alter table relation change copy_common_attributes_yn copy_common_columns_yn char(1);
update table_column set column_name = 'copy_common_columns_yn' where column_name = 'copy_common_attributes_yn';
update appaserver_column set column_name = 'copy_common_columns_yn' where column_name = 'copy_common_attributes_yn';
all_done

exit 0
