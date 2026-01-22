:
# $APPASERVER_HOME/src_canvass/street_votes_per_election_statement.sh
# -------------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# -------------------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage `basename.e $0 n` county_district" 1>&2
	exit 1
fi

table="street"
key="street_name,city,state_code"

street_votes_per_election $1				|
sed 's/\^/^votes_per_election^/3'			|
update_statement.e table=$table key=$key carrot=y	|
cat

exit 0
