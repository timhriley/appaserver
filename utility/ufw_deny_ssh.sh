:
# ------------------------------------
# $APPASERVER_HOME/utility/ufw_deny.sh
# ------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 ip_address" 1>&2
	exit 1
fi

sudo ufw insert 1 deny from $1 to any port ssh

exit 0
