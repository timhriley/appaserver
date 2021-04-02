:
echo "select permit_code from permits where permit_code not in ( select permit_code from fishing_trips where permit_code is not null);"	|
sql							|
delete_statement table=permits field=permit_code	|
cat
