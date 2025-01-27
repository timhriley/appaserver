:
# $APPASERVER_HOME/utility/iotop_average.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: `echo $0 | basename.e` read|write" 1>&2
	exit 1
fi

operation=$1

iterations=10
seconds=5

if [ "$operation" = "read" ]
then
	grab=0
else
	grab=1
fi

iotop.sh $seconds $iterations n	|
grep -i '^total'		|
piece.e '|' $grab		|
piece.e ':' 1			|
column.e 0			|
grep -v '0.00'			|
average.e '|'			|
cat

exit $?
