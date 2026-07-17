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

# Returns 0 if ELEMENT.element exists
# -----------------------------------
table_column_exists.sh element element

if [ $? -ne 0 ]
then
	exit 0
fi

(
echo "alter table transaction add transaction_lock_yn char (1);" | tee_appaserver.sh | sql.e 2>&1
echo "insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('transaction_lock_yn','character',1,null,null);" | sql.e 2>&1
echo "insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,insert_required_yn,lookup_required_yn,default_value) values ('transaction','transaction_lock_yn',null,4,'y',null,'y',null,null,null,null,null);" | sql.e 2>&1
echo "insert into row_security_role_update (table_name,column_not_null) values ('transaction','transaction_lock_yn');" | sql.e 2>&1
echo "update role set override_row_restrictions_yn = 'n' where role = 'supervisor';" | sql.e 2>&1
) 2>&1 | grep -vi duplicate
exit 0

