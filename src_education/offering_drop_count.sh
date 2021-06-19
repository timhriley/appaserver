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

where="course_name = '$course_name_escaped'	\
   and season_name = '$season_name'		\
   and year = $year"

select="count(1)"

drop_count=`select.sh "$select" course_drop "$where" ''`

if [ "$drop_count" = "" ]
then
	drop_count=null
fi

echo "	update offering				\
	set drop_count = $drop_count		\
	where $where;"				|
sql

exit 0

