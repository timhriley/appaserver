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

full_name_escaped=`echo "$full_name" | escape_character.e "'"`

select="	enrollment_tuition_total,				\
		course_drop_total,					\
		tuition_payment_total,					\
		tuition_refund_total,
		payor_full_name,
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

course_drop_total=`echo $results | piece.e '^' 1`

if [ "$course_drop_total" = "" ]
then
	course_drop_total=0
fi

tuition_payment_total=`echo $results | piece.e '^' 2`

if [ "$tuition_payment_total" = "" ]
then
	tuition_payment_total=0
fi

tuition_refund_total=`echo $results | piece.e '^' 3`

if [ "$tuition_refund_total" = "" ]
then
	tuition_refund_total=0
fi

payor_full_name=`echo $results | piece.e '^' 4`
payor_street_address=`echo $results | piece.e '^' 5`

net_enrollment=`bc.sh $enrollment_tuition_total - $course_drop_total`

net_payment=`bc.sh $tuition_payment_total - $tuition_refund_total`

prepaid_liability=`				\
	customer_prepaid_liability.sh		\
	"$payor_full_name"			\
	"$payor_street_address"`

invoice_amount_due=`bc.sh $net_enrollment - $net_payment`

if [ "$prepaid_liability" != "" ]
then
	invoice_amount_due=`bc.sh $invoice_amount_due - $prepaid_liability`

fi

if [ "$invoice_amount_due" = "" ]
then
	invoice_amount_due=0
fi

echo "	update registration						\
	set invoice_amount_due =					\
		$invoice_amount_due					\
	where	student_full_name = '$full_name_escaped' and		\
		student_street_address = '$street_address' and		\
	  	season_name = '$season_name' and			\
	  	year = $year;"						|
sql

exit 0

