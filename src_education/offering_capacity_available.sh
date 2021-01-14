#!/bin/bash

echo $0 $* 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 course_name season_name year" 1>&2
	exit 1
fi

course_name="$1"
season_name="$2"
year=$3

course_name_escaped=`echo "$course_name" | escape_character.e "'"`

echo "	update offering							\
	set capacity_available =					\
		ifnull( class_capacity, 0 ) -				\
		ifnull( enrollment_count, 0 ) 				\
	where	course_name = '$course_name_escaped' and		\
 		season_name = '$season_name' and			\
 		year = $year;"						|
sql

exit 0

