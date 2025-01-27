:
if [ $# -ne 1 ]
then
	echo "Usage: $0 ip_address" 1>&2
	exit 1
fi

ip_address=$1

echo $ip_address						|
breakin_whitelist.e						|
xargs.e "sudo ufw insert 1 deny from {}; sudo rm /tmp/dos-{}"

exit 0
