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

echo "	update registration						\
	set tuition_payment_total =					\
		( select sum( payment_amount )				\
			from tuition_payment				\
			where	tuition_payment.full_name =		\
				registration.full_name and		\
				tuition_payment.street_address =	\
				registration.street_address and		\
				tuition_payment.season_name =		\
				registration.season_name and		\
				tuition_payment.year =			\
				registration.year )			\
	where	full_name = '$full_name_escaped' and			\
	  	street_address = '$street_address' and			\
	  	season_name = '$season_name' and			\
	  	year = $year;"						|
sql

exit 0
