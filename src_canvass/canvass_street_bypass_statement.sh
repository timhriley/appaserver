:
# $APPASERVER_HOME/src_canvass/canvass_street_bypass_statement.sh
# -------------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# -------------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage `basename.e $0 n` canvass minimum_votes_per_election" 1>&2
	exit 1
fi

canvass="$1"
minimum_votes_per_election=$2

join="	canvass_street.street_name = street.street_name and	\
	canvass_street.city = street.city and			\
	canvass_street.state_code = street.state_code"

subquery="	select 1				\
		from street				\
		where $join and				\
			ifnull(votes_per_election,0) <	\
			$minimum_votes_per_election"

where="	canvass = '$canvass' and	\
	canvass_date is null and	\
	exists ($subquery)"

echo "update canvass_street set action = 'bypass' where $where;"

exit 0
