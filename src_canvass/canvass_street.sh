:
# $APPASERVER_HOME/src_canvass/canvass_street.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 canvass_name include_yn" 1>&2
	exit 1
fi

canvass_name="$1"
include_yn=$2

select="	street.street_name,		\
		longitude,			\
		latitude,			\
		ifnull(house_count,0),		\
		ifnull(apartment_count,0),	\
		ifnull(house_count,0) +		\
		ifnull(apartment_count,0)"

from="canvass_street,street"

join="	canvass_street.street_name = street.street_name		\
    and canvass_street.city = street.city			\
    and canvass_street.state_code = street.state_code"

if [ "$include_yn" = "y" ]
then
	include_where="include_yn = 'y'"
else
	include_where="(include_yn = 'n' or include_yn is null)"
fi

where="	canvass = '$canvass'			\
    and canvass_date is null			\
    and ifnull(bypass_yn,'n') <> 'y'		\
    and $include_where				\
    and $join"

statement="select $select from $from where $where;"

echo "$statement"		|
sql.e				|
grep -v '\^0$'			|
cat

exit 0
