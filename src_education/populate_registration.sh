#!/bin/bash
# -------------------------------------------------------
# $APPASERVER_HOME/src_education/populate_registration.sh
# -------------------------------------------------------

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
	echo "Usage: $0 where" 1>&2
	exit 1
fi

echo $0 $* 1>&2

where="$1"

select="anchor_date,anchor_time,location,concat( site_number,'|', anchor_date ), anchor_time,project,collection_name,location,site_number"

select="full_name,street_address,season_name,concat( year, ' [', invoice_amount_due, ']' )"

table="registration"

echo "select $select from $table where $where order by $select;"	|
sql.e

exit 0
