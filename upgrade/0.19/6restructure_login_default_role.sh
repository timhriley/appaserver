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
alter table login_default_role add full_name char( 60 ) not null;
alter table login_default_role add street_address char( 60 ) not null;
alter table login_default_role drop index login_default_role;
alter table login_default_role drop index login_default_role_unique;
alter table login_default_role drop login_name;

insert into table_column (table_name,column_name,primary_key_index) values ('login_default_role','full_name',1);
insert into table_column (table_name,column_name,primary_key_index) values ('login_default_role','street_address',2);
delete from table_column where table_name = 'login_default_role' and column_name = 'login_name';

update login_default_role set full_name = (select full_name from self), street_address = (select street_address from self);

create unique index login_default_role_unique on login_default_role (full_name,street_address);

show create table login_default_role;
shell_all_done
) | sql.e

select.sh '*' login_default_role

exit 0
