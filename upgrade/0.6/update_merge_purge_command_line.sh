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
update process set command_line = 'merge_purge \$session \$login_name \$role \$process', notepad = 'This process allows you to remove duplicate rows. It may be that two rows should really be one row because a spelling is slightly different in one of them.' where process = 'merge_purge';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
