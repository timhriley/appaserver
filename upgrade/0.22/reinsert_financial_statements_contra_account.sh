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
delete from process where process = 'financial_position';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('financial_position','financial_position \$session \$login_name \$role \$process as_of_date prior_year_count subclassification_option output_medium contra_account_yn fund_name',null,null,null,null,'output',null,null,null);
delete from process_parameter where process = 'financial_position';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('financial_position','null','null','finance_output_medium','null','5',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('financial_position','null','null','null','as_of_date','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('financial_position','null','null','null','contra_account_yn','4',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('financial_position','null','null','null','prior_year_count','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('financial_position','null','null','subclassification_option','null','3',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('as_of_date',null,null,'y','10');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('contra_account_yn',null,null,null,'1');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('prior_year_count',null,null,null,'3');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('finance_output_medium',null,'output_medium');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('subclassification_option',null,null);
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','PDF','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','table','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','aggregate','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','display','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','omit','3');
insert into process_group (process_group) values ('output');
delete from process where process = 'statement_of_activities';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('statement_of_activities','statement_of_activities \$session \$login_name \$role \$process as_of_date prior_year_count  subclassification_option output_medium contra_account_yn fund_name',null,null,null,null,'output',null,null,null);
delete from process_parameter where process = 'statement_of_activities';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('statement_of_activities','null','null','finance_output_medium','null','5',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('statement_of_activities','null','null','null','as_of_date','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('statement_of_activities','null','null','null','contra_account_yn','4',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('statement_of_activities','null','null','null','prior_year_count','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('statement_of_activities','null','null','subclassification_option','null','3',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('as_of_date',null,null,'y','10');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('contra_account_yn',null,null,null,'1');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('prior_year_count',null,null,null,'3');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('finance_output_medium',null,'output_medium');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('subclassification_option',null,null);
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','PDF','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','table','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','aggregate','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','display','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','omit','3');
insert into process_group (process_group) values ('output');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
