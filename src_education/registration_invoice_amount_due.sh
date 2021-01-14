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
	set invoice_amount_due =					\
		ifnull(tuition,0) -					\
		ifnull(tuition_payment_total,0) -			\
		ifnull(tuition_refund_total,0) 				\
	where	full_name = '$full_name_escaped' and			\
		street_address = '$street_address' and			\
	  	season_name = '$season_name' and			\
	  	year = $year;"						|
sql

echo "	update registration						\
	set invoice_amount_due = 0					\
	where	full_name = '$full_name_escaped' and			\
		street_address = '$street_address' and			\
	  	season_name = '$season_name' and			\
	  	year = $year and					\
		ifnull( tuition_refund_total, 0 ) <> 0;"		|
sql

exit 0

