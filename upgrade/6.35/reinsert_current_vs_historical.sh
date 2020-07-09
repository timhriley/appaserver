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
delete from $process where process = 'current_vs_historical';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('current_vs_historical','output_current_vs_historical \$login_name \$session \$process initial station datatype current_year historical_range_years',null,null,null,null,'output',null,null);
delete from $process_parameter where process = 'current_vs_historical';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('current_vs_historical','null','null','current_year','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('current_vs_historical','null','null','historical_range_years','null','2',null,null,null,null);
delete from $javascript_processes where process = 'current_vs_historical';
delete from $process_generic_output where process = 'current_vs_historical';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('current_year',null,null,null,'4');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('historical_range_years',null,null,null,'3');
insert into $process_groups (process_group) values ('output');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
