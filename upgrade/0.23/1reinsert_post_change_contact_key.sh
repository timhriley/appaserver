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
delete from process where process = 'account_balance';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('account_balance','account_balance.sh \$process as_of_date full_name contact_key investment_purpose execute_yn',null,null,null,null,'output',null,null,null);
delete from role_process where process = 'account_balance';
insert into role_process (role,process) values ('supervisor','account_balance');
delete from process_parameter where process = 'account_balance';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('account_balance','financial_institution','null','null','null','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('account_balance','investment_purpose','null','null','null','3',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('account_balance','null','null','null','as_of_date','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('account_balance','null','null','null','execute_yn','9',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('as_of_date',null,null,'y','10');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Omit execute for display.',null,null,'1');
insert into process_group (process_group) values ('output');
delete from process where process = 'initialize_feeder_account';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('initialize_feeder_account','feeder_init_execute \$session \$login \$role \$process fund_name full_name contact_key account_type exchange_format_filename execute_yn','<ol><li>This process generates an opening entry transaction.<li>The transaction is assigned to the entity stored in the Self table. When you first install Appaserver, the Self table has a placeholder full name. Change this placeholder to your full name. The menu path is:<br>Lookup --> Entity --> Self<li>This process creates: <ul><li>an asset account, if the Account Type is Checking or<li>a liability account, if the Account Type is Credit Card</ul> The account name is derived from the financial institution\'s full name. When you first import PredictBooks, the Financial Institution table has a placeholder full name. Change this placeholder to your financial institution\'s full name. The menu path is:<br>Lookup --> Entity --> Financial Institution<li>This process also generates useful bookkeeping tips. You can view the tips without generating an opening entry by leaving at least one choice blank.</ol>',null,null,null,'feeder',null,null,null);
delete from role_process where process = 'initialize_feeder_account';
insert into role_process (role,process) values ('supervisor','initialize_feeder_account');
delete from process_parameter where process = 'initialize_feeder_account';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('initialize_feeder_account','financial_institution','null','null','null','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('initialize_feeder_account','null','null','account_type','null','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('initialize_feeder_account','null','null','null','exchange_format_filename','3',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('initialize_feeder_account','null','null','null','execute_yn','9',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('exchange_format_filename','Log into your bank\'s website and download your transactions. Choose the format option likely to be the Exchange Format. It will probably be the top row in the format selection list. It will also probably have a filename that ends with xf.','y',null,'255');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Omit execute for display.',null,null,'1');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('account_type',null,null);
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('account_type','checking','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('account_type','credit_card','2');
insert into process_group (process_group) values ('feeder');
delete from process where process = 'insert_expense_transaction';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('insert_expense_transaction','insert_expense_transaction.sh \$process feeder_account full_name contact_key transaction_date account transaction_amount check_number memo','<ul><li>This is a shortcut process instead of executing:<br>Insert --> Transaction --> Transaction<li>If your account isn\'t in the list below, then insert it using the path:<br>Insert --> Transaction --> Account.</ul>',null,null,'post_change_insert_expense_transaction( this )','feeder',null,'post_change_insert_expense_transaction.js',null);
delete from role_process where process = 'insert_expense_transaction';
insert into role_process (role,process) values ('supervisor','insert_expense_transaction');
delete from process_parameter where process = 'insert_expense_transaction';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_expense_transaction','account','null','null','null','4',null,null,'populate_expense_account',null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_expense_transaction','entity','null','null','null','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_expense_transaction','feeder_account','null','null','null','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_expense_transaction','null','null','null','check_number','7',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_expense_transaction','null','null','null','full_name','3',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_expense_transaction','null','null','null','memo','8',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_expense_transaction','null','null','null','transaction_amount','6',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_expense_transaction','null','null','null','transaction_date','5',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('check_number',null,null,null,'10');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('full_name',null,null,null,'60');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('memo',null,null,null,'60');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('transaction_amount',null,null,null,'10');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('transaction_date',null,null,'y','10');
insert into process_group (process_group) values ('feeder');
insert into process (process,command_line) values ('populate_expense_account','populate_expense_account.sh');
delete from process where process = 'pay_liabilities';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('pay_liabilities','pay_liabilities_process \$process \$session account full_name contact_key starting_check_number memo payment_amount execute_yn','<h2>Print Checks</h2><ol><li>Open in Evince, also known as Document Viewer (recommended)<li>Papersize width = 8.5 inches<li>Papersize height = 3 inches<li>Orientation = Portrait<li>Page order = Back to front<li>Reverse = Yes<li>Turn off scaling and other automatic formatting<li>Set all margins to zero<li>Place first check face down on white paper<li>Print the checks<li>Print a blank page to eject the last check</ol>',null,null,'post_change_pay_liabilities( this )','alter',null,'post_change_pay_liabilities.js',null);
delete from role_process where process = 'pay_liabilities';
insert into role_process (role,process) values ('supervisor','pay_liabilities');
delete from process_parameter where process = 'pay_liabilities';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('pay_liabilities','account','null','null','null','1',null,null,'populate_checking_account',null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('pay_liabilities','entity','null','null','null','2','y',null,'populate_print_checks_entity',null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('pay_liabilities','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('pay_liabilities','null','null','null','memo','5',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('pay_liabilities','null','null','null','payment_amount','4',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('pay_liabilities','null','null','null','starting_check_number','3',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Omit execute for display.',null,null,'1');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('memo',null,null,null,'60');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('payment_amount',null,null,null,'10');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('starting_check_number',null,null,null,'5');
insert into process_group (process_group) values ('alter');
insert into process (process,command_line) values ('populate_checking_account','populate_checking_account.sh');
insert into process (process,command_line) values ('populate_print_checks_entity','populate_print_checks_entity');
delete from process where process = 'post_change_account_balance';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('post_change_account_balance','post_change_account_balance full_name contact_key account_number date \$state',null,null,null,null,null,null,null,null);
delete from role_process where process = 'post_change_account_balance';
delete from process_parameter where process = 'post_change_account_balance';
delete from process where process = 'prior_fixed_asset_trigger';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('prior_fixed_asset_trigger','prior_fixed_asset_trigger asset_name \$state preupdate_full_name preupdate_contact_key preupdate_purchase_date_time',null,null,null,null,null,null,null,null);
delete from role_process where process = 'prior_fixed_asset_trigger';
delete from process_parameter where process = 'prior_fixed_asset_trigger';
delete from process where process = 'self_trigger';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('self_trigger','self_trigger \$state full_name contact_key preupdate_credit_card_number credit_card_number',null,null,null,null,null,null,null,null);
delete from role_process where process = 'self_trigger';
delete from process_parameter where process = 'self_trigger';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
