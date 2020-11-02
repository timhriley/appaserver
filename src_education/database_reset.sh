:
echo "delete from transaction where transaction_date_time >= '2020-01-02';" | sql
echo "delete from journal_ledger where transaction_date_time >= '2020-01-02';" | sql
echo "delete from enrollment;" | sql
echo "delete from registration;" | sql
echo "delete from program_payment;" | sql
echo "delete from tuition_payment;" | sql
echo "delete from student;" | sql
echo "delete from entity where full_name <> 'TNT';" | sql
echo "delete from deposit;" | sql

echo "	update offering					\
	set	enrollment_count = 0,			\
		capacity_available = class_capacity;"	|
sql

exit 0
