:
# $APPASERVER_HOME/utility/memory_used.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

sleep_seconds=10

free -b | grep 'Mem:' | column.e 2 | commas_in_long.e

if [ "$1" = "loop" ]
then
	sleep $sleep_seconds
	exec $0 $1
fi

exit 0
