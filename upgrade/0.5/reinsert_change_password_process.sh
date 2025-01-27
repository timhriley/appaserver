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
cat << all_done
delete from process where process = 'change_password';
insert into process (process,command_line) values ('change_password','change_password \$session \$login_name \$role \$process' );
delete from process_parameter where process = 'change_password';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
