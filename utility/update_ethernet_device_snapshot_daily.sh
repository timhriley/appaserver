:
# $APPASERVER_HOME/utility/update_ethernet_device_snapshot_daily.sh
# -----------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# -----------------------------------------------------------------

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 application interface [starting_date]" 1>&2
	exit 1
fi

application=$1
interface=$2

if [ "$#" -eq 3 ]
then
	starting_date=$3
else
	starting_date=`date.e -3`
fi

results=`ethernet_bytes.sh $interface`
IP_address=`echo $results | column.e 0`

echo "	select date,ip_address,receive_megabytes,transmit_megabytes
	from ethernet_device_snapshot
	where date >= '$starting_date';"				|
sql.e $application							|
awk '	BEGIN {
		FS = "^"
		first_time = 1
	}
	{
		date = $1
		ip_address = $2
		receive_megabytes = $3
		transmit_megabytes = $4

		if ( first_time )
		{
			first_time = 0
			prior_receive_megabytes = receive_megabytes
			prior_transmit_megabytes = transmit_megabytes
		}
		else
		{
			receive_daily_megabytes = \
				receive_megabytes - \
				prior_receive_megabytes

			if ( receive_daily_megabytes < 0 )
			{
				receive_daily_megabytes = receive_megabytes
			}

			transmit_daily_megabytes = \
				transmit_megabytes - \
				prior_transmit_megabytes

			if ( transmit_daily_megabytes < 0 )
			{
				transmit_daily_megabytes = transmit_megabytes
			}

			printf( "%s^%s^receive_daily_megabytes^%d\n",
				date,
				ip_address,
				receive_daily_megabytes )

			printf( "%s^%s^transmit_daily_megabytes^%d\n",
				date,
				ip_address,
				transmit_daily_megabytes )

			prior_receive_megabytes = receive_megabytes
			prior_transmit_megabytes = transmit_megabytes
		}
	}'								|
update_statement.e	table=ethernet_device_snapshot			\
			key=date,ip_address				\
			carrot=y					|
sql.e $application

exit 0
