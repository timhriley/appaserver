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
drop table if exists vendor;
create table vendor (full_name char (60) not null,street_address char (60) not null,website_address char (50),website_login char (50),website_password char (20)) engine MyISAM;
create unique index vendor_unique on vendor (full_name,street_address);
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
