:
# $APPASERVER_HOME/utility/iotop_average.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: `echo $0 | basename.e` iterations read|write" 1>&2
	exit 1
fi

iterations=$1
operation=$2

seconds=5

if [ "$operation" = "read" ]
then
	grab=4
else
	grab=11
fi

iotop.sh $seconds $iterations n	|
grep 'Total DISK READ'		|
column.e $grab			|
grep -v '0.00'			|
average.e '|'			|
cat

exit $?
