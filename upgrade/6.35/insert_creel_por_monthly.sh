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

if [ "$application" != "creel" ]
then
	exit 0
fi

table_name=select_statement
(
sql.e << all_done
insert into $table_name (select_statement_title,login_name,select_statement) values ('POR_catches_per_month','tim','select substr( census_date, 1, 4) year, substr( census_date, 6, 2 ) month, count(*) from catches group by substr( census_date, 1, 4), substr( census_date, 6, 2);');
all_done
) 2>&1 | grep -vi duplicate
exit 0
