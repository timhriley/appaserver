#!/bin/sh
# -----------------------------------------------------------
# $APPASERVER_HOME/src_predictive/select_invoice_lineitems.sh
# -----------------------------------------------------------
#
# Freely available software: see Appaserver.org
# -----------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 application full_name street_address sale_date_time completed_date_time" 1>&2
	exit 1
fi

application=$1
full_name=$2
street_address=$3
sale_date_time=$4
completed_date_time=$5

export DATABASE=$application

inventory_sale=`get_table_name $application inventory_sale`
fixed_service_sale=`get_table_name $application fixed_service_sale`
hourly_service_sale=`get_table_name $application hourly_service_sale`
hourly_service_work=`get_table_name $application hourly_service_work`

inventory_sale_select="inventory_name,quantity,retail_price,discount_amount,extended_price"

fixed_service_sale_select="service_name,'1',retail_price,discount_amount,extended_price"

hourly_service_sale_select="	concat( service_name, '--', description ),
				estimated_hours,
				hourly_rate,
				'0',
				extended_price"

hourly_service_work_select="	concat( ${hourly_service_work}.description,
				' from: ',
				substr( begin_work_date_time, 1, 16 ),
				' to: ',
				substr( end_work_date_time, 1, 16 ) ),
				${hourly_service_work}.work_hours,
				hourly_rate,
				'0',
				extended_price"

hourly_service_work_join="	hourly_service_work.full_name =
				hourly_service_sale.full_name and
				hourly_service_work.street_address =
				hourly_service_sale.street_address and
				hourly_service_work.sale_date_time =
				hourly_service_sale.sale_date_time and
				hourly_service_work.service_name =
				hourly_service_sale.service_name and
				hourly_service_work.description =
				hourly_service_sale.description"

echo "select ${inventory_sale_select}
from ${inventory_sale}
where full_name = '$full_name'
and street_address = '$street_address'
and sale_date_time = '$sale_date_time';" |
sql.e '^'

echo "select ${fixed_service_sale_select}
from ${fixed_service_sale}
where full_name = '$full_name'
and street_address = '$street_address'
and sale_date_time = '$sale_date_time';" |
sql.e '^'

if [ "$completed_date_time" = "" ]
then
	echo "select ${hourly_service_sale_select}
	from ${hourly_service_sale}
	where ${hourly_service_sale}.full_name = '$full_name'
	and ${hourly_service_sale}.street_address = '$street_address'
	and ${hourly_service_sale}.sale_date_time = '$sale_date_time';" |
	sql.e '^'
else
	echo "select ${hourly_service_work_select}
	from ${hourly_service_sale},${hourly_service_work}
	where ${hourly_service_work}.full_name = '$full_name'
	and ${hourly_service_work}.street_address = '$street_address'
	and ${hourly_service_work}.sale_date_time = '$sale_date_time'
	and $hourly_service_work_join;" |
	sql.e '^'
fi

exit 0
