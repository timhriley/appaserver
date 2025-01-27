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
echo "alter table relation add omit_ajax_fill_drop_down_yn char (1);" | tee_appaserver.sh | sql.e 2>&1
echo "insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('omit_ajax_fill_drop_down_yn','text',1,null,'Don''t display the ajax fill button if the relation_mto1_to_one has ajax_fill_drop_down_yn set');" | sql.e 2>&1
echo "insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,insert_required_yn,lookup_required_yn,default_value) values ('relation','omit_ajax_fill_drop_down_yn',null,10,null,null,null,null,null,null,null,null);" | sql.e 2>&1

echo "update table_column set display_order = 1 where table_name = 'relation' and column_name = 'pair_one2m_order';" | sql.e
echo "update table_column set display_order = 2 where table_name = 'relation' and column_name = 'omit_drillthru_yn';" | sql.e
echo "update table_column set display_order = 3 where table_name = 'relation' and column_name = 'omit_drilldown_yn';" | sql.e
echo "update table_column set display_order = 4 where table_name = 'relation' and column_name = 'relation_type_isa_yn';" | sql.e
echo "update table_column set display_order = 5 where table_name = 'relation' and column_name = 'copy_common_columns_yn';" | sql.e
echo "update table_column set display_order = 6 where table_name = 'relation' and column_name = 'automatic_preselection_yn';" | sql.e
echo "update table_column set display_order = 7 where table_name = 'relation' and column_name = 'drop_down_multi_select_yn';" | sql.e
echo "update table_column set display_order = 8 where table_name = 'relation' and column_name = 'join_one2m_each_row_yn';" | sql.e
echo "update table_column set display_order = 9 where table_name = 'relation' and column_name = 'ajax_fill_drop_down_yn';" | sql.e
echo "update table_column set display_order = 10 where table_name = 'relation' and column_name = 'omit_ajax_fill_drop_down_yn';" | sql.e
echo "update table_column set display_order = 11 where table_name = 'relation' and column_name = 'trigger_insert_update_yn';" | sql.e
echo "update table_column set display_order = 12 where table_name = 'relation' and column_name = 'hint_message';" | sql.e

) 2>&1 | grep -vi duplicate
exit 0

