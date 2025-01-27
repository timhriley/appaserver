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

echo "update process set process = 'drilldown', command_line = 'drilldown \$session \$login_name \$role \$target_frame \$folder \$primary_data_list \$update_results \$update_error \$dictionary' where process = 'detail';" | sql

echo "update operation set operation = 'drilldown' where operation = 'detail';" | sql

echo "update role_operation set operation = 'drilldown' where operation = 'detail';" | sql

exit 0
