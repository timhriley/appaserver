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
cat << shell_all_done
delete from process where process = 'insert_cash_expense_transaction';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('insert_cash_expense_transaction','insert_cash_expense.sh \$process full_name street_address transaction_date account transaction_amount check_number memo program_name',null,null,null,'post_change_insert_cash_transaction( this )','reconcile',null,'post_change_insert_cash_transaction.js',null);
delete from role_process where process = 'insert_cash_expense_transaction';
insert into role_process (role,process) values ('supervisor','insert_cash_expense_transaction');
delete from process_parameter where process = 'insert_cash_expense_transaction';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','account','null','null','null','5',null,null,'populate_expense_account',null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','entity','null','null','null','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','null','null','null','check_number','8',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','null','null','null','full_name','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','null','null','null','memo','9',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','null','null','null','street_address','3',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','null','null','null','transaction_amount','7',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','null','null','null','transaction_date','6',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('check_number',null,null,null,'10');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('full_name',null,null,null,'60');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('memo',null,null,null,'60');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('street_address',null,null,null,'60');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('transaction_amount',null,null,null,'10');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('transaction_date',null,null,'y','10');
insert into process_group (process_group) values ('reconcile');
insert into process (process,command_line) values ('populate_expense_account','populate_expense_account.sh');
delete from process where process = 'insert_cash_revenue_transaction';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('insert_cash_revenue_transaction','insert_cash_revenue.sh \$process full_name street_address transaction_date account transaction_amount memo program_name',null,null,null,'post_change_insert_cash_transaction( this )','reconcile',null,'post_change_insert_cash_transaction.js',null);
delete from role_process where process = 'insert_cash_revenue_transaction';
insert into role_process (role,process) values ('supervisor','insert_cash_revenue_transaction');
delete from process_parameter where process = 'insert_cash_revenue_transaction';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_revenue_transaction','account','null','null','null','5',null,null,'populate_revenue_account',null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_revenue_transaction','entity','null','null','null','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_revenue_transaction','null','null','null','full_name','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_revenue_transaction','null','null','null','memo','9',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_revenue_transaction','null','null','null','street_address','3',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_revenue_transaction','null','null','null','transaction_amount','7',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_revenue_transaction','null','null','null','transaction_date','6',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('full_name',null,null,null,'60');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('memo',null,null,null,'60');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('street_address',null,null,null,'60');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('transaction_amount',null,null,null,'10');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('transaction_date',null,null,'y','10');
insert into process_group (process_group) values ('reconcile');
insert into process (process,command_line) values ('populate_revenue_account','populate_revenue_account.sh');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
