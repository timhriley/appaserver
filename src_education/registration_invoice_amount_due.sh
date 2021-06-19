#!/bin/bash

echo $0 $* 1>&2

if [ "$#" -ne 4 ]
then
	echo "Usage: $0 full_name street_address season_name year" 1>&2
	exit 1
fi

full_name="$1"
street_address="$2"
season_name="$3"
year=$4

function invoice_amount_due_calculate ()
{
	enrollment_tuition_total=$1
	tuition_payment_total=$2
	tuition_refund_total=$3
	prepaid_liability=$4

	if [ "$prepaid_liability" != "0" ]
	then
		invoice_amount_due=-${prepaid_liability}
	else
		net_payment=`bc.sh				\
				$tuition_payment_total - 	\
				$tuition_refund_total`

		invoice_amount_due=`bc.sh			\
				$enrollment_tuition_total -	\
				$net_payment`
	fi
}

full_name_escaped=`echo "$full_name" | escape_character.e "'"`

select="	enrollment_tuition_total,				\
		tuition_payment_total,					\
		tuition_refund_total,					\
		payor_full_name,					\
		payor_street_address"

registration_where="							\
		student_full_name = '$full_name_escaped' and		\
	  	student_street_address = '$street_address' and		\
		season_name = '$season_name' and			\
		year = $year"

results=`select.sh "$select" registration "$registration_where"`

if [ "$results" = "" ]
then
	echo "ERROR in $0: select.sh returned empty." 1>&2
	exit 1
fi

enrollment_tuition_total=`echo $results | piece.e '^' 0`

if [ "$enrollment_tuition_total" = "" ]
then
	enrollment_tuition_total=0
fi

tuition_payment_total=`echo $results | piece.e '^' 1`

if [ "$tuition_payment_total" = "" ]
then
	tuition_payment_total=0
fi

tuition_refund_total=`echo $results | piece.e '^' 2`

if [ "$tuition_refund_total" = "" ]
then
	tuition_refund_total=0
fi

payor_full_name=`echo $results | piece.e '^' 3`

if [ "$payor_full_name" != "" ]
then
	payor_street_address=`echo $results | piece.e '^' 4`

	prepaid_liability=`				\
		entity_prepaid_liability		\
		"$payor_full_name"			\
		"$payor_street_address"`

	if [ "$prepaid_liability" = "" ]
	then
		prepaid_liability=0
	fi
else
	prepaid_liability=0
fi

invoice_amount_due_calculate				\
	$enrollment_tuition_total			\
	$tuition_payment_total				\
	$tuition_refund_total				\
	$prepaid_liability

echo "	update registration						\
	set invoice_amount_due =					\
		$invoice_amount_due					\
	where	student_full_name = '$full_name_escaped' and		\
		student_street_address = '$street_address' and		\
	  	season_name = '$season_name' and			\
	  	year = $year;"						|
sql

exit 0

