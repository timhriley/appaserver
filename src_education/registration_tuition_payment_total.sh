#!/bin/bash

echo $0 $* 1>&2

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
	tuition_payment_total=$5

	table="registration"
	key="full_name,street_address,season_name,year"

	data=`echo "$full_name^$street_address^$season_name^$year^tuition_payment_total^$tuition_payment_total"`

	echo $data 						|
	update_statement table=$table key=$key carrot=y 	|
	sql
}

function fetch_tuition_payment_total()
{
	full_name_escaped="$1"
	street_address="$2"
	season_name="$3"
	year=$4

	tuition_payment_total=`						\
		echo "	select sum(payment_amount)			\
			from tuition_payment				\
			where full_name = '$full_name_escaped'		\
	  		and street_address = '$street_address'		\
	  		and season_name = '$season_name'		\
	  		and year = $year;"				|
	sql`
}

fetch_tuition_payment_total	"$full_name_escaped"			\
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
		$tuition_payment_total
else
	echo "$tuition_payment_total"
fi

exit 0

