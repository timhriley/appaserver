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
echo "alter table appaserver_table add storage_engine char (10);" | tee_appaserver.sh | sql.e 2>&1
echo "insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('storage_engine','text',10,null,null);" | sql.e 2>&1
echo "insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,insert_required_yn,lookup_required_yn,default_value) values ('table','storage_engine',null,9,null,null,null,null,null,null,null,null);" | sql.e 2>&1
echo "insert into relation (table_name,related_table,related_column) values ('table','storage_engine','null');" | sql.e 2>&1
) 2>&1 | grep -vi duplicate
exit 0

