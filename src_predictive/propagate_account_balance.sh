#!/bin/bash
# ------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/propagate_account_balance.sh
# ------------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in $0: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 ignored fund [full_name street_address account_number]" 1>&2
	exit 1
fi

fund=$2

if [ $# -gt 2 ]
then
	full_name=$3
	street_address=$4
	account_number=$5
else
	full_name=""
	street_address=""
	account_number=""
fi

function post_change_account_balance_execute()
{
	application=$1
	fund=$2
	full_name=$3
	street_address=$4
	account_number=$5

	post_change_account_balance	$application			\
					"$fund"				\
					"$full_name"			\
					"$street_address"		\
					"$account_number"		\
					date_time			\
					update				\
					preupdate_full_name		\
					preupdate_street_address	\
					preupdate_account_number	\
					preupdate_date_time
}

function post_change_each_account_balance()
{
	application=$1
	fund=$2

	table="investment_account"

	select="	full_name,
			street_address,
			account_number"

	echo "select ${select} from $table;"				|
	sql.e								|
	while read record
	do
		full_name=`echo $record | piece.e '^' 0`
		street_address=`echo $record | piece.e '^' 1`
		account_number=`echo $record | piece.e '^' 2`

		post_change_account_balance_execute	\
			"${application}"		\
			"${fund}"			\
			"${full_name}"			\
			"${street_address}"		\
			"${account_number}"
	done
}

if [ "$full_name" = "" -o "$full_name" = "full_name" ]
then
	post_change_each_account_balance $application "$fund"
else
	post_change_account_balance_execute	$application		\
						"$fund"			\
						"$full_name"		\
						"$street_address"	\
						"$account_number"
fi

exit 0
