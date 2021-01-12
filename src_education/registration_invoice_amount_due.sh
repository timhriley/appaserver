#!/bin/bash

if [ "$#" -lt 4 ]
then
	echo "Usage: $0 full_name street_address season_name year [set_yn]" 1>&2
	exit 1
fi

full_name="$1"
street_address="$2"
season_name="$3"
year=$4

if [ "$#" -eq 5 -a "$5" = 'y' ]
then
	do_set=1
else
	do_set=0
fi

full_name_escaped=`echo "$full_name" | escape_character.e "'"`

function registration_set()
{
	full_name="$1"
	street_address="$2"
	season_name="$3"
	year=$4
	invoice_amount_due=$5

	table="registration"
	key="full_name,street_address,season_name,year"

	data=`echo "$full_name^$street_address^$season_name^$year^invoice_amount_due^$invoice_amount_due"`

	echo $data 						|
	update_statement table=$table key=$key carrot=y 	|
	sql
}

function fetch_invoice_amount_due()
{
	full_name_escaped="$1"
	street_address="$2"
	season_name="$3"
	year=$4

	invoice_amount_due=`						\
		echo "	select ifnull(tuition,0) -			\
			       ifnull(tuition_payment_total,0) -	\
			       ifnull(tuition_refund_total,0) 		\
			from registration				\
			where full_name = '$full_name_escaped'		\
	  		and street_address = '$street_address'		\
	  		and season_name = '$season_name'		\
	  		and year = $year;"				|
	sql`

	# ----------------------------------
	# If negative then overpayment.
	# Overpayment goes to gain_donation.
	# ----------------------------------
	if [ "`echo $invoice_amount_due | piece.e '-' 1 2>/dev/null`" != "" ]
	then
		invoice_amount_due=0
	fi
}

fetch_invoice_amount_due	"$full_name_escaped"			\
				"$street_address"			\
				"$season_name"				\
				$year

if [ "$do_set" -eq 1 ]
then
	registration_set						\
		"$full_name"						\
		"$street_address"					\
		"$season_name"						\
		$year							\
		$invoice_amount_due
else
	echo "$invoice_amount_due"
fi

exit 0

