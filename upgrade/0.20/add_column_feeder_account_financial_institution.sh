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
echo "alter table feeder_account add full_name char (60);" | tee_appaserver.sh | sql.e 2>&1
echo "insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,insert_required_yn,lookup_required_yn,default_value) values ('feeder_account','full_name',null,1,null,null,null,null,null,null,null,null);" | sql.e 2>&1

echo "alter table feeder_account add street_address char (60);" | tee_appaserver.sh | sql.e 2>&1
echo "insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,insert_required_yn,lookup_required_yn,default_value) values ('feeder_account','street_address',null,2,null,null,null,null,null,null,null,null);" | sql.e 2>&1

echo "insert into relation (table_name,related_table,related_column) values ('feeder_account','financial_institution','null');" | sql.e 2>&1

echo "update process set command_line = 'feeder_init_execute \$session \$login \$role \$process full_name street_address account_type exchange_format_filename execute_yn' where process = 'initialize_feeder_account';" | sql.e

echo "update feeder_phrase set full_name = null, street_address = null where feeder_phrase = 'interest';" | sql.e

echo "update process set notepad = '<ul><li>This is a shortcut process instead of executing:<br>Insert --> Transaction --> Transaction<li>If your account isn''t in the list below, then insert it using the path:<br>Insert --> Transaction --> Account.</ul>' where process = 'insert_expense_transaction';" | sql.e

) 2>&1 | grep -vi duplicate

exit 0

