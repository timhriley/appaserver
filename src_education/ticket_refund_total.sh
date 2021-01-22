#!/bin/bash

echo $0 $* 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 program_name event_date event_time" 1>&2
	exit 1
fi

program_name="$1"
event_date="$2"
event_time="$3"

program_name_escaped=`echo "$program_name" | escape_character.e "'"`

echo "	update event							\
	set ticket_refund_total =					\
		( select sum( refund_amount )				\
			from ticket_refund				\
			where ticket_refund.program_name =		\
				event.program_name and			\
			      ticket_refund.event_date =		\
				event.event_date and			\
			      ticket_refund.event_time =		\
				event.event_time )			\
	where	program_name = '$program_name_escaped' and		\
		event_date = '$event_date' and				\
		event_time = '$event_time';"				|
sql

exit 0

