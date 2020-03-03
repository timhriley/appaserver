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

station=$1
parameter=$2

select="station,parameter,units"
where="station = '$station' and parameter = '$parameter'"

echo "select $select from station_parameter where $where;"	|
sql.e waterquality						|
sort

exit 0
