#!/bin/bash
# ---------------------------------------
# $APPASERVER_HOME/utility/killprocess.sh
# ---------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: sudo `basename.e $0 n` username process execute_yn" 1>&2
	exit 1
fi

if [ $UID -ne 0 ]
then
	sudo -p "Restarting with sudo. Password: " bash $0 $*
	exit $?
fi

username=$1
process_name=$2
execute_yn=$3

if [ "$execute_yn" = "y" ]
then
	kill_process="xargs -t -n1 kill"
else
	kill_process="xargs -t -n1 echo kill"
fi

sort_command="sort -n"
process_list="ps -fu $username | grep -v grep | grep $process_name"

eval "$process_list"		|
grep -v PID			|
grep -v killprocess		|
#tee /dev/tty			|
awk '{print $2}'		|
$sort_command			|
$kill_process

exit 0
