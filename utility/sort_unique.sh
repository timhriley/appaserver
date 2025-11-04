:
# $APPASERVER_HOME/utility/sort_unique.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: `echo $0 | basename.e` delimiter column1 column2" 1>&2
	exit 1
fi

delimiter=$1
column1=$2
column2=$3

sort -t $delimiter -k $column1,$column2 -u

exit 0
