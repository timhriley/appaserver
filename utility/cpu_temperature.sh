:
# $APPASERVER_HOME/utility/cpu_temperature.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

# -------------------------
# apt -y install lm-sensors
# sensors-detect
# -------------------------

sleep_seconds=10

sensors | egrep 'Physical id|Package id' | column.e 2,3

if [ "$1" = "loop" ]
then
	sleep $sleep_seconds
	exec $0 $1
fi

