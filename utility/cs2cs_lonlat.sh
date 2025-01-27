:
# $APPASERVER_HOME/utility/cs2cs_lonlat.sh
# -------------------------------------------------------------
# This script uses the cs2cs utility to convert lon/lat to UTM.
# Usage: echo "longitude latitude" | cs2cs_lonlat.sh [zone]
# -------------------------------------------------------------

if [ "$#" -eq 1 ]
then
	zone=$1
else
	zone=10
fi

cs2cs					\
		+proj=latlong		\
		+datum=NAD83		\
		+to			\
		+proj=utm		\
		+zone=$zone		\
		+north			\
		-f "%.0f"		|
column.e 0,1				|
sed 's/|/ /'				|
#sed 's/14935104.63779 0.00000//'	|
cat

exit 0
