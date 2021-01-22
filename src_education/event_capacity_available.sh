#!/bin/bash

echo $0 $* 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 program_name event_date event_time" 1>&2
	exit 1
fi

program_name="$1"
event_date="$2"
event_time=$3

program_name_escaped=`echo "$program_name" | escape_character.e "'"`

event_where="	program_name = '$program_name_escaped' and	\
 		event_date = '$event_date' and			\
 		event_time = '$event_time'"

select="venue_name,street_address,ticket_sale_count"

results=`select.sh $select event "$event_where"`

if [ "$results" = "" ]
then
     echo "ERROR in $0: select.sh event failed with where = $event_where" 1>&2
     exit 1
fi

venue_name=`echo $results | piece.e '^' 0`
street_address=`echo $results | piece.e '^' 1`
ticket_sale_count=`echo $results | piece.e '^' 2`

if [ "$ticket_sale_count" = "" ]
then
	ticket_sale_count=0
fi

venue_where="	venue_name = '$venue_name' and			\
 		street_address = '$street_address'"

capacity=`select.sh capacity venue "$venue_where"`

if [ "$capacity" = "" ]
then
     echo "ERROR in $0: select.sh venue failed with where = $venue_where" 1>&2
     exit 1
fi

capacity_available=`expr $capacity - $ticket_sale_count`

echo "	update event						\
	set capacity_available = $capacity_available		\
	where $event_where;"					|
sql

exit 0

