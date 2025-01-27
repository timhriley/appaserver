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
echo "alter table relation add trigger_insert_update_yn char (1);" | tee_appaserver.sh | sql.e 2>&1
echo "insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('trigger_insert_update_yn','text',1,null,null);" | sql.e 2>&1
echo "insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,insert_required_yn,lookup_required_yn,default_value) values ('relation','trigger_insert_update_yn',null,10,null,null,null,null,null,null,null,null);" | sql.e 2>&1
echo "update table_column set display_order = 11 where table_name = 'relation' and column_name = 'relation';" | sql.e
) 2>&1 | grep -vi duplicate
exit 0

