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
echo "insert into appaserver_column (column_name,column_datatype,width) values ('omit_update_yn','character',1);" | sql.e 2>&1
echo "insert into table_column (table_name,column_name,display_order) values ('table_column','omit_update_yn',5);" | sql.e 2>&1
echo "update table_column set display_order = 1 where table_name = 'table_column' and column_name = 'primary_key_index';" | sql.e
echo "update table_column set display_order = 2 where table_name = 'table_column' and column_name = 'display_order';" | sql.e
echo "update table_column set display_order = 3 where table_name = 'table_column' and column_name = 'omit_insert_yn';" | sql.e
echo "update table_column set display_order = 4 where table_name = 'table_column' and column_name = 'omit_insert_prompt_yn';" | sql.e
echo "update table_column set display_order = 6 where table_name = 'table_column' and column_name = 'additional_unique_index_yn';" | sql.e
echo "update table_column set display_order = 7 where table_name = 'table_column' and column_name = 'additional_index_yn';" | sql.e
echo "update table_column set display_order = 8 where table_name = 'table_column' and column_name = 'insert_required_yn';" | sql.e
echo "update table_column set display_order = 9 where table_name = 'table_column' and column_name = 'lookup_required_yn';" | sql.e
echo "update table_column set display_order = 10 where table_name = 'table_column' and column_name = 'default_value';" | sql.e

) 2>&1 | grep -vi duplicate

exit 0

