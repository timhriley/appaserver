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
alter table role_appaserver_user add full_name char( 60 ) not null;
alter table role_appaserver_user add street_address char( 60 ) not null;
alter table role_appaserver_user drop index role_appaserver_user;
alter table role_appaserver_user drop index role_appaserver_user_unique;
alter table role_appaserver_user drop login_name;

insert into table_column (table_name,column_name,primary_key_index) values ('role_appaserver_user','full_name',1);
insert into table_column (table_name,column_name,primary_key_index) values ('role_appaserver_user','street_address',2);
update table_column set primary_key_index = 3 where table_name = 'role_appaserver_user' and column_name = 'role';
delete from table_column where table_name = 'role_appaserver_user' and column_name = 'login_name';

update role_appaserver_user set full_name = (select full_name from self), street_address = (select street_address from self);

create unique index role_appaserver_user_unique on role_appaserver_user (full_name,street_address,role);

show create table role_appaserver_user;
shell_all_done
) | sql.e

select.sh '*' role_appaserver_user

exit 0
