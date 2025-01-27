#!/bin/bash
# ---------------------------------------------
# $APPASERVER_HOME/utility/flock.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ $# -lt 2 ]
then
	echo "Usage: $0 key process [output_file]" 1>&2
	exit 1
fi

key=$1
process=$2

if [ $# -eq 3 ]
then
	output_process="| cat > $3"
else
	output_process="| cat"
fi

execute_process="$process $output_process"

semaphore_file="/var/lock/flock_${key}"
seconds_wait=10

(
	flock -x -w ${seconds_wait} 200 || exit 1

	eval $execute_process

) 200>$semaphore_file

rm -f $semaphore_file

exit 0
