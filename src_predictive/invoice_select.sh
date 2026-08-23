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

if [ "$#" -ne 4 ]
then
	echo "Usage: $0 fund_name full_name contact_key sale_date_time" 1>&2
	exit 1
fi

fund_name=$1
full_name=$2
contact_key=$3
sale_date_time=$4

inventory_sale_select="inventory_name,'',quantity,retail_price,discount_amount,extended_price"

fixed_service_sale_select="service_name,'','1',fixed_price,discount_amount,net_revenue"

hourly_service_work_select="	hourly_service.service_name,
				concat(
					service_description,
					' (',
					activity,
					') ',
					'from: ',
					substr( begin_work_date_time, 1, 16 ),
					' to: ',
					substr( end_work_date_time, 1, 16 ) ),
				work_hours,
				hourly_service.hourly_rate,
				discount_hours * hourly_service.hourly_rate,
				work_hours * hourly_service.hourly_rate"

if [ "$fund_name" = "" ]
then
	fund_where="1 = 1"
else
	fund_where="fund_name = '$fund_name'"
fi

if [ "$contact_key" = "" ]
then
	contact_key_where="1 = 1"
else
	contact_key_where="contact_key = '$contact_key'"
fi

#echo "select ${inventory_sale_select}
#from inventory_sale
#where full_name = '$full_name'
#and $fund_where
#and $contact_key_where
#and sale_date_time = '$sale_date_time';" |
#sql.e '^'

echo "select ${fixed_service_sale_select}
from fixed_service_sale
where full_name = '$full_name'
and $fund_where
and $contact_key_where
and sale_date_time = '$sale_date_time';" |
sql.e '^'

hourly_service_join="				\
	hourly_service_work.service_name =	\
	hourly_service.service_name"

echo "select ${hourly_service_work_select}
from hourly_service_work,hourly_service
where full_name = '$full_name'
and $fund_where
and $contact_key_where
and sale_date_time = '$sale_date_time'
and end_work_date_time is not null
and $hourly_service_join;" |
sql.e '^'

exit 0
