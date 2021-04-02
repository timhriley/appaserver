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
		tuition_refund_total"

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
course_drop_total=`echo $results | piece.e '^' 1`
tuition_payment_total=`echo $results | piece.e '^' 2`
tuition_refund_total=`echo $results | piece.e '^' 3`

net_enrollment=`bc.sh $enrollment_tuition_total - $course_drop_total`

net_payment=`bc.sh $tuition_payment_total - $tuition_refund_total`

invoice_aount_due=`bc.sh $net_enrollment - $net_payment`

echo "	update registration						\
	set invoice_amount_due =					\
		$invoice_amount_due					\
	where	full_name = '$full_name_escaped' and			\
		street_address = '$street_address' and			\
	  	season_name = '$season_name' and			\
	  	year = $year;"						|
sql

exit 0

