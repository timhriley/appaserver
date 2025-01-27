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


if [ "$application" != creel ]
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
delete from $process where process = 'update_sport_day_of_week';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('update_sport_day_of_week','update_sport_day_of_week filename execute_yn','This process is to fix an oversight. Prior to November 5, 2021, the Load Sport Fishing Trips process didn''t set the Day of Week column. Use this process to update the CREEL_CENSUS.day_of_week column. When the period of record is complete, tell Tim so he can remove this process from your menu.',null,null,null,null,'load',null);
insert into $role_process (role,process) values ('Supervisor','update_sport_day_of_week');
delete from $process_parameter where process = 'update_sport_day_of_week';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('update_sport_day_of_week','null','null','execute_yn','null','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('update_sport_day_of_week','null','null','filename','null','1',null,null,null,null);
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
