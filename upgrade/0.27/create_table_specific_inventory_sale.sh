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
drop table if exists specific_inventory_sale;
create table specific_inventory_sale (full_name char (60) not null,contact_key char (60) not null,sale_date_time datetime not null,inventory_name char (30) not null,serial_key char (40) not null,retail_price double (10,2),unit_cost double (10,2),discount_amount double (14,2),extended_price double (14,2)) engine MyISAM;
create unique index specific_inventory_sale_unique on specific_inventory_sale (full_name,contact_key,sale_date_time,inventory_name,serial_key);
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
