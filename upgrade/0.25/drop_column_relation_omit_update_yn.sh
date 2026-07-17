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
echo "alter table relation drop omit_update_yn;" | tee_appaserver.sh | sql.e 2>&1
echo "delete from appaserver_column where column_name = 'omit_update_yn';" | sql.e 2>&1
echo "delete from table_column where column_name = 'omit_update_yn';" | sql.e 2>&1

echo "update table_column set display_order = 4 where table_name = 'relation' and column_name = 'relation_type_isa_yn';" | sql.e
echo "update table_column set display_order = 5 where table_name = 'relation' and column_name = 'copy_common_columns_yn';" | sql.e
echo "update table_column set display_order = 6 where table_name = 'relation' and column_name = 'automatic_preselection_yn';" | sql.e
echo "update table_column set display_order = 7 where table_name = 'relation' and column_name = 'drop_down_multi_select_yn';" | sql.e
echo "update table_column set display_order = 8 where table_name = 'relation' and column_name = 'join_one2m_each_row_yn';" | sql.e
echo "update table_column set display_order = 9 where table_name = 'relation' and column_name = 'ajax_fill_drop_down_yn';" | sql.e
echo "update table_column set display_order = 10 where table_name = 'relation' and column_name = 'hint_message';" | sql.e

) 2>&1 | grep -vi duplicate

exit 0

