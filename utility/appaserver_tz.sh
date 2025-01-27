:
# $APPASERVER_HOME/utility/appaserver_tz.sh
# -----------------------------------------

if [ "$DATABASE" = "" -a "$APPASERVER_DATABASE" = "" ]
then
	export DATABASE=template
fi

utc_offset=`echo "	select application_constant_value		\
			from application_constant			\
			where application_constant = 'utc_offset';"	|
			sql.e | sed 's/^-//'`

if [ "$utc_offset" = "" ]
then
	utc_offset=5
fi

if [ "$utc_offset" -eq 8 ]
then
	echo "America/Los_Angeles"
elif [ "$utc_offset" -eq 7 ]
then
	echo "America/Denver"
elif [ "$utc_offset" -eq 6 ]
then
	echo "America/Chicago"
elif [ "$utc_offset" -eq 5 ]
then
	echo "America/New_York"
fi

exit 0
