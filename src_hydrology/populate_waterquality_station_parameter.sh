:
# src_hydrology/populate_waterquality_station_parameter.sh
# --------------------------------------------------------
# Freely available software. See appaserver.org
# --------------------------------------------------------

if [ $# -ne 2 ]
then
	echo "Usage: $0 station parameter" 1>&2
	exit 1
fi

station_list=$1
parameter_list=$2

select="station,parameter,units"

station_single_quotes=`single_quotes_around.e "$station_list"`
station_in_clause="and station in ($station_single_quotes)"

parameter_single_quotes=`single_quotes_around.e "$parameter_list"`
parameter_in_clause="and parameter in ($parameter_single_quotes)"

where="1 = 1 $station_in_clause $parameter_in_clause"

echo "select $select from station_parameter where $where;"	|
sql.e waterquality						|
sort

exit 0
