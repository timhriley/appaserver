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
delete from process where process = 'budget_report';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('budget_report','budget_report \$session \$process output_medium',null,null,null,null,'output',null,null,null);
delete from role_process where process = 'budget_report';
insert into role_process (role,process) values ('supervisor','budget_report');
delete from process_parameter where process = 'budget_report';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('budget_report','null','null','output_medium','null','2',null,null,null,null);
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('output_medium',null,null);
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','PDF','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','spreadsheet','3');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','table','1');
insert into process_group (process_group) values ('output');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
