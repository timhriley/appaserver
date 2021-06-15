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

# Returns 0 if BANK_UPLOAD.bank_date exists
# -----------------------------------------
folder_attribute_exists.sh $application bank_upload bank_date

if [ $? -ne 0 ]
then
	exit 1
fi

table_name=cost_recovery_period
(
sql.e << all_done
insert into $table_name (cost_recovery_period) values ('03-year');
insert into $table_name (cost_recovery_period) values ('05-year');
insert into $table_name (cost_recovery_period) values ('07-year');
insert into $table_name (cost_recovery_period) values ('10-year');
insert into $table_name (cost_recovery_period) values ('15-year');
insert into $table_name (cost_recovery_period) values ('20-year');
insert into $table_name (cost_recovery_period) values ('25-year');
insert into $table_name (cost_recovery_period) values ('27.5-year');
insert into $table_name (cost_recovery_period) values ('31.5-year');
insert into $table_name (cost_recovery_period) values ('39-year');
insert into $table_name (cost_recovery_period) values ('40-year');
all_done
) 2>&1 | grep -vi duplicate
exit 0
