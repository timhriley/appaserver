:
# ------------------------------------
# $APPASERVER_HOME/utility/ufw_deny.sh
# ------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 ip_address" 1>&2
	exit 1
fi

ip_address=$1

ufw_whitelist.sh $ip_address

# If not shell okay
# -----------------
if [ $? -ne 0 ]
then
	whitelist_count=`ufw_whitelist.e | wc -l`
	whitelist_skip=`expr ${whitelist_count} + 1`
	sudo ufw insert $whitelist_skip deny from $ip_address
fi

exit 0
