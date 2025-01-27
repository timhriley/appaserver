:
# $APPASERVER_HOME/utility/iotop_average_loop.sh
# ----------------------------------------------
# Freely available software: see Appaserver.org
# ----------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: `echo $0 | basename.e` read|write" 1>&2
	exit 1
fi

operation=$1

while [ true ]
do
	iotop_average.sh $operation		|
	tail -1					|
	piece.e '|' 1				|
	commas_in_long.e			|
	cat
done

exit 0
