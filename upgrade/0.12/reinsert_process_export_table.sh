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

(
cat << shell_all_done
delete from process where process = 'export_table';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('export_table','export_table \$process table_name export_output',null,null,'5',null,'output',null,null,null);
delete from role_process where process = 'export_table';
insert into role_process (role,process) values ('system','export_table');
delete from process_parameter where process = 'export_table';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('export_table','null','null','export_output','null','3',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('export_table','table','null','null','null','2',null,null,null,null);
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('export_output',null,null);
insert into process_group (process_group) values ('output');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
