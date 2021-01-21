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
	set enrollment_count =						\
		( select count( 1 )					\
			from enrollment					\
			where	enrollment.course_name =		\
				offering.course_name and		\
				enrollment.season_name =		\
				offering.season_name and		\
				enrollment.year =			\
				offering.year )	 			\
	where	course_name = '$course_name_escaped' and		\
 		season_name = '$season_name' and			\
 		year = $year;"						|
sql

exit 0

