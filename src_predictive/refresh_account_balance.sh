#!/bin/bash
# ----------------------------------------------------------
# $APPASERVER_HOME/src_predictive/refresh_account_balance.sh
# ----------------------------------------------------------

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

select="full_name,street_address,account_number"
folder="investment_account"

echo "select $select from $folder order by $select;"		|
sql.e								|
while read record
do
	full_name=`echo $record | piece.e '^' 0`
	street_address=`echo $record | piece.e '^' 1`
	account_number=`echo $record | piece.e '^' 2`

	post_change_account_balance	ignored				\
					fund				\
					"$full_name"			\
					"$street_address"		\
					"$account_number"		\
					""				\
					update				\
					preupdate_full_name		\
					preupdate_street_address	\
					preupdate_account_number	\
					preupdate_date_time
done

exit $?
