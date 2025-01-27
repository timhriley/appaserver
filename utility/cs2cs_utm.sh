:
# utility/cs2cs_utm.sh
# --------------------
# This script uses the cs2cs utility to convert utm to latitude/longitude.
# Usage: echo "utm_easting utm_northing" | cs2cs_utm.sh
# ------------------------------------------------------------------------

if [ "$#" -eq 1 ]
then
	if [ "$1" != "minutes" ]
	then
		echo "Usage: $0 ['minutes'] [zone]" 1>&2
		exit 1
	fi

	minutes_process="awk"

else
	minutes_process="cat"
fi

if [ "$#" -eq 2 ]
then
	zone=$2
else
	zone=17
fi

if [ "$minutes_process" = "awk" ]
then
	cs2cs					\
			+proj=utm		\
			+north			\
			+zone=$zone		\
			+to			\
			+proj=latlong		\
			+datum=NAD83		\
			-f "%.5f"		|
	column.e 0,1				|
	sed 's/|/ /'				|
	sed 's/\./ ./g'				|
	/usr/bin/awk '
{ 
	latitude_degrees = $1
	latitude_float = $2
	latitude_float *= 60.0
	longitude_degrees = $3
	longitude_float = $4
	longitude_float *= 60.0
	printf( "%d %.5lf %d %.5lf\n", latitude_degrees, latitude_float, longitude_degrees, longitude_float )
} '
else
	cs2cs					\
			+proj=utm		\
			+north			\
			+zone=$zone		\
			+to			\
			+proj=latlong		\
			+datum=NAD83		\
			-f "%.5f"		|
	column.e 0,1				|
	sed 's/|/ /'				|
	cat
fi

exit 0
