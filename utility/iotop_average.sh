:
# $APPASERVER_HOME/utility/iotop_average.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: `echo $0 | basename.e` seconds" 1>&2
	exit 1
fi

seconds=$1

iotop.sh 1 $seconds n		|
grep 'Total DISK READ'		|
column.e 4			|
grep -v '0.00'			|
average.e '|'			|
cat

exit $?
