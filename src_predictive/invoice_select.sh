:
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/invoice_select.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

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

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 full_name street_address sale_date_time" 1>&2
	exit 1
fi

full_name=$1
street_address=$2
sale_date_time=$3

inventory_sale_select="inventory_name,'',quantity,retail_price,discount_amount,extended_price"

fixed_service_sale_select="service_name,'','1',retail_price,discount_amount,extended_price"

hourly_service_work_select="	service_name,
				concat(
					work_description,
					' (',
					activity,
					') ',
					'from: ',
					substr( begin_work_date_time, 1, 16 ),
					' to: ',
					substr( end_work_date_time, 1, 16 ) ),
				work_hours,
				hourly_rate,
				'0'"

#echo "select ${inventory_sale_select}
#from inventory_sale
#where full_name = '$full_name'
#and street_address = '$street_address'
#and sale_date_time = '$sale_date_time';" |
#sql.e '^'

#echo "select ${fixed_service_sale_select}
#from fixed_service_sale
#where full_name = '$full_name'
#and street_address = '$street_address'
#and sale_date_time = '$sale_date_time';" |
#sql.e '^'

echo "select ${hourly_service_work_select}
from hourly_service_work
where full_name = '$full_name'
and street_address = '$street_address'
and sale_date_time = '$sale_date_time'
and end_work_date_time is not null;" |
sql.e '^'

exit 0
