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

# Returns 0 if FEEDER_PHRASE.feeder_phrase exists
# -----------------------------------------------
folder_attribute_exists.sh $application feeder_phrase feeder_phrase

if [ $? -ne 0 ]
then
	exit 0
fi

process="process"
operation="operation"
process_parameter="process_parameter"
process_generic_output="process_generic_output"
prompt="prompt"
process_groups="process_groups"
drop_down_prompt="drop_down_prompt"
drop_down_prompt_data="drop_down_prompt_data"
role_process="role_process"
role_process_set_member="role_process_set_member"
role_operation="role_operation"
(
cat << all_done
delete from $process where process = 'financial_position';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('financial_position','financial_position \$session \$login_name \$role \$process as_of_date prior_year_count subclassification_option output_medium',null,null,null,null,'output',null,null);
insert into $role_process (role,process) values ('bookkeeper','financial_position');
insert into $role_process (role,process) values ('supervisor','financial_position');
delete from $process_parameter where process = 'financial_position';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('financial_position','null','null','as_of_date','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('financial_position','null','null','null','finance_output_medium','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('financial_position','null','null','null','subclassification_option','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('financial_position','null','null','prior_year_count','null','2',null,null,null,null);
delete from $process_generic_output where process = 'financial_position';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('as_of_date',null,null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('prior_year_count',null,null,null,'3');
insert into $process_groups (process_group) values ('output');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('finance_output_medium',null,'output_medium');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','PDF','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','table','1');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('subclassification_option',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','aggregate','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','display','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','omit','3');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'statement_of_activities';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('statement_of_activities','statement_of_activities \$session \$login_name \$role \$process as_of_date prior_year_count  subclassification_option output_medium',null,null,null,null,'output',null,null);
insert into $role_process (role,process) values ('bookkeeper','statement_of_activities');
insert into $role_process (role,process) values ('supervisor','statement_of_activities');
delete from $process_parameter where process = 'statement_of_activities';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('statement_of_activities','null','null','as_of_date','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('statement_of_activities','null','null','null','finance_output_medium','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('statement_of_activities','null','null','null','subclassification_option','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('statement_of_activities','null','null','prior_year_count','null','2',null,null,null,null);
delete from $process_generic_output where process = 'statement_of_activities';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('as_of_date',null,null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('prior_year_count',null,null,null,'3');
insert into $process_groups (process_group) values ('output');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('finance_output_medium',null,'output_medium');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','PDF','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','table','1');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('subclassification_option',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','aggregate','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','display','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','omit','3');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'trial_balance';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('trial_balance','trial_balance_output \$session \$login_name \$role \$process as_of_date prior_year_count subclassification_option output_medium',null,null,null,null,'output',null,null);
insert into $role_process (role,process) values ('bookkeeper','trial_balance');
insert into $role_process (role,process) values ('supervisor','trial_balance');
delete from $process_parameter where process = 'trial_balance';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('trial_balance','null','null','as_of_date','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('trial_balance','null','null','null','finance_output_medium','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('trial_balance','null','null','null','trial_balance_subclassification_option','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('trial_balance','null','null','prior_year_count','null','2',null,null,null,null);
delete from $process_generic_output where process = 'trial_balance';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('as_of_date',null,null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('prior_year_count',null,null,null,'3');
insert into $process_groups (process_group) values ('output');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('finance_output_medium',null,'output_medium');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','PDF','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','table','1');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('trial_balance_subclassification_option',null,'subclassification_option');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('trial_balance_subclassification_option','display','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('trial_balance_subclassification_option','omit','2');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
