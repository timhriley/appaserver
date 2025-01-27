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
update process set command_line = 'create_empty_application \$session \$login_name \$process destination_application new_application_title execute_yn', notepad = 'This process creates an empty application. It creates a new database, the Appaserver tables, the data directories, among other application objects. Following this process, you can begin inserting the table rows.' where process = 'create_empty_application';
delete from process_parameter where process = 'create_empty_application' and prompt = 'delete_application_yn';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
