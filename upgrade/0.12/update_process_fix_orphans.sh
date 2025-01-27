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
update process set command_line = 'fix_orphans \$process orphans_table delete_yn execute_yn' where process = 'fix_orphans';
update process set notepad = 'This process traverses the many-to-one relationships for a table. It inserts the missing primary keys that contain foreign keys in the selected table.' where process = 'fix_orphans';
update process set process_group = 'alter' where process = 'fix_orphans';
update process_parameter set prompt = 'orphans_table' where process = 'fix_orphans' and prompt = 'orphans_folder';
update prompt set prompt = 'orphans_table' where prompt = 'orphans_folder';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
