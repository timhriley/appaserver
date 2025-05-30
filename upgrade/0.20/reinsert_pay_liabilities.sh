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
delete from process where process = 'pay_liabilities';
insert into process (process,command_line,process_group,notepad,post_change_javascript,javascript_filename,execution_count,preprompt_help_text,html_help_file_anchor,process_set_display) values ('pay_liabilities','pay_liabilities_process \$process \$session account full_name street_address starting_check_number memo payment_amount execute_yn','alter','<h2>Print Checks</h2><ol><li>Open in Evince, also known as Document Viewer (recommended)<li>Papersize width = 8.5 inches<li>Papersize height = 3 inches<li>Orientation = Portrait<li>Page order = Back to front<li>Reverse = Yes<li>Place first check face down on white paper<li>Print the checks<li>Print a blank page to eject the last check</ol>','post_change_pay_liabilities( this )','post_change_pay_liabilities.js',null,null,null,null);
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
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
