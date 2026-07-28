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

# Returns 0 if INVESTMENT_ACCOUNT.full_name exists
# ------------------------------------------------
table_column_exists.sh investment_account full_name

if [ $? -ne 0 ]
then
	exit 0
fi

(
cat << shell_all_done
delete from process where process = 'mark_to_market_investments';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,javascript_filename,preprompt_help_text) values ('mark_to_market_investments','account_balance_execute \$process fund_name as_of_date full_name contact_key investment_purpose execute_yn',null,null,null,null,'alter',null,null);
delete from role_process where process = 'mark_to_market_investments';
insert into role_process (role,process) values ('supervisor','mark_to_market_investments');
delete from process_parameter where process = 'mark_to_market_investments';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('mark_to_market_investments','null','null','null','execute_yn','9',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Omit execute for display.',null,null,'1');
insert into process_group (process_group) values ('alter');
delete from process where process = 'account_balance';
delete from role_process where process = 'account_balance';
delete from process_parameter where process = 'account_balance';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
