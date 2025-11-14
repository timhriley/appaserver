:
# $APPASERVER_HOME/utility/iotop.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

flags="-o -b -n1 -k"

if [ "$#" -ne 2 ]
then
	echo "Usage: `echo $0 | basename.e` seconds iterations" 1>&2
	exit 1
fi

seconds=$1
iterations=$2

decimal_count.e $iterations |
while read ignore
do
	sudo iotop $flags | trim.e 80

	sleep $seconds
	echo
done

exit 0
