:
# $APPASERVER_HOME/src_canvass/lookup.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

first_time=1

while read input
do
	street_name=`echo $input | piece.e '^' 0`

	if [ "$street_name" = "Street" -o "$street_name" = "Sum" ]
	then
		continue
	fi

	if [ "$first_time" -eq 1 ]
	then
		first_time=0
	else
		/bin/echo -n ','
	fi

	/bin/echo -n $street_name
done

/bin/echo

exit 0
