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
delete from process where process = 'insert_feeder_phrase';
insert into process (process,command_line,process_group,notepad,post_change_javascript,javascript_filename,execution_count,preprompt_help_text,html_help_file_anchor,process_set_display) values ('insert_feeder_phrase','insert_feeder_phrase.sh \$process account full_name feeder_phrase','feeder','<ul><li>This is a shortcut process instead of executing Insert --> Feeder --> Feeder Phrase<li>If your account isn\'t in the list below, then insert it using the path:<br>Insert --> Transaction --> Account.</ul>',null,null,null,null,null,null);
delete from process_parameter where process = 'insert_feeder_phrase';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_feeder_phrase','account','null','null','null','3',null,null,'populate_expense_revenue_account',null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_feeder_phrase','null','null','null','feeder_phrase','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_feeder_phrase','null','null','null','full_name','2',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('feeder_phrase',null,null,null,'80');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('full_name',null,null,null,'60');
insert into process_group (process_group) values ('feeder');
insert into process (process,command_line) values ('populate_expense_revenue_account','populate_expense_revenue_account.sh');
shell_all_done
) | sql.e 2>&1 | grep -vi duplicate

exit 0
