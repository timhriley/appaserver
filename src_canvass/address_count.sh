:
# $APPASERVER_HOME/src_canvass/address_count.sh
# ----------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ----------------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: `basename.e $0 n` street_name" 1>&2
	exit 1
fi

street_name="$1"

select="count(1)"
from="address"
where="street_name = '$street_name'"

statement="select $select from $from where $where;"

echo "$statement"		|
sql.e

exit 0
