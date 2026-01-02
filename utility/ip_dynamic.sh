:
# ----------------------------------------------
# $APPASERVER_HOME/utility/ip_dynamic.sh
# ----------------------------------------------

if [ $# -ne 1 ]
then
	echo "`basename.e $0 n` device" 1>&2
	exit 1
fi

device=$1

ip_identity.sh $device

if [ $? -ne 0 ]
then
	echo "`basename.e $0 n` ip_identity.sh $device failed" 1>&2
	exit 1
fi

sudo nmcli c mod $device ipv4.method auto

if [ $? -ne 0 ]
then
	echo "`basename.e $0 n`: ipv4.method auto failed" 1>&2
	exit 1
fi

#sudo nmcli c mod $device ipv4.addresses ""
#
#if [ $? -ne 0 ]
#then
#	echo "`basename.e $0 n` ipv4.addresses failed" 1>&2
#	exit 1
#fi
#
#sudo nmcli c mod $device ipv4.gateway ""
#
#if [ $? -ne 0 ]
#then
#	echo "`basename.e $0 n` ipv4.gateway failed" 1>&2
#	exit 1
#fi

sudo nmcli c mod $device connection.autoconnect yes

if [ $? -ne 0 ]
then
	echo "`basename.e $0 n` connection.autoconnect yes failed" 1>&2
	exit 1
fi

sudo systemctl restart systemd-networkd

if [ $? -ne 0 ]
then
	echo "`basename.e $0 n` systemctl restart systemd-networkd failed" 1>&2
	exit 1
fi

exit 0
