#!/bin/bash

echo $0 $* 1>&2

if [ "$#" -lt 5 ]
then
	echo "Usage: $0 full_name street_address course_name season_name year [set_yn]" 1>&2
	exit 1
fi

full_name="$1"
street_address="$2"
course_name="$3"
season_name="$4"
year=$5

if [ "$#" -eq 6 -a "$6" = 'y' ]
then
	do_set=1
else
	do_set=0
fi

full_name_escaped=`echo "$full_name" | escape_character.e "'"`
course_name_escaped=`echo "$course_name" | escape_character.e "'"`

function enrollment_set()
{
	full_name="$1"
	street_address="$2"
	course_name="$3"
	season_name="$4"
	year=$5
	tuition_refund_total=$6

	table="enrollment"
	key="full_name,street_address,course_name,season_name,year"

	data=`echo "$full_name^$street_address^$course_name^$season_name^$year^tuition_refund_total^$tuition_refund_total"`

	echo $data 						|
	update_statement table=$table key=$key carrot=y 	|
	sql
}

function fetch_tuition_refund_total()
{
	full_name_escaped="$1"
	street_address="$2"
	course_name_escaped="$3"
	season_name="$4"
	year=$5

	tuition_refund_total=`						\
		echo "	select sum(refund_amount)			\
			from tuition_refund				\
			where full_name = '$full_name_escaped'		\
	  		and street_address = '$street_address'		\
	  		and course_name = '$course_name_escaped'	\
	  		and season_name = '$season_name'		\
	  		and year = $year;"				|
	sql`
}

fetch_tuition_refund_total	"$full_name_escaped"			\
				"$street_address"			\
				"$course_name_escaped"			\
				"$season_name"				\
				$year

if [ "$do_set" -eq 1 ]
then
	enrollment_set							\
		"$full_name"						\
		"$street_address"					\
		"$course_name"						\
		"$season_name"						\
		$year							\
		$tuition_refund_total
else
	echo "$tuition_refund_total"
fi

exit 0

