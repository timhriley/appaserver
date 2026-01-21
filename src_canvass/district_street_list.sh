:
# $APPASERVER_HOME/src_canvass/district_street_list.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: `basename $0 n` county_district" 1>&2
	exit 1
fi

county_district=$1

select="	street_name,		\
		city,			\
		state_code"

from="street"
where="county_district = $county_district"
statement="select $select from $from where $where;"

echo "$statement"		|
sql.e				|
cat

exit 0
