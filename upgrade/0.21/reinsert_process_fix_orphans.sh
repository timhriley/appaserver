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
delete from process where process = 'fix_orphans';
insert into process (process,command_line,process_group,notepad,post_change_javascript,javascript_filename,execution_count,preprompt_help_text,html_help_file_anchor,process_set_display) values ('fix_orphans','fix_orphans \$process delete_yn execute_yn','alter','This process traverses the many-to-one relationships for each table that has them. If a one table is missing a related row, then the many table has an orphan. This process either:<ol><li>Inserts the missing row in the one table.<li>Deletes the orphaned row in the many table.</ol>',null,null,null,null,null,null);
delete from process_parameter where process = 'fix_orphans';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('fix_orphans','null','null','null','delete_yn','5',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('fix_orphans','null','null','null','execute_yn','9',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('delete_yn',null,null,null,'1');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','\'Yes\' will alter the database; otherwise, a hint message will display.',null,null,'1');
insert into process_group (process_group) values ('alter');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
