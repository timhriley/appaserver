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

if [ "$application" != "creel" ]
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
delete from $process where process = 'load_guide_angler_submission';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('load_guide_angler_submission','load_guide_angler_submission \$process \$login_name filename execute_yn','Template: A=Angler_name, B=Permit_code, C=Census_date, D=Fishing_duration, E=Fishing_count, F=Targeted_common_name, I=Caught_common_name(starting), L=Fishing_area(starting), M=Fishing_zone(starting)',null,null,null,null,'load',null);
delete from $process_parameter where process = 'load_guide_angler_submission';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_guide_angler_submission','null','null','execute_yn','null','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_guide_angler_submission','null','null','filename','null','1',null,null,null,null);
delete from $javascript_processes where process = 'load_guide_angler_submission';
delete from $process_generic_output where process = 'load_guide_angler_submission';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'load_guide_paper_fishing_trips';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('load_guide_paper_fishing_trips','load_guide_fishing_trips \$process \$login_name filename execute_yn','Template: A=Interview_number, B=Permit_code, C=Census_date, D=Fishing_duration, E=Angler_count, F=Species_targeted, G=Florida_code(starting), H=Kept_count(starting), I=Released_count(starting), J=Fishing_area(starting), K=Fishing_zone(starting)',null,null,null,null,'load',null);
delete from $process_parameter where process = 'load_guide_paper_fishing_trips';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_guide_paper_fishing_trips','null','null','execute_yn','null','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_guide_paper_fishing_trips','null','null','filename','null','1',null,null,null,null);
delete from $javascript_processes where process = 'load_guide_paper_fishing_trips';
delete from $process_generic_output where process = 'load_guide_paper_fishing_trips';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
