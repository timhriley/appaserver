:
# $APPASERVER_HOME/utility/iotop_average_loop.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: `echo $0 | basename.e` read|write" 1>&2
	exit 1
fi

operation=$1

seconds=5
iterations=10

while [ true ]
do
	iotop_average.sh $operation $seconds $iterations	|
	tail -1							|
	piece.e '|' 1						|
	commas_in_long.e					|
	cat
done

exit 0
