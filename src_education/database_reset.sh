:
echo "delete from enrollment;" | sql
echo "delete from program_donation;" | sql
echo "delete from tuition_payment;" | sql
echo "delete from tuition_refund;" | sql
echo "delete from product_sale;" | sql
echo "delete from ticket_sale;" | sql
echo "delete from ticket_refund;" | sql
echo "delete from transaction;" | sql
echo "delete from journal_ledger;" | sql
echo "delete from paypal;" | sql

#echo "delete from registration;" | sql

#echo "delete from student;" | sql
echo "delete from student where full_name not like 'New %';" | sql

#echo "delete from entity where full_name <> 'TNT';" | sql
echo "delete from entity where full_name <> 'TNT' and full_name not like 'New %';" | sql

echo "	update offering					\
	set	enrollment_count = 0,			\
		capacity_available = class_capacity;"	|
sql

exit 0
