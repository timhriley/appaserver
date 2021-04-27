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

# Returns 0 if BANK_UPLOAD.bank_date exists
# -----------------------------------------
folder_attribute_exists.sh $application bank_upload bank_date

if [ $? -ne 0 ]
then
	exit 1
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
(
cat << all_done
delete from $process where process = 'trial_balance';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('trial_balance','trial_balance \$session \$login_name \$role \$process fund as_of_date prior_year_count aggregation subclassification_option output_medium',null,null,null,null,null,'output',null);
delete from $process_parameter where process = 'trial_balance';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('trial_balance','null','null','as_of_date','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('trial_balance','null','null','null','finance_output_medium','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('trial_balance','null','null','null','trial_balance_subclassification_option','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('trial_balance','null','null','prior_year_count','null','2',null,null,null,null);
delete from $javascript_processes where process = 'trial_balance';
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
