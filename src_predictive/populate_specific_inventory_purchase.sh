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

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 one_folder" 1>&2
	exit 1
fi

one_folder=$1

table="specific_inventory_purchase"

select="inventory_name, concat( serial_key, ' [', retail_price, ']' )"

if [ "$one_folder" = "specific_inventory_sale" ]
then
	where="not exists ( select inventory_name, serial_key from specific_inventory_sale where specific_inventory_purchase.inventory_name = specific_inventory_sale.inventory_name and specific_inventory_purchase.serial_key = specific_inventory_sale.serial_key )"
else
	where="1 = 1"
fi


order="inventory_name, serial_key"

echo "select $select from $table where $where order by $order;" | sql.e

exit 0
