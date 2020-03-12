:
# src_alligator/group_annually.sh
# ---------------------------------------------
# Freely available software. See appaserver.org
# ---------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application basin" 1>&2
	exit 1
fi

application=$1
basin=$2

if [ "$basin" = "basin" -o "$basin" = "" ]
then
	basin_in_clause=""
else
	basin_single_quotes=`single_quotes_around.e "$basin"`
	basin_in_clause="and basin in ($basin_single_quotes)"
fi

table_name=`get_table_name $application nest`

echo "	select	year( discovery_date ),
     		count(*),
		sum( total_eggs ),
		sum( total_eggs ) / count(*)
	from $table_name
	where total_eggs is not null
	  and total_eggs > 0
	  $basin_in_clause
	group by year( discovery_date );"		|
sql.e ','

#echo "	select	year( discovery_date ),
#     		count(*),
#		sum( total_eggs ),
#		sum( total_eggs ) / count(*)
#	from $table_name
#	where 1 = 1
#	  $basin_in_clause
#	group by year( discovery_date );"		|
#sql.e ','

exit 0
