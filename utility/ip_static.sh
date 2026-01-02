:
# ----------------------------------------------
# $APPASERVER_HOME/utility/ip_static.sh
# ----------------------------------------------

if [ $# -ne 3 ]
then
	echo "`basename.e $0 n` device ip_address gateway" 1>&2
	exit 1
fi

device=$1
ip_address=$2
gateway=$3

# Hints:
# nmcli device status
# nmcli connection show
# ip link show
# ip addr
# ip route

ip_identity.sh $device

if [ $? -ne 0 ]
then
	echo "`basename.e $0 n` ip_identity.sh $device failed" 1>&2
	exit 1
fi

sudo nmcli c mod $device ipv4.addresses ${ip_address}/24

if [ $? -ne 0 ]
then
	echo "`basename.e $0 n` ipv4.addresses ${ip_address} failed" 1>&2
	exit 1
fi

sudo nmcli c mod $device ipv4.method manual

if [ $? -ne 0 ]
then
	echo "`basename.e $0 n`: ipv4.method manual failed" 1>&2
	exit 1
fi

sudo nmcli c mod $device ipv4.gateway $gateway

if [ $? -ne 0 ]
then
	echo "`basename.e $0 n` ipv4.gateway $gateway failed" 1>&2
	exit 1
fi

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
