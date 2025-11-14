:
# $APPASERVER_HOME/utility/iotop_average.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: `echo $0 | basename.e` read|write seconds iterations" 1>&2
	exit 1
fi

operation=$1
seconds=$2
iterations=$2

if [ "$operation" = "read" ]
then
	grab=0
else
	grab=1
fi

iotop.sh $seconds $iterations 	|
grep -i '^total'		|
piece.e '|' $grab		|
piece.e ':' 1			|
column.e 0			|
# Don't let sleeping skew
grep -v '0.00'			|
average.e '|'			|
cat

exit $?
