#!/bin/sh

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

# Returns 0 if INVESTMENT_ACCOUNT.full_name exists
# ------------------------------------------------
folder_attribute_exists.sh $application investment_account full_name

if [ $? -ne 0 ]
then
	exit 0
fi

select.sh 'full_name,street_address,account_number' investment_account |
while read input
do
	full_name=`echo $input | piece.e '^' 0`
	street_address=`echo $input | piece.e '^' 1`
	account_number=`echo $input | piece.e '^' 2`

	$APPASERVER_HOME/src_predictive/post_change_account_balance	\
		"$full_name"						\
		"$street_address"					\
		"$account_number"					\
		''							\
		update
done

exit 0
