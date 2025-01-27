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
drop table if exists storage_engine;
create table storage_engine (storage_engine char (10) not null) engine MyISAM;
create unique index storage_engine on storage_engine (storage_engine);
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
