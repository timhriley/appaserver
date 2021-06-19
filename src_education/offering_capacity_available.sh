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

select="class_capacity,enrollment_count,drop_count"

results=`select.sh $select offering "$where" ''`

class_capacity=`echo $results | piece.e '^' 0`

if [ "$class_capacity" = "" ]
then
	class_capacity=0
fi

enrollment_count=`echo $results | piece.e '^' 1`

if [ "$enrollment_count" = "" ]
then
	enrollment_count=0
fi


drop_count=`echo $results | piece.e '^' 2`

if [ "$drop_count" = "" ]
then
	drop_count=0
fi

capacity_available=`expr $class_capacity - $enrollment_count + $drop_count`

if [ "$capacity_available" = "" ]
then
	capacity_available=null
fi

echo "	update offering							\
	set capacity_available = $capacity_available			\
	where $where;"							|
sql

exit 0

