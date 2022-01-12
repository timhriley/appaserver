:

#days_ago=270

if [ $# -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

#minimum_discovery_date=`now.sh ymd -${days_ago}`
#where_clause="discovery_date >= '${minimum_discovery_date}'"
where_clause="1 = 1"

table_name=`get_table_name $application nest`

echo "	select nest_number				\
	from $table_name				\
	where $where_clause;"				|
sql.e							|
sort_delimited_numbers.e '-' y

exit 0

