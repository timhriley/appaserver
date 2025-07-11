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

if [ "$application" != "admin" ]
then
	exit 0
fi

(
echo "alter table post add random_number integer;" | tee_appaserver.sh | sql.e 2>&1
echo "insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('random_number','integer',5,null,null);" | sql.e 2>&1
echo "insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,insert_required_yn,lookup_required_yn,default_value) values ('post','random_number',null,3,null,null,null,null,null,null,null,null);" | sql.e 2>&1
echo "update table_column set display_order = 4 where table_name = 'post' and column_name = 'confirmation_received_date';" | sql.e 2>&1
echo "delete from role_table where table_name in ('email_address','post','post_contact','post_signup') and role = 'supervisor' and permission = 'insert';" | sql.e 2>&1
echo "update role_table set permission = 'lookup' where table_name in ('email_address','post','post_contact','post_signup') and role = 'supervisor' and permission = 'update';" | sql.e 2>&1
) 2>&1 | grep -vi duplicate
exit 0

