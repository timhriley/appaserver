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
alter table appaserver_sessions rename as session;
alter table session change column appaserver_session session char(10) not null;
update folder set folder = 'session' where folder = 'appaserver_sessions';
update folder_attribute set folder = 'session' where folder = 'appaserver_sessions';
update relation set folder = 'session' where folder = 'appaserver_sessions';
update relation set related_folder = 'session' where related_folder = 'appaserver_sessions';
update role_folder set folder = 'session' where folder = 'appaserver_sessions';
update process_parameter set folder = 'session' where folder = 'appaserver_sessions';
update folder_operation set folder = 'session' where folder = 'appaserver_sessions';
update attribute set attribute = 'session' where attribute = 'appaserver_session';
update folder_attribute set attribute = 'session' where attribute = 'appaserver_session';
update attribute_exclude set attribute = 'session' where attribute = 'appaserver_session';
update process_parameter set attribute = 'session' where attribute = 'appaserver_session';
update process_set_parameter set attribute = 'session' where attribute = 'appaserver_session';
alter table session drop index appaserver_sessions;
create unique index session on session ( session );
all_done

exit 0
