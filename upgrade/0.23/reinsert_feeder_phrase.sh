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
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('insert_feeder_phrase','insert_feeder_phrase \$process account new_full_name feeder_phrase','<ul><li>This is a shortcut process instead of executing:<br>Insert --> Feeder --> Feeder Phrase<li>If your account isn\'t in the list below, then insert it using the path:<br>Insert --> Transaction --> Account.<li>Both the feeder phrase and the full name may be the vendor\'s name copied from the feeder file\'s description cell.<br>However, the feeder phrase may need to include an adjoining keyword to remove ambiguity.<li>If the vendor already exists as an entity and already has an existing, but sightly different, feeder phrase, then:<ol><li>Lookup --> Feeder --> Feeder Phrase<li>query the entity<li>set the additional feeder phrase to the right of the existing feeder phrase, separated with a \'|\'</ol></ul>',null,null,null,'feeder',null,null,null);
delete from process_parameter where process = 'insert_feeder_phrase';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_feeder_phrase','account','null','null','null','3',null,null,'populate_expense_revenue_account',null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_feeder_phrase','null','null','null','feeder_phrase','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('insert_feeder_phrase','null','null','null','new_full_name','2',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('feeder_phrase',null,null,null,'80');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('new_full_name',null,null,null,'60');
insert into process_group (process_group) values ('feeder');
insert into process (process,command_line) values ('populate_expense_revenue_account','populate_expense_revenue_account.sh');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
