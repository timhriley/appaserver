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
update process set command_line = 'grant_select_to_user \$process login_name connect_from_host revoke_only_yn execute_yn' where process = 'grant_select_to_user';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
