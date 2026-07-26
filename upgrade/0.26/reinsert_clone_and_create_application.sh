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
delete from process where process = 'clone_application';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,javascript_filename,preprompt_help_text) values ('clone_application','clone_application \$login_name \$process destination_application new_application_title execute_yn','This process creates a new application that is a clone of the current application. For security on a public facing website, the source code must be compiled with NON_TEMPLATE_APPLICATION_OKAY set to 1.',null,null,null,'alter',null,null);
delete from role_process where process = 'clone_application';
insert into role_process (role,process) values ('system','clone_application');
delete from process_parameter where process = 'clone_application';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('clone_application','null','null','null','destination_application','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('clone_application','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('clone_application','null','null','null','new_application_title','2',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('destination_application',null,null,null,'30');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Omit execute for display.',null,null,'1');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('new_application_title',null,null,null,'30');
insert into process_group (process_group) values ('alter');
delete from process where process = 'create_application';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,javascript_filename,preprompt_help_text) values ('create_application','create_application \$process execute_yn',null,null,null,null,'alter',null,null);
delete from role_process where process = 'create_application';
insert into role_process (role,process) values ('system','create_application');
delete from process_parameter where process = 'create_application';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('create_application','null','null','null','execute_yn','1',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Omit execute for display.',null,null,'1');
insert into process_group (process_group) values ('alter');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
