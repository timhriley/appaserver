:
# $APPASERVER_HOME/src_canvass/apartment_address.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

echo "ApartmentAddress^UnitCount^UnitMinimum^UnitMaximum"

while read apartment_complex
do
	street_name=`echo $apartment_complex | piece.e '^' 0`
	apartment_address=`echo $apartment_complex | piece.e '^' 1`

	where="	street_name = '$street_name' 			\
	    and street_address like '%$apartment_address%'"

	select="	count(1),				\
			min( address_unit ),			\
			max( address_unit )"
	
	/bin/echo -n "$apartment_address^"
	echo "select $select from address where $where;" | sql
done

exit 0
