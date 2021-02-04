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
role_process="role_process"
role_process_set_member="role_process_set_member"
role_operation="role_operation"
(
cat << all_done

delete from $process where process = 'midnight_measurements';
delete from $role_process where process = 'midnight_measurements';
delete from $process_parameter where process = 'midnight_measurements';
delete from $drop_down_prompt where drop_down_prompt = 'midnight_output_medium';
delete from $drop_down_prompt_data where drop_down_prompt = 'midnight_output_medium';

delete from $process where process = 'on_the_hour_measurements';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('on_the_hour_measurements','on_the_hour_measurements \$process station datatype begin_date end_date output_medium','This process outputs every measurement with measurement_time like ''%00''',null,null,null,'output',null,null);
insert into $role_process (role,process) values ('hydrologist','on_the_hour_measurements');
insert into $role_process (role,process) values ('public','on_the_hour_measurements');
insert into $role_process (role,process) values ('supervisor','on_the_hour_measurements');
insert into $role_process (role,process) values ('technician','on_the_hour_measurements');
delete from $process_parameter where process = 'on_the_hour_measurements';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('on_the_hour_measurements','datatype','null','null','null','2','y','y',null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('on_the_hour_measurements','null','null','begin_date','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('on_the_hour_measurements','null','null','end_date','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('on_the_hour_measurements','null','null','null','on_the_hour_output_medium','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('on_the_hour_measurements','station','null','null','null','1','y','y',null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('on_the_hour_measurements','station_datatype','null','null','null','1','n',null,null,null);
delete from $javascript_processes where process = 'on_the_hour_measurements';
delete from $process_generic_output where process = 'on_the_hour_measurements';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $process_groups (process_group) values ('output');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('on_the_hour_output_medium',null,'output_medium');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('on_the_hour_output_medium','chart','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('on_the_hour_output_medium','spreadsheet','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('on_the_hour_output_medium','table','1');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
