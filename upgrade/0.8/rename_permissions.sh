#!/bin/sh
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

sql.e << all_done
alter table permissions rename as permission;
update folder set folder = 'permission' where folder = 'permissions';
update folder_attribute set folder = 'permission' where folder = 'permissions';
update relation set folder = 'permission' where folder = 'permissions';
update relation set related_folder = 'permission' where related_folder = 'permissions';
update role_folder set folder = 'permission' where folder = 'permissions';
update process_parameter set folder = 'permission' where folder = 'permissions';
update folder_operation set folder = 'permission' where folder = 'permissions';
alter table permission drop index permissions;
create unique index permission on permission ( permission );
all_done

exit 0
