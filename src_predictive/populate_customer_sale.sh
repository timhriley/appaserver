#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/populate_customer_sale.sh
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

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 ignored one2m_folder where" 1>&2
	exit 1
fi

echo $0 $* 1>&2

one2m_folder=$2
parameter_where=$3

if [ "$one2m_folder" = "activity_work" ]
then
	where="$parameter_where and completed_date_time is not null"
else
	where="$parameter_where"
fi

table=customer_sale
select="full_name,street_address,sale_date_time"

echo "select $select from $table where $where order by $select;"	|
sql.e

exit 0
