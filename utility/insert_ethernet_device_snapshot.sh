:
# $APPASERVER_HOME/utility/insert_ethernet_device_snapshot.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application interface" 1>&2
	exit 1
fi


application=$1
interface=$2

results=`ethernet_bytes.sh $interface`
IP_address=`echo $results | column.e 0`
receive_bytes=`echo $results | column.e 1`
transmit_bytes=`echo $results | column.e 2`

megabytes=1000000
receive_megabytes=$((receive_bytes / megabytes))
receive_megabytes=$((receive_megabytes + 1))

transmit_megabytes=$((transmit_bytes / megabytes))
transmit_megabytes=$((transmit_megabytes + 1))
date=`/bin/date '+%F'`

table=ethernet_device_snapshot
field="date,IP_address,transmit_megabytes,receive_megabytes"

echo "$date|${IP_address}|${transmit_megabytes}|${receive_megabytes}"									|
insert_statement.e table=$table field=$field		|
sql.e $application 2>&1					|
grep -vi duplicate

exit 0
