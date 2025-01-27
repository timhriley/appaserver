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
delete from process where process = 'import_predictbooks';
insert into process (process,command_line,notepad,execution_count,process_group,post_change_javascript,javascript_filename,preprompt_help_text,html_help_file_anchor,process_set_display) values ('import_predictbooks','import_predictbooks \$process full_name name_of_bank checking_begin_balance execute_yn','This process imports the PredictBooks home edition.',null,'alter','post_change_import_predictbooks()','post_change_import_predictbooks.js',null,null,null);
delete from role_process where process = 'import_predictbooks';
insert into role_process (role,process) values ('supervisor','import_predictbooks');
delete from process_parameter where process = 'import_predictbooks';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('import_predictbooks','financial_institution','null','null','null','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('import_predictbooks','null','null','null','checking_begin_balance','3',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('import_predictbooks','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('import_predictbooks','null','null','null','name_of_bank','2',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('checking_begin_balance','<ol><li>Enter the earliest checking account balance from your bank\'s website.<li>Download all of your checking transactions from your bank.<li>Execute the soon to be installed checking upload process.</ol>',null,null,'14');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('name_of_bank','Enter the name of your bank if it\'s not in the list above.',null,null,'30');
insert into process_group (process_group) values ('alter');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
