:
# ------------------------------------
# $APPASERVER_HOME/utility/ufw_deny.sh
# ------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 ip_address" 1>&2
	exit 1
fi

# Skip 4 friendly allows
# ----------------------
sudo ufw insert 5 deny from $1

exit 0
