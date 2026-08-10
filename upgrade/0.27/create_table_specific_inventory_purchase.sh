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

if [ "$application" != "appahost" ]
then
	exit 0
fi

(
cat << shell_all_done
drop table if exists specific_inventory_purchase;
create table specific_inventory_purchase (inventory_name char (30) not null,serial_key char (40) not null,full_name char (60) not null,contact_key char (60) not null,purchase_date_time datetime,unit_cost double (10,2),retail_price double (10,2)) engine MyISAM;
create unique index specific_inventory_purchase_unique on specific_inventory_purchase (inventory_name,serial_key,full_name,contact_key);
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
