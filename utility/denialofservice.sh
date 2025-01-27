:
deny_process="cat"
display_process="less"

if [ "$#" -eq 1 ]
then
	if [ "$1" != "withdeny" -a "$1" != "cat" ]
	then
		echo "Usage $0 [withdeny|cat]" 1>&2
		exit 1
	fi
fi

if [ "$1" = "cat" ]
then
	display_process="cat"
fi

for ip_address in `ls -1 /tmp/dos-* 2>/dev/null	| piece_inverse.e 0  '-'`
do
	if [ "$1" = "withdeny" ]
	then
		denialofservice_deny.sh $ip_address
	else
		(
			echo "IP Address: $ip_address"
			grep $ip_address /var/log/apache2/access.log
		) | $display_process
	fi
done

exit 0
