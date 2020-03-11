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
delete from $process where process = 'contour_map';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('contour_map','mapping.sh \$process begin_date end_date station datatype n',null,null,null,null,'output',null,null);
delete from $process_parameter where process = 'contour_map';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('contour_map','favorite_station_set','null','null','null','1',null,'y',null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('contour_map','null','null','begin_date','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('contour_map','null','null','end_date','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('contour_map','station_datatype','null','null','null','3','y',null,null,null);
delete from $javascript_processes where process = 'contour_map';
delete from $process_generic_output where process = 'contour_map';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $process_groups (process_group) values ('output');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
