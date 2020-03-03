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
delete from $process where process = 'output_merged_waterquality';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('output_merged_waterquality','output_merged_waterquality \$process waterquality_station waterquality_parameter waterquality_units hydrology_station hydrology_datatype begin_date end_date output_medium',null,null,null,null,'waterquality',null,null);
insert into $role_process (role,process) values ('supervisor','output_merged_waterquality');
delete from $process_parameter where process = 'output_merged_waterquality';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_waterquality','null','null','begin_date','null','3',null,'n',null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_waterquality','null','null','end_date','null','4',null,'n',null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_waterquality','null','null','null','merge_waterquality_output_medium','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_waterquality','null','null','null','waterquality_parameter','1',null,'y','populate_waterquality_parameter',null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_waterquality','null','null','null','waterquality_station','2',null,'y','populate_waterquality_station',null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_waterquality','null','null','null','waterquality_station_parameter','1','y',null,'populate_waterquality_station_parameter',null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_waterquality','station','null','null','null','3',null,'y',null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_waterquality','station_datatype','null','null','null','2','y','n',null,null);
delete from $javascript_processes where process = 'output_merged_waterquality';
delete from $process_generic_output where process = 'output_merged_waterquality';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $process_groups (process_group) values ('waterquality');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('merge_waterquality_output_medium',null,'output_medium');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('merge_waterquality_output_medium','gracechart','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('merge_waterquality_output_medium','spreadsheet','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('merge_waterquality_output_medium','table','1');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('waterquality_parameter',null,null);
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('waterquality_station',null,null);
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('waterquality_station_parameter',null,null);
delete from $process where process = '';
delete from $process where process = 'populate_waterquality_parameter';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('populate_waterquality_parameter','populate_waterquality_parameter.sh',null,null,null,null,null,null,null);
delete from $process where process = 'populate_waterquality_station';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('populate_waterquality_station','populate_waterquality_station.sh',null,null,null,null,null,null,null);
delete from $process where process = 'populate_waterquality_station_parameter';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('populate_waterquality_station_parameter','populate_waterquality_station_parameter.sh waterquality_station waterquality_parameter',null,null,null,null,null,null,null);
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
