:

field="bank_date,bank_description"

bank_upload_orphan.sh						|
delete_statement table=bank_upload field=$field delimiter='^'	|
cat

exit 0

