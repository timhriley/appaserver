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

table_name=cost_recovery_method
(
sql.e << all_done
insert into $table_name (cost_recovery_method) values ('alternative_accelerated');
insert into $table_name (cost_recovery_method) values ('alternative_straight_line');
insert into $table_name (cost_recovery_method) values ('statutory_accelerated');
insert into $table_name (cost_recovery_method) values ('statutory_straight_line');
all_done
) 2>&1 | grep -vi duplicate
exit 0
