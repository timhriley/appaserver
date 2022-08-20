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

if [ "$application" != "timriley" ]
then
	exit 0
fi

process="process"
operation="operation"
process_parameter="process_parameter"
javascript_processes="javascript_processes"
process_generic_output="process_generic_output"
javascript_files="javascript_files"
prompt="prompt"
process_groups="process_groups"
drop_down_prompt="drop_down_prompt"
drop_down_prompt_data="drop_down_prompt_data"
role_process="role_process"
role_process_set_member="role_process_set_member"
role_operation="role_operation"
(
cat << all_done
delete from $process where process = 'BofA_cash_load';
delete from $process where process = 'BofA_cash_upload';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('BofA_cash_load','feeder_load \$process \$login_name bank_of_america_checking filename 1 2 3 -1 4 reverse_order_yn account_end_balance execute_yn',null,null,null,null,'reconcile',null,null);
insert into $role_process (role,process) values ('bookkeeper','BofA_cash_load');
insert into $role_process (role,process) values ('supervisor','BofA_cash_load');
delete from $process_parameter where process = 'BofA_cash_load';
delete from $process_parameter where process = 'BofA_cash_upload';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('BofA_cash_load','null','null','execute_yn','null','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('BofA_cash_load','null','null','filename','null','1',null,null,null,null);
delete from $javascript_processes where process = 'BofA_cash_load';
delete from $process_generic_output where process = 'BofA_cash_load';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('reconcile');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'BofA_credit_load';
delete from $process where process = 'BofA_credit_upload';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('BofA_credit_load','feeder_load \$process \$login_name bank_of_america_credit_card filename 1 3 5 -1 -1 reverse_order_yn account_end_balance execute_yn',null,null,null,null,'reconcile',null,null);
insert into $role_process (role,process) values ('bookkeeper','BofA_credit_load');
insert into $role_process (role,process) values ('supervisor','BofA_credit_load');
delete from $process_parameter where process = 'BofA_credit_load';
delete from $process_parameter where process = 'BofA_credit_upload';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('BofA_credit_load','null','null','account_end_balance','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('BofA_credit_load','null','null','execute_yn','null','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('BofA_credit_load','null','null','filename','null','1',null,null,null,null);
delete from $javascript_processes where process = 'BofA_credit_load';
delete from $process_generic_output where process = 'BofA_credit_load';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('account_end_balance',null,null,null,'12');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('reconcile');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
