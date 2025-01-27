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
update process set process = 'view_appaserver_log_file' where process = 'view_appaserver_error_file';
update role_process set process = 'view_appaserver_log_file' where process = 'view_appaserver_error_file';
update process_parameter set process = 'view_appaserver_log_file' where process = 'view_appaserver_error_file';
update process set command_line = 'view_appaserver_error_file.sh \$process line_count' where process = 'view_appaserver_log_file';
update process set notepad = 'Most error messages are logged to this file. If you get the Server Error screen, then this file is the first place to find a clue. <big><bold>Warning:</bold></big> this process exposes password changes and session numbers.' where process = 'view_appaserver_log_file';
update process set process_group = 'view' where process = 'view_appaserver_log_file';
delete from process where process = 'view_appaserver_error_file';
delete from role_process where process = 'view_appaserver_error_file';
delete from process_parameter where process = 'view_appaserver_error_file';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
