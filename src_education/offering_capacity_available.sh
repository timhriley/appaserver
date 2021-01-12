#!/bin/bash

echo $0 $* 1>&2

if [ "$#" -lt 3 ]
then
	echo "Usage: $0 course_name season_name year [set_yn]" 1>&2
	exit 1
fi

course_name="$1"
season_name="$2"
year=$3

if [ "$#" -eq 4 -a "$4" = 'y' ]
then
	do_set=1
else
	do_set=0
fi

function offering_set()
{
	course_name="$1"
	season_name="$2"
	year=$3
	capacity_available=$4

	table="offering"
	key="course_name,season_name,year"

	data=`echo "$course_name^$season_name^$year^capacity_available^$capacity_available"`

	echo $data 						|
	update_statement table=$table key=$key carrot=y 	|
	sql
}

function fetch_capacity_available()
{
	course_name="$1"
	season_name=$2
	year=$3

	course_name_escaped=`echo "$course_name" | escape_character.e "'"`

	capacity_available=`						\
	echo "	select ifnull( class_capacity, 0 ) -			\
		       ifnull( enrollment_count, 0 )			\
		from offering						\
		where course_name = '$course_name_escaped'		\
	  	and season_name = '$season_name'			\
	  	and year = $year;"					|
	sql`
}

fetch_capacity_available	"$course_name"				\
				"$season_name"				\
				$year

if [ "$do_set" -eq 1 ]
then
	offering_set							\
		"$course_name"						\
		"$season_name"						\
		$year							\
		$capacity_available
else
	echo "$capacity_available"
fi

exit 0

