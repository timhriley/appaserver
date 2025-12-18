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
delete from process where process = 'BofA_checking_upload';
insert into process (process,command_line,process_group,notepad,post_change_javascript,javascript_filename,execution_count,preprompt_help_text,html_help_file_anchor,process_set_display) values ('BofA_checking_upload','feeder_upload_csv \$process \$login_name fund_name bank_of_america_checking filename 1 2 3 credit_column 4 reference_column reverse_order_yn account_end_balance execute_yn','BofA','<ol><li>Download your checking transactions from Bank of America.<li>Choose the file type of Excel.<li>Hint: do not first load them into Excel and then save them to your computer. Instead, save them and then, optionally, load them into Excel.<li> Use a begin date as the end date of your last download. The end date of your last download will display if you submit this form without a file. Use \'Final Feeder Date\' as the begin date.<li>Hint: [Right Click] the browser\'s tab above and select \'Duplicate Tab\'. Use the new PredictBooks instance to:<ul><li>Feeder --> Insert Expense Transaction or<li>Feeder --> Insert Feeder Phrase</ul></ol>',null,null,'50',null,null,null);
delete from process_parameter where process = 'BofA_checking_upload';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('BofA_checking_upload','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('BofA_checking_upload','null','null','null','filename','1',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Omit execute for display.',null,null,'1');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into process_group (process_group) values ('BofA');
delete from process where process = 'BofA_credit_upload';
insert into process (process,command_line,process_group,notepad,post_change_javascript,javascript_filename,execution_count,preprompt_help_text,html_help_file_anchor,process_set_display) values ('BofA_credit_upload','feeder_upload_csv \$process \$login_name fund_name bank_of_america_credit_card filename 1 3 5 credit_column balance_column 2 y account_end_balance execute_yn','BofA','The bank\'s begin date will display if you submit this form without a file. Use \'Final Feeder Date\' as the begin date.',null,null,'37',null,null,null);
delete from process_parameter where process = 'BofA_credit_upload';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('BofA_credit_upload','null','null','null','account_end_balance','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('BofA_credit_upload','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('BofA_credit_upload','null','null','null','filename','1',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('account_end_balance','For Credit Card accounts, this will be a negative number.',null,null,'10');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Omit execute for display.',null,null,'1');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into process_group (process_group) values ('BofA');
delete from process where process = 'feeder_upload';
insert into process (process,command_line,process_group,notepad,post_change_javascript,javascript_filename,execution_count,preprompt_help_text,html_help_file_anchor,process_set_display) values ('feeder_upload','feeder_upload \$process \$login_name fund_name feeder_account exchange_format_filename execute_yn','feeder','<ol><li>Download your transactions from your Financial Institution using a begin date as the end date of your last download. The end date of your last download will display if you submit this form without a file. Use \'Final Feeder Date\' as the begin date.<li>Hint: [Right Click] the browser\'s tab above and select \'Duplicate Tab\'. Use the new PredictBooks instance to:<ul><li>Feeder --> Insert Expense Transaction or<li>Feeder --> Insert Feeder Phrase</ul></ol>',null,null,'73',null,null,null);
delete from process_parameter where process = 'feeder_upload';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('feeder_upload','feeder_account','null','null','null','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('feeder_upload','null','null','null','exchange_format_filename','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('feeder_upload','null','null','null','execute_yn','9',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('exchange_format_filename','Log into your bank\'s website and download your transactions. Choose the format option likely to be the Exchange Format. It will probably be the top row in the format selection list. It will also probably have a filename that ends with xf.','y',null,'255');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Omit execute for display.',null,null,'1');
insert into process_group (process_group) values ('feeder');
delete from process where process = 'initialize_BofA_checking_account';
insert into process (process,command_line,process_group,notepad,post_change_javascript,javascript_filename,execution_count,preprompt_help_text,html_help_file_anchor,process_set_display) values ('initialize_BofA_checking_account','feeder_init_csv_execute \$session \$login \$role \$process fund_name bank_of_america \'\' checking filename 1 2 3 credit_column 4 reference_column reverse_order_yn account_end_balance execute_yn','BofA','<ol><li>Bank of America does not support the Open Financial eXchange file format. Therefore, on the bank\'s website, you must select the Excel output option.<li>This process generates an opening entry transaction.<li>The transaction is assigned to the entity stored in the Self table. When you first install Appaserver, the Self table has a placeholder full name. Change this placeholder to your full name. The menu path is:<br>Lookup --> Entity --> Self<li>This process creates an asset account. The account name is a variation of bank_of_america. When you first import PredictBooks, the Financial Institution table has a placeholder full name. Change this placeholder to bank_of_america. The menu path is:<br>Lookup --> Entity --> Financial Institution<li>This process also generates useful bookkeeping tips. You can view the tips without generating an opening entry by leaving the Filename blank.</ol>',null,null,null,null,null,null);
delete from process_parameter where process = 'initialize_BofA_checking_account';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('initialize_BofA_checking_account','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('initialize_BofA_checking_account','null','null','null','filename','1',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Omit execute for display.',null,null,'1');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into process_group (process_group) values ('BofA');
delete from process where process = 'initialize_BofA_credit_account';
insert into process (process,command_line,process_group,notepad,post_change_javascript,javascript_filename,execution_count,preprompt_help_text,html_help_file_anchor,process_set_display) values ('initialize_BofA_credit_account','feeder_init_csv_execute \$session \$login \$role \$process fund_name bank_of_america \'\' checking filename 1 2 3 credit_column 4 reference_column reverse_order_yn account_end_balance execute_yn','BofA','<ol><li>Bank of America does not support the Open Financial eXchange file format. Therefore, on the bank\'s website, you must select the Excel output option.<li>This process generates an opening entry transaction.<li>The transaction is assigned to the entity stored in the Self table. When you first install Appaserver, the Self table has a placeholder full name. Change this placeholder to your full name. The menu path is:<br>Lookup --> Entity --> Self<li>This process creates a liability account. The account name is a variation of bank_of_america. When you first import PredictBooks, the Financial Institution table has a placeholder full name. Change this placeholder to bank_of_america. The menu path is:<br>Lookup --> Entity --> Financial Institution<li>This process also generates useful bookkeeping tips. You can view the tips without generating an opening entry by leaving at least one choice blank.</ol>',null,null,null,null,null,null);
delete from process_parameter where process = 'initialize_BofA_credit_account';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('initialize_BofA_credit_account','null','null','null','account_end_balance','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('initialize_BofA_credit_account','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('initialize_BofA_credit_account','null','null','null','filename','1',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('account_end_balance','For Credit Card accounts, this will be a negative number.',null,null,'10');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Omit execute for display.',null,null,'1');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into process_group (process_group) values ('BofA');
delete from process where process = 'initialize_feeder_account';
insert into process (process,command_line,process_group,notepad,post_change_javascript,javascript_filename,execution_count,preprompt_help_text,html_help_file_anchor,process_set_display) values ('initialize_feeder_account','feeder_init_execute \$session \$login \$role \$process fund_name full_name street_address account_type exchange_format_filename execute_yn','feeder','<ol><li>This process generates an opening entry transaction.<li>The transaction is assigned to the entity stored in the Self table. When you first install Appaserver, the Self table has a placeholder full name. Change this placeholder to your full name. The menu path is:<br>Lookup --> Entity --> Self<li>This process creates: <ul><li>an asset account, if the Account Type is Checking or<li>a liability account, if the Account Type is Credit Card</ul> The account name is derived from the financial institution\'s full name. When you first import PredictBooks, the Financial Institution table has a placeholder full name. Change this placeholder to your financial institution\'s full name. The menu path is:<br>Lookup --> Entity --> Financial Institution<li>This process also generates useful bookkeeping tips. You can view the tips without generating an opening entry by leaving at least one choice blank.</ol>',null,null,null,null,null,null);
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
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
