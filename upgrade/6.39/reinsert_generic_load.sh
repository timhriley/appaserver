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
delete from $process where process = 'generic_load';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('generic_load','generic_load \$login \$session \$process \$role',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'generic_load';
delete from $javascript_processes where process = 'generic_load';
delete from $process_generic_output where process = 'generic_load';
insert into $process_groups (process_group) values ('load');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
