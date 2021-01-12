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
	tuition_refund_total=$5

	table="registration"
	key="full_name,street_address,season_name,year"

	data=`echo "$full_name^$street_address^$season_name^$year^tuition_refund_total^$tuition_refund_total"`

	echo $data 						|
	update_statement table=$table key=$key carrot=y 	|
	sql
}

function fetch_tuition_refund_total()
{
	full_name_escaped="$1"
	street_address="$2"
	season_name="$3"
	year=$4

	tuition_refund_total=`						\
		echo "	select sum(refund_amount)			\
			from tuition_refund				\
			where full_name = '$full_name_escaped'		\
	  		and street_address = '$street_address'		\
	  		and season_name = '$season_name'		\
	  		and year = $year;"				|
	sql`
}

fetch_tuition_refund_total	"$full_name_escaped"			\
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
		$tuition_refund_total
else
	echo "$tuition_refund_total"
fi

exit 0

