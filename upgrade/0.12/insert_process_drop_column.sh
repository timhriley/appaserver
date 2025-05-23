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
delete from process where process = 'drop_column';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('drop_column','drop_column \$application \$session \$login_name \$role \$process table_name column_name content_type_yn execute_yn',null,null,null,'alter',null,null,null);
insert into role_process (role,process) values ('system','drop_column');
delete from process_parameter where process = 'drop_column';
insert into process_parameter (process,table_name,column_name,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('drop_column','table','null','null','null','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('drop_column','null','null','column_name','null','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('drop_column','null','null','execute_yn','null','9',null,null,null,null);
insert into prompt ( prompt, input_width ) values ( 'column_name', 60 );
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
