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

if [ "$application" != "modeland" ]
then
	exit 0
fi

process_set="process_set"
process_set_parameter="process_set_parameter"
javascript_process_sets="javascript_process_sets"
javascript_files="javascript_files"
prompt="prompt"
process="process"
drop_down_prompt="drop_down_prompt"
drop_down_prompt_data="drop_down_prompt_data"
role_process_set_member="role_process_set_member"
(
cat << all_done
delete from $process_set where process_set = 'output_model_results';
insert into $process_set (process_set,notepad,html_help_file_anchor,post_change_javascript,prompt_display_text,prompt_display_bottom_yn,process_group,preprompt_help_text) values ('output_model_results',null,'process_set_model_results_help.html','post_change_output_model_results(this)','output_medium','y','output',null);
insert into $role_process_set_member (process,process_set,role) values ('output_model_moving_average_chart','output_model_results','modeler');
insert into $role_process_set_member (process,process_set,role) values ('output_model_moving_average_table','output_model_results','modeler');
insert into $role_process_set_member (process,process_set,role) values ('output_model_results_easychart','output_model_results','modeler');
insert into $role_process_set_member (process,process_set,role) values ('output_model_results_exceedance','output_model_results','modeler');
insert into $role_process_set_member (process,process_set,role) values ('output_model_results_exceedance_chart','output_model_results','modeler');
insert into $role_process_set_member (process,process_set,role) values ('output_model_results_exceedance_table','output_model_results','modeler');
insert into $role_process_set_member (process,process_set,role) values ('output_model_results_googlechart','output_model_results','modeler');
insert into $role_process_set_member (process,process_set,role) values ('output_model_results_gracechart','output_model_results','modeler');
insert into $role_process_set_member (process,process_set,role) values ('output_model_results_high_average_low','output_model_results','modeler');
insert into $role_process_set_member (process,process_set,role) values ('output_model_results_histogram','output_model_results','modeler');
insert into $role_process_set_member (process,process_set,role) values ('output_model_results_spreadsheet','output_model_results','modeler');
insert into $role_process_set_member (process,process_set,role) values ('output_model_results_statistics','output_model_results','modeler');
insert into $role_process_set_member (process,process_set,role) values ('output_model_results_table','output_model_results','modeler');
delete from $process_set_parameter where process_set = 'output_model_results';
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_model_results','alternative','null','null','null','1','y','y',null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_model_results','alternative_station_datatype','null','null','null','1','n','n',null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_model_results','datatype','null','null','null','2','y','y',null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_model_results','level','null','null','null','3','y','y',null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_model_results','null','accumulate_yn','null','null','14',null,null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_model_results','null','begin_date','null','null','6','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_model_results','null','days_to_average','null','null','20','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_model_results','null','email_address','null','null','16','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_model_results','null','end_date','null','null','8','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_model_results','null','null','null','aggregate_statistic','12','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_model_results','null','null','null','model_results_aggregate_level','10','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_model_results','units_converted','null','null','null','18','n',null,'units_converted_list',null);
delete from $javascript_process_sets where process_set = 'output_model_results';
insert into $javascript_process_sets (javascript_filename,process_set) values ('output_model_results_process_set.js','output_model_results');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('accumulate_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date',null,null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('days_to_average','Defaults to 30',null,null,'3');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('email_address','Optionally email the results',null,null,'50');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date',null,null,'y','10');
delete from $process where process = 'output_model_moving_average_chart';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('output_model_moving_average_chart','generic_daily_moving_average \$process_set \$process \$login_name days_to_average exceedance_format_yn gracechart \$dictionary',null,null,null,'moving_average_chart','y',null,null);
delete from $process where process = 'output_model_moving_average_table';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('output_model_moving_average_table','generic_daily_moving_average \$process_set \$process \$login_name days_to_average exceedance_format_yn table \$dictionary',null,null,null,'moving_average_table','y',null,null);
delete from $process where process = 'output_model_results_easychart';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('output_model_results_easychart','generic_easychart \$application \$process_set \$dictionary',null,null,null,'easychart',null,null,null);
delete from $process where process = 'output_model_results_exceedance';
delete from $process where process = 'output_model_results_exceedance_chart';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('output_model_results_exceedance_chart','generic_output_exceedance \$process_set \$process \$login_name chart \$dictionary',null,null,null,'exceedance_chart',null,null,null);
delete from $process where process = 'output_model_results_exceedance_table';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('output_model_results_exceedance_table','generic_output_exceedance \$process_set \$process \$login_name table \$dictionary',null,null,null,'exceedance_table',null,null,null);
delete from $process where process = 'output_model_results_googlechart';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('output_model_results_googlechart','generic_googlechart \$process_set \$login_name \$dictionary',null,null,null,'googlechart',null,'output',null);
delete from $process where process = 'output_model_results_gracechart';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('output_model_results_gracechart','generic_gracechart \$process_set \$process \$login_name \$dictionary',null,null,null,'gracechart',null,null,null);
delete from $process where process = 'output_model_results_high_average_low';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('output_model_results_high_average_low','generic_high_average_low \$process_set \$process \$login_name \$dictionary',null,null,null,'high_average_low',null,null,null);
delete from $process where process = 'output_model_results_histogram';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('output_model_results_histogram','generic_histogram \$process_set \$process \$login_name \$dictionary',null,null,null,'histogram',null,null,null);
delete from $process where process = 'output_model_results_spreadsheet';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('output_model_results_spreadsheet','generic_output_text_file \$process_set \$process \$login_name spreadsheet \$dictionary',null,null,null,'spreadsheet',null,null,null);
delete from $process where process = 'output_model_results_statistics';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('output_model_results_statistics','generic_statistics_report \$process_set \$process \$login_name \$dictionary',null,null,null,'statistics_report',null,null,null);
delete from $process where process = 'output_model_results_table';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('output_model_results_table','generic_output_text_file \$process_set \$process \$login_name table \$dictionary',null,null,null,'table',null,null,null);
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('aggregate_statistic',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','average','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','maximum','5');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','median','4');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','minimum','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','non_zero_percent','8');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','range','6');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','standard_deviation','7');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','sum','2');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('model_results_aggregate_level',null,'aggregate_level');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('model_results_aggregate_level','annually','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('model_results_aggregate_level','daily','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('model_results_aggregate_level','hourly','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('model_results_aggregate_level','monthly','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('model_results_aggregate_level','weekly','1');
insert into $javascript_files (javascript_filename,relative_source_directory_yn) values ('output_model_results_process_set.js',null);
delete from $process where process = 'output_model_moving_average';
delete from $role_process_set_member where process = 'output_model_moving_average';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
