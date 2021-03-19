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

course_drop_join="							\
		course_drop.season_name = offering.season_name and	\
		course_drop.year = offering.year"

offering_join="	enrollment.season_name = offering.season_name and	\
		enrollment.year = offering.year"

student_where="	enrollment.student_full_name = '$full_name_escaped' and	\
	  	enrollment.student_street_address = '$street_address'"

semester_where="enrollment.season_name = '$season_name' and		\
	  	enrollment.year = $year"

where="		$student_where and					\
		$semester_where and					\
		$offering_join and					\
		refund_due_yn = 'y'"

select="sum( offering.course_price )"

from="course_drop,enrollment,offering"

total=`select.sh "$select" "$from" "$where"`

if [ "$total" = "" ]
then
	total=0.0
fi

registration_where="							\
		student_full_name = '$full_name_escaped' and		\
	  	student_street_address = '$street_address' and		\
		season_name = '$season_name' and			\
		year = $year"

echo "	update registration						\
	set course_drop_total = $total					\
	where $registration_where;"					|
sql

exit 0
