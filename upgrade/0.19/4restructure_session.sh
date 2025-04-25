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
alter table session add full_name char( 60 );
alter table session add street_address char( 60 );
alter table session drop login_name;

insert into table_column (table_name,column_name,display_order) values ('session','full_name',1);
insert into table_column (table_name,column_name,display_order) values ('session','street_address',2);
delete from table_column where table_name = 'session' and column_name = 'login_name';

update table_column set display_order = 3 where table_name = 'session' and column_name = 'login_date';
update table_column set display_order = 4 where table_name = 'session' and column_name = 'login_time';
update table_column set display_order = 5 where table_name = 'session' and column_name = 'last_access_date';
update table_column set display_order = 6 where table_name = 'session' and column_name = 'last_access_time';
update table_column set display_order = 7 where table_name = 'session' and column_name = 'http_user_agent';
update table_column set display_order = 8 where table_name = 'session' and column_name = 'remote_ip_address';

show create table session;
shell_all_done
) | sql.e

exit 0
