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
	enrollment_count=$4

	table="offering"
	key="course_name,season_name,year"

	data=`echo "$course_name^$season_name^$year^enrollment_count^$enrollment_count"`

	echo $data 						|
	update_statement table=$table key=$key carrot=y 	|
	sql
}

function fetch_enrollment_count()
{
	course_name="$1"
	season_name="$2"
	year=$3

	course_name_escaped=`echo "$course_name" | escape_character.e "'"`

	enrollment_count=`						\
	echo "	select count(1)						\
		from enrollment						\
		where course_name = '$course_name_escaped'		\
	  	and season_name = '$season_name'			\
	  	and year = $year;"					|
	sql`
}

fetch_enrollment_count		"$course_name"				\
				"$season_name"				\
				$year

if [ "$do_set" -eq 1 ]
then
	offering_set							\
		"$course_name"						\
		"$season_name"						\
		$year							\
		$enrollment_count
else
	echo "$enrollment_count"
fi

exit 0

