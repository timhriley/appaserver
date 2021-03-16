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

if [ "$application" != "waterquality" ]
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
delete from $process where process = 'output_results_gracechart';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,process_group,preprompt_help_text,appaserver_yn) values ('output_results_gracechart','generic_gracechart \$process_set \$process \$login_name \$dictionary',null,null,null,'gracechart',null,null,null);
delete from $process_parameter where process = 'output_results_gracechart';
delete from $javascript_processes where process = 'output_results_gracechart';
delete from $process_generic_output where process = 'output_results_gracechart';
insert into $role_process_set_member (process,process_set,role) values ('output_results_gracechart','output_results_per_project','public');
insert into $role_process_set_member (process,process_set,role) values ('output_results_gracechart','output_results_per_project','scientist');
insert into $role_process_set_member (process,process_set,role) values ('output_results_gracechart','output_results_per_project','supervisor');
insert into $role_process_set_member (process,process_set,role) values ('output_results_gracechart','output_results_per_station','public');
insert into $role_process_set_member (process,process_set,role) values ('output_results_gracechart','output_results_per_station','scientist');
insert into $role_process_set_member (process,process_set,role) values ('output_results_gracechart','output_results_per_station','supervisor');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
