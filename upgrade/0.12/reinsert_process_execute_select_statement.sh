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
delete from process where process = 'execute_select_statement';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('execute_select_statement','execute_select_statement \$process \$session \$role select_statement_title login_name statement',null,null,null,null,'output',null,null,null);
delete from process_parameter where process = 'execute_select_statement';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('execute_select_statement','null','null','null','statement','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('execute_select_statement','select_statement','null','null','null','1',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('statement',null,null,null,'2048');
insert into process_group (process_group) values ('output');
insert into role_process (role,process) values ('system','execute_select_statement');
insert into role_process (role,process) values ('supervisor','execute_select_statement');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
