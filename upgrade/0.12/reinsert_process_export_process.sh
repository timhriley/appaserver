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
delete from process where process = 'export_process';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('export_process','export_process \$process process exclude_roles_yn','For a list of processes, this process exports from the following: process, role_process, process_parameter, prompt, drop_down_prompt, and drop_down_prompt_data.',null,'10',null,'output',null,null,null);
delete from process_parameter where process = 'export_process';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('export_process','null','null','null','exclude_roles_yn','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('export_process','process','null','null','null','1','y',null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('exclude_roles_yn',null,null,null,'1');
insert into process_group (process_group) values ('output');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
