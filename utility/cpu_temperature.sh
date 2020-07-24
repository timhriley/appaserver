:
# -------------------------
# apt -y install ln-sensors
# sensors-detect
# -------------------------
sensors | egrep 'Physical id|Package id' | column.e 2,3

if [ "$1" = "loop" ]
then
	sleep 5
	exec $0 $1
fi

