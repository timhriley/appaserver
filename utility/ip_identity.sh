:
# ----------------------------------------------
# $APPASERVER_HOME/utility/ip_identity.sh
# ----------------------------------------------

if [ $# -ne 1 ]
then
	echo "`basename.e $0 n` device" 1>&2
	exit 1
fi

device=$1

identity=`nmcli device status | grep "^$device" | cut -c 32-99 | trim.e`

if [ "$identity" = "" ]
then
	echo "Cannot discern the identity name." 1>&2
	exit 1
fi

if [ "$identity" != "$device" ]
then
	sudo nmcli connection modify "$identity" connection.id $device

	if [ $? -ne 0 ]
	then
		echo "`basename.e $0 n`: connection modify failed" 1>&2
		exit 1
	fi

	sudo nmcli c mod $device connection.interface-name $device

	if [ $? -ne 0 ]
	then
		echo "`basename.e $0 n`: connection.interface-name failed" 1>&2
		exit 1
	fi
fi

exit 0
