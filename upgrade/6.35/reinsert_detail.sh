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
delete from $process where process = 'detail';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('detail','detail \$session \$login_name \$folder \$role \$target_frame \$primary_data_list \$dictionary',null,null,null,null,'y',null,null);
delete from $operation where operation = 'detail';
insert into $operation (operation,output_yn) values ('detail','y');
delete from $process_parameter where process = 'detail';
delete from $javascript_processes where process = 'detail';
delete from $process_generic_output where process = 'detail';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
