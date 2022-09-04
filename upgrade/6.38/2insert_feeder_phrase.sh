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
	exit 0
fi

field="feeder_phrase,nominal_account,full_name,street_address"

from="reoccurring_transaction"

debit_account_where="debit_account != 'bank_of_america_checking' and debit_account != 'chase_checking' and debit_account != 'account_payable' and debit_account != 'bank_of_america_credit_card'"

credit_account_where="credit_account != 'bank_of_america_checking' and credit_account != 'chase_checking' and credit_account != 'account_payable' and credit_account != 'bank_of_america_credit_card'"

debit_select="bank_upload_feeder_phrase,debit_account,full_name,street_address"
debit_where="bank_upload_feeder_phrase is not null and $debit_account_where"

credit_select="bank_upload_feeder_phrase,credit_account,full_name,street_address"
credit_where="bank_upload_feeder_phrase is not null and $credit_account_where"

(
select.sh "$debit_select" $from "$debit_where"
select.sh "$credit_select" $from "$credit_where"
) 								|
insert_statement table=feeder_phrase field=$field delimiter="^"	|
sql.e								|
cat

exit 0
