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

if [ "$application" != "hydrology" ]
then
	exit 0
fi

process="process"
operation="operation"
process_parameter="process_parameter"
process_generic_output="process_generic_output"
prompt="prompt"
process_groups="process_groups"
drop_down_prompt="drop_down_prompt"
drop_down_prompt_data="drop_down_prompt_data"
role_process="role_process"
role_process_set_member="role_process_set_member"
role_operation="role_operation"
(
cat << all_done
delete from $process where process = 'cubic_feet_per_second_moving_sum';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('cubic_feet_per_second_moving_sum','cubic_feet_per_second_moving_sum \$process \$role station datatype begin_date end_date days_to_sum output_medium units_converted',null,'process_moving_statistic_help.html',null,null,'output',null,'post_change_output_daily_moving_average(this)');
insert into $role_process (role,process) values ('supervisor','cubic_feet_per_second_moving_sum');
delete from $process_parameter where process = 'cubic_feet_per_second_moving_sum';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('cubic_feet_per_second_moving_sum','datatype','null','null','null','10',null,'y','datatype_cfs_list',null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('cubic_feet_per_second_moving_sum','null','null','begin_date','null','30',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('cubic_feet_per_second_moving_sum','null','null','days_to_sum','null','70',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('cubic_feet_per_second_moving_sum','null','null','end_date','null','40',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('cubic_feet_per_second_moving_sum','null','null','null','output_medium','100',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('cubic_feet_per_second_moving_sum','station_datatype','null','null','null','20','y',null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('cubic_feet_per_second_moving_sum','units_converted','null','null','null','90',null,null,null,null);
delete from $process_generic_output where process = 'cubic_feet_per_second_moving_sum';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('days_to_sum',null,null,null,'5');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $process_groups (process_group) values ('output');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('output_medium',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','chart','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','spreadsheet','4');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','table','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','text_file','3');
delete from $process where process = 'datatype_cfs_list';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('datatype_cfs_list','datatype_cfs_list.sh',null,null,null,null,null,null,null);
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
