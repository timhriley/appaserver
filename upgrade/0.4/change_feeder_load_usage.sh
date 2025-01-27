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

# Returns 0 if FEEDER_PHRASE.feeder_phrase exists
# -----------------------------------------------
folder_attribute_exists.sh $application feeder_phrase feeder_phrase

if [ $? -ne 0 ]
then
	exit 0
fi

(
cat << all_done
update process set command_line = 'feeder_load \$process \$login_name bank_of_america_checking filename 1 2 3 0 4 0 reverse_order_yn account_end_balance execute_yn' where process = 'BofA_cash_load';
update process set command_line = 'feeder_load \$process \$login_name bank_of_america_credit_card filename 1 3 5 0 0 2 y account_end_balance execute_yn' where process = 'BofA_credit_load';
update process set command_line = 'feeder_load \$process \$login_name chase_checking filename 2 3 4 0 6 0 y account_end_balance execute_yn' where process = 'chase_cash_load';
all_done
) | sql.e

exit 0
