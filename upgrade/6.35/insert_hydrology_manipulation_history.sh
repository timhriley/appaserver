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
delete from $process where process = 'measurement_value_manipulation_history';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('measurement_value_manipulation_history','measurement_value_manipulation_history \$process station datatype measurement_date measurement_time output_medium',null,null,null,null,'manipulate',null,null);
insert into $role_process (role,process) values ('hydrologist','measurement_value_manipulation_history');
insert into $role_process (role,process) values ('supervisor','measurement_value_manipulation_history');
insert into $role_process (role,process) values ('technician','measurement_value_manipulation_history');
delete from $process_parameter where process = 'measurement_value_manipulation_history';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('measurement_value_manipulation_history','measurement','null','null','null','1',null,null,'measurement_update_list',null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('measurement_value_manipulation_history','null','null','begin_measurement_date','null','2',null,'y',null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('measurement_value_manipulation_history','null','null','null','manipulation_history_output_medium','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('measurement_value_manipulation_history','station_datatype','null','null','null','1',null,'y',null,null);
delete from $javascript_processes where process = 'measurement_value_manipulation_history';
delete from $process_generic_output where process = 'measurement_value_manipulation_history';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_measurement_date',null,null,'y','10');
insert into $process_groups (process_group) values ('manipulate');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('manipulation_history_output_medium',null,'output_medium');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('manipulation_history_output_medium','table','1');
delete from $process where process = 'measurement_update_list';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('measurement_update_list','measurement_update_list.sh station datatype begin_measurement_date',null,null,null,null,null,null,null);
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
