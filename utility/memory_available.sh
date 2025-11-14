:
# $APPASERVER_HOME/utility/memory_available.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

sleep_seconds=10

top -b -Em | head -5 | tail -1 | column.e 8

if [ "$1" = "loop" ]
then
	sleep $sleep_seconds
	exec $0 $1
fi

exit 0
