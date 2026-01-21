:
# $APPASERVER_HOME/src_canvass/address_count.sh
# ----------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ----------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: `basename.e $0 n` street_name city" 1>&2
	exit 1
fi

street_name="$1"
city="$2"

select="count(1)"
from="address"
where="street_name = '$street_name' and city = '$city'"

statement="select $select from $from where $where;"

echo "$statement"		|
sql.e

exit 0
