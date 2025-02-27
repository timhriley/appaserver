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

/usr2/ufw/ufw_whitelist.sh $ip_address

# If not shell okay
# -----------------
if [ $? -ne 0 ]
then
	# Skip 4 friendly allows
	# ----------------------
	sudo ufw insert 5 deny from $ip_address
fi

exit 0
