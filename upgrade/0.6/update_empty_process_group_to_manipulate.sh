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
update process set process_group = 'alter' where process_group is null and process in ( select distinct process from role_process );
insert into process_group values ( 'alter' );
update process set process_group = 'alter' where process_group = 'manipulate';
delete from process_group where process_group = 'manipulate';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
