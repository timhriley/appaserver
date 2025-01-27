:
# ----------------------------------------------
# $APPASERVER_HOME/utility/ufw_blacklist_deny.sh
# ----------------------------------------------

ufw_blacklist.e					|
grep -v '^$'					|
xargs.e ufw_deny.sh {}

exit 0
