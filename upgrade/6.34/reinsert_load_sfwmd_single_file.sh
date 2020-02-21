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

if [ "$application" != "hydrology" -a "$application" != "audubon" ]
then
	exit 0
fi

process="process"
operation="operation"
process_parameter="process_parameter"
javascript_processes="javascript_processes"
process_generic_output="process_generic_output"
javascript_files="javascript_files"
prompt="prompt"
process_groups="process_groups"
drop_down_prompt="drop_down_prompt"
drop_down_prompt_data="drop_down_prompt_data"
(
cat << all_done
delete from $process where process = 'load_SFWMD_single_file';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('load_SFWMD_single_file','load_sfwmd_single_file \$process filename change_existing_data_yn execute_yn',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'load_SFWMD_single_file';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_SFWMD_single_file','null','null','change_existing_data_yn','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_SFWMD_single_file','null','null','execute_yn','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_SFWMD_single_file','null','null','filename','null','1',null,null,null,null);
delete from $javascript_processes where process = 'load_SFWMD_single_file';
delete from $process_generic_output where process = 'load_SFWMD_single_file';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
