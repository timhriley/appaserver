:
# $APPASERVER_HOME/utility/ethernet_bytes.sh
# --------------------------------------------------------------- 
# No warranty and freely available software. Visit appaserver.org
# --------------------------------------------------------------- 

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 ethernet_interface" 1>&2
	exit 1
fi

interface=$1

results=`/sbin/ifconfig "$interface"	|
	 grep -v ':'			|
	 joinlines.e ';'		|
	 cat`

line1=`	echo "$results"			| 
	piece.e ';' 0`

ip_address=`	echo $line1		| 
		column.e 1`

line2=`	echo "$results"			| 
	piece.e ';' 1`

receive_bytes=`	echo "$line2"		| 
		column.e 4`

line4=`	echo "$results"			| 
	piece.e ';' 3`

transmit_bytes=`echo "$line4"		| 
		column.e 4`

echo $ip_address $receive_bytes $transmit_bytes

exit 0
