#!/bin/sh
# ---------------------------------------------
# discovery_census_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

#days_ago=1000

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

discovery_census=`get_table_name $application alligator_census`

#minimum_discovery_date=`now.sh ymd -${days_ago}`
#where_clause="discovery_date >= '${minimum_discovery_date}'"
where_clause="1 = 1 "

echo "	select discovery_date, primary_researcher			\
	from $discovery_census						\
	where $where_clause;"						|
sql.e									|
sort_alligator_census.e

exit 0
