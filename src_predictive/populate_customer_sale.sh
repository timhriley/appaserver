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

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 one2m_folder where" 1>&2
	exit 1
fi

echo $0 $* 1>&2

one2m_folder=$1
parameter_where=$2

#if [ "$one2m_folder" = "activity_work" ]
#then
	#where="$parameter_where and completed_date_time is not null"
#else
	#where="$parameter_where"
#fi

where="$parameter_where"
table=sale
select="full_name,street_address,sale_date_time"

select.sh $select $table "$where" select

exit 0
