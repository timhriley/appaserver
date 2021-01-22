#!/bin/bash

echo $0 $* 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 program_name" 1>&2
	exit 1
fi

program_name="$1"

program_name_escaped=`echo "$program_name" | escape_character.e "'"`

echo "	update program						\
	set program_donation_total =				\
	( select sum( donation_amount )				\
		from program_donation				\
		where	program_donation.program_name =		\
			program.program_name ) and		\
	program_name = '$program_name';"			|
sql

exit 0

