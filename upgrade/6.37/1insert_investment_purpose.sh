#!/bin/sh
table_name=investment_purpose

# Returns 0 if BANK_UPLOAD.bank_date exists
# -----------------------------------------
folder_attribute_exists.sh $application bank_upload bank_date

if [ $? -ne 0 ]
then
	exit 1
fi

(
sql.e << all_done
insert into $table_name (investment_purpose) values ('available');
insert into $table_name (investment_purpose) values ('retirement');
all_done
) 2>&1 | grep -vi duplicate
exit 0
