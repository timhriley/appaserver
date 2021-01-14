#!/bin/bash

echo $0 $* 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 full_name street_address course_name season_name year" 1>&2
	exit 1
fi

full_name="$1"
street_address="$2"
course_name="$3"
season_name="$4"
year=$5

full_name_escaped=`echo "$full_name" | escape_character.e "'"`
course_name_escaped=`echo "$course_name" | escape_character.e "'"`

echo "	update enrollment						\
	set tuition_payment_total =					\
		( select sum( payment_amount )				\
			from tuition_payment				\
			where	tuition_payment.full_name =		\
				enrollment.full_name and		\
				tuition_payment.street_address =	\
				enrollment.street_address and		\
				tuition_payment.course_name =		\
				enrollment.course_name and		\
				tuition_payment.season_name =		\
				enrollment.season_name and		\
				tuition_payment.year =			\
				enrollment.year )			\
	where	full_name = '$full_name_escaped' and			\
 		street_address = '$street_address' and			\
		course_name = '$course_name_escaped' and		\
 		season_name = '$season_name' and			\
 		year = $year;"						|
sql

exit 0

