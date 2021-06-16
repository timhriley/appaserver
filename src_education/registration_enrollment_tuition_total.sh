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

offering_join="								\
	enrollment.course_name = offering.course_name and		\
	enrollment.season_name = offering.season_name and		\
	enrollment.year = offering.year and				\
	offering.course_name = course.course_name"

echo "	update registration						    \
	set enrollment_tuition_total =					    \
	( select sum( course_price )					    \
		from enrollment, offering, course			    \
		where	enrollment.student_full_name =			    \
			registration.student_full_name and		    \
			enrollment.student_street_address =		    \
			registration.student_street_address and		    \
			enrollment.season_name =			    \
			registration.season_name and			    \
			enrollment.year =				    \
			registration.year and				    \
		$offering_join )					    \
	where	registration.student_full_name = '$full_name_escaped' and   \
		registration.student_street_address = '$street_address' and \
	 	registration.season_name = '$season_name' and		    \
	 	registration.year = $year;"				    |
sql

exit 0

