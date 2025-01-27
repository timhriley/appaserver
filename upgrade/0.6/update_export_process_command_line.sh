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

(
cat << shell_all_done
update process set command_line = 'export_process \$process process exclude_roles_yn', notepad = 'For a list of processes, this process exports from the following: process, role_process, process_parameter, prompt, drop_down_prompt, and drop_down_prompt_data.' where process = 'export_process';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
