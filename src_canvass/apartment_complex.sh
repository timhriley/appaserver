:
# $APPASERVER_HOME/src_canvass/apartment_complex.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

(
while read street_name
do
	select="street_name,street_address"
	where="street_name = '$street_name' and address_unit is not null"
	echo "select $select from address where $where;"		  |
	sql								  |
	sed 's/ APT [1-9][0-9]*$//g'
done
)									  |
sort -u									  |
apartment_address.sh

exit 0
