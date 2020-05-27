:
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
	echo "\$ . set_database" 1>&2
	exit 1
fi

echo "$0 $*" 1>&2

table="reoccurring_transaction"

select="full_name,street_address,transaction_description"

where="1 = 1"

order="$select"

echo "select $select from $table where $where order by $order;" | sql.e

exit 0
